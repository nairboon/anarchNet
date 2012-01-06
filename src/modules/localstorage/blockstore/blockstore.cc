#include "logger.h"
#include "puggKernel.h"
#include "db.h"
#include "config_manager.h"
#include "blockstore.h"
#include "exception.h"
#include "crypto.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/foreach.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/log/common.hpp>


namespace fs = boost::filesystem;

using namespace pugg;


extern "C" //__declspec(dllexport)
void registerPlugin(Kernel &K) 
{
  LOG(INFO) << "register";
  
	Server<an::plgdrv::LocalStorage>* server = CastToServerType<an::plgdrv::LocalStorage>(K.getServer(PLG_LOCALSTORAGE_SERVER_NAME));
	assert(server != NULL);
	server->addDriver(new BlockstoreDriver(),PLG_LOCALSTORAGE_SERVER_VERSION);
}


public_file::~public_file() {
  if(!fs::remove(path))
    LOG(ERROR) << "could not remove: " << path;
}


bool Blockstore::initialise() {
    if(!get_config()) {
      LOG(ERROR) << "could not load config";
      return false;
    }
	_path = an::ConfigManager::instance().datadir() + "/blockstore";
	if(!fs::exists(_path)) {
		if(!fs::create_directory(_path)) {
			LOG(ERROR) << "could not create filestore directory";
			return false;	
		}
	}
	_path += "/";
	
	_pub_dir = _config.get("publicdir",_path + "public");
	if(!fs::exists(_pub_dir)) {
		if(!fs::create_directory(_pub_dir)) {
			LOG(ERROR) << "could not create filestore directory: " << _pub_dir;
			return false;	
		}
	}
	_pub_dir += "/";

	_data_dir = _config.get("datadir",_path + "data");
	if(!fs::exists(_data_dir)) {
		if(!fs::create_directory(_data_dir)) {
			LOG(ERROR) << "could not create filestore directory: " << _data_dir;
			return false;	
		}
	}
	_data_dir += "/";
	
	_blocksize = _config.get("blocksize",128) /* in kB */ * 1024;
	
	_block_cache = boost::shared_ptr<block_cache_type>(new block_cache_type(boost::bind(&Blockstore::load_block,this,_1),_config.get("block_cache_size",512)));
	_pubfile_cache = boost::shared_ptr<file_cache_type>(new file_cache_type(boost::bind(&Blockstore::load_file,this,_1),_config.get("file_cache_size",16)));
	
	return true;
}

void Blockstore::shutdown() {}

smart_block Blockstore::load_block(id& bid)
{
    try {
  	fs::ifstream ifile(_data_dir+hash_to_path(bid),std::ios::in|std::ios::binary|std::ios::ate);
	if(!ifile.good()) {
	LOG(ERROR) << "could not open " << _data_dir+hash_to_path(bid);
		 throw an::file_not_found();
	}

	std::ifstream::pos_type size;
	 size = ifile.tellg();
	char *buffer = new char [size];
	ifile.seekg (0, std::ios::beg);
	ifile.read (buffer, size);
	ifile.close();
	
	LOG(INFO) << "read: " << buffer;
	
	return smart_block(new block(buffer,size));
  }
	catch (const fs::filesystem_error& ex) {
				LOG(ERROR) << "fs error: "<< ex.what();
		 throw an::file_not_found();
	}


}

smart_pf Blockstore::load_file(id& fid)
{
  try {
  	fs::ifstream ifile(_data_dir+hash_to_path(fid),std::ios::binary);
	if(!ifile.good()) {
	LOG(ERROR) << "could not open " << _data_dir+hash_to_path(fid);
		 throw an::file_not_found();
	}
	
  	fs::ofstream ofile(_pub_dir+fid,std::ios::binary);
	if(!ofile.good()) {
	LOG(ERROR) << "could not open " << _pub_dir+fid;
	  ifile.close();
		 throw an::file_not_found();
	}		
	
	uint filesize;
	ifile.read((char *)&filesize,sizeof(uint));
	if(!boost::math::isfinite<uint>(filesize)) {
	 LOG(ERROR) << "invalid filesize: " << filesize;
	 ofile.close();
	 ifile.close();
	 return smart_pf(new public_file("",false));
	}
	LOG(INFO) << "loading file: " << filesize;
	
	boost::shared_array<char> buffer = boost::shared_array<char>(new char[_blocksize]);
	if(filesize < (_blocksize - sizeof(uint))) { // just read one block
	  ifile.read(buffer.get(),filesize);
	  ofile.write(buffer.get(),filesize);
	  
	}
	else {
	  uint readbytes = 0;

	  
	  boost::shared_array<char> bid = boost::shared_array<char>(new char[HASH_SIZE]);
	  do
	  {
	    ifile.read(bid.get(),HASH_SIZE); //hash
	    if(ifile.eof())
	     break;
	    
	    std::string newifile = hash_to_path(an::crypto::toHex(bid.get()));
	    LOG(INFO) << "loading: " << newifile;
	    fs::ifstream nif(_data_dir+newifile);
	    if(!nif.good()) {
	      LOG(ERROR) << "could not open " << _data_dir+newifile;
		ifile.close();
		ofile.close();
		 return smart_pf(new public_file("",false));
	    }
	      
	    ofile << nif.rdbuf();

	    nif.close();
	    readbytes+=_blocksize;
	    LOG(INFO) << "rb: " << readbytes / 1024;
	  }while(ifile.good());

	}
	
	ifile.close();
	ofile.close();
}
	catch (const fs::filesystem_error& ex) {
				LOG(ERROR) << "fs error: "<< ex.what();
		return smart_pf(new public_file("",false)); 
	}

	return smart_pf(new public_file(_pub_dir+fid));
}


bool Blockstore::store_file(const std::string& path, std::string& res) 
{ 
	BOOST_LOG_SEV(an::log::Logger::instance().get(),INFO) << "test";
	
	std::string hash = an::crypto::toHex(an::crypto::HashFile(path));
	std::string hpath = hash_to_path(hash);
	
    try {
      	uint filesize = fs::file_size(path);
	uint blocks = floor(filesize / _blocksize)+1;
	uint blockc = 1;
	LOG(INFO) << "storing in " << blocks << " blocks ( " << filesize << " : " << _blocksize;
	
		if(fs::exists(_data_dir + hpath)) {
			LOG(INFO) << "file already stored: " << hash;
			res = hash;
			return true;
		}
		
		fs::path hf = fs::path(_data_dir + hpath);
		fs::path ppath = hf.remove_filename();
		LOG(INFO) << "from: " << path << " to: " << _data_dir + hpath << std::endl;
		if(!fs::exists(ppath)) 
		if(!fs::create_directories(ppath)) {
			LOG(ERROR) << "could not create directories" << std::endl;
			return false;
		}
		
		fs::ofstream header_file(_data_dir+hpath,std::ios::binary);
		if(!header_file.good()) {
		LOG(ERROR) << "could not open " << _data_dir+hpath;
		  return false;
		}
		header_file.write(reinterpret_cast<char*>(&filesize),sizeof(uint));
		
		fs::ifstream input_file(path,std::ios::binary);
		if(!input_file.good()) {
		LOG(ERROR) << "could not open " << path;
		  header_file.close();
		  return false;
		}
		
		if(filesize < _blocksize) { // we only need one file
		  header_file << input_file.rdbuf();
		}
		else {
		uint readbytes = 0;
		boost::shared_array<char> buffer = boost::shared_array<char>(new char[_blocksize]);
		std::string hash,block_path;
		do {
		  input_file.read(buffer.get(),_blocksize);
		  readbytes = _blocksize;
		  if(input_file.eof()) {
		    readbytes = abs(filesize - ((blockc-1) * _blocksize));
		    LOG(INFO) << "last bit: " << readbytes;
		    hash = an::crypto::Hash(std::string(buffer.get(),readbytes));
		  }
		  else
		    hash = an::crypto::Hash(buffer.get());
		  block_path = hash_to_path(an::crypto::toHex(hash));
		  LOG(DEBUG) << "write block: " << block_path;
		  {
		    fs::path ppath = fs::path(_data_dir + block_path).remove_filename();
		    if(!fs::exists(ppath)) 
		      if(!fs::create_directories(ppath)) {
			LOG(ERROR) << "could not create directories" << std::endl;
			return false;
		      }
		    fs::ofstream block_file(_data_dir + block_path,std::ios::binary);
		    if(!block_file.good()) {
		      LOG(ERROR) << "block: " << blockc << " could not open " << _data_dir + block_path;
		      LOG(ERROR) << an::crypto::toHex(hash);
		      header_file.close();
		      input_file.close();
		      return false;
		    }
		    block_file.write(buffer.get(),readbytes);
		    block_file.close();
		  }
		  header_file.write(hash.c_str(),512);
		  
		} while(blockc++ < blocks);
		}
		
		input_file.close();
		header_file.close();
	}
	catch (const fs::filesystem_error& ex) {
				LOG(ERROR) << "fs error: "<< ex.what();
		return false; 
	}
	res = hash;
	return true;
}

bool Blockstore::get_file(const std::string& id,std::string& res)
{
	smart_pf file = (*_pubfile_cache)(id);
	if(!file->good)
	  return false;
	res = file->path;
	return true; 
}
bool Blockstore::get_stored_file_path(const std::string& id,std::string& res) 
{ 
	res = _path + hash_to_path(id);
	return true;
}

bool Blockstore::remove_file(const std::string& id) 
{ 
	if(!fs::remove(_data_dir + hash_to_path(id))) {
		LOG(ERROR) << "could not remove file";
		return false; 
	}
	// FIXME delete all blocks
	return true;
}


bool Blockstore::store_block(std::string& content)
{
  std::string hash = an::crypto::toHex(an::crypto::Hash(content));
  
  fs::path ppath = fs::path(_data_dir + hash_to_path(hash)).remove_filename();
  if(!fs::exists(ppath)) 
    if(!fs::create_directories(ppath)) {
      LOG(ERROR) << "could not create directories" << std::endl;
      return false;
    }
    
    
  fs::ofstream file(_data_dir+hash_to_path(hash));
  if(!file.good()) {
    LOG(ERROR) << "could not open " << _data_dir+hash_to_path(hash);
    return false;
  }
  
  file << content;
  file.close();
  return true;
}

bool Blockstore::get_block(const std::string& id,std::string& res)
{
  try {
      smart_block block = (*_block_cache)(id);
      res = std::string(block.get()->data,block.get()->size);
      return true;
  }
  catch(an::file_not_found& e)
  {
    LOG(DEBUG) << e.what();
    return false;
  }
}

bool Blockstore::remove_block(const std::string& id)
{
  	if(!fs::remove(_data_dir + hash_to_path(id))) {
		LOG(ERROR) << "could not remove file";
		return false; 
	}
	return true;
}

std::string Blockstore::hash_to_path(const std::string& inp)
{
	std::string res = inp;
	res.insert(4,"/").insert(2,"/");
	return res;
}
