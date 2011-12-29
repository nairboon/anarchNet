#include "logger.h"
#include "puggKernel.h"
#include "db.h"
#include "config_manager.h"
#include "blockstore.h"
#include "crypto.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/log/common.hpp>


namespace fs = boost::filesystem;

using namespace pugg;


extern "C" //__declspec(dllexport)
void registerPlugin(Kernel &K) 
{
  LOG(INFO) << "regisster";
  
	Server<an::plgdrv::LocalStorage>* server = CastToServerType<an::plgdrv::LocalStorage>(K.getServer(PLG_LOCALSTORAGE_SERVER_NAME));
	assert(server != NULL);
	server->addDriver(new BlockstoreDriver(),PLG_LOCALSTORAGE_SERVER_VERSION);
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

	_data_dir = _config.get("datadir",_path + "data");
	if(!fs::exists(_data_dir)) {
		if(!fs::create_directory(_data_dir)) {
			LOG(ERROR) << "could not create filestore directory: " << _data_dir;
			return false;	
		}
	}
	
	_blocksize = _config.get("blocksize",128) /* in kB */ * 1024;
	
	_block_cache = boost::shared_ptr<block_cache_type>(new block_cache_type(boost::bind(&Blockstore::load_block,this,_1),_config.get("block_cache_size",512)));
	_pubfile_cache = boost::shared_ptr<file_cache_type>(new file_cache_type(boost::bind(&Blockstore::load_file,this,_1),_config.get("file_cache_size",16)));
	
	return true;
}

smart_block Blockstore::load_block(id& bid)
{
}

smart_pf Blockstore::load_file(id& fid)
{
}
void Blockstore::shutdown() {}

bool Blockstore::store_file(const std::string& path, std::string& res) 
{ 
	BOOST_LOG_SEV(an::log::Logger::instance().get(),INFO) << "test";
	
	std::string hash = an::crypto::toHex(an::crypto::HashFile(path));
	std::string hpath =hash_to_path(hash);
	
    try {
      	uint filesize = fs::file_size(path);
	uint blocks = ceil(filesize / _blocksize);
	LOG(INFO) << "storing in " << blocks << " blocks";
	
		if(fs::exists(_path + hpath)) {
			LOG(INFO) << "file already stored: " << hash;
			res = hash;
			return true;
		}
		
		fs::path hf = fs::path(_path + hpath);
		fs::path ppath = hf.remove_filename();
		LOG(INFO) << "from: " << path << " to: " << _path + hpath << std::endl;
		if(!fs::exists(ppath)) 
		if(!fs::create_directories(ppath)) {
			LOG(ERROR) << "could not create directories" << std::endl;
			return false;
		}
		
		fs::ofstream header_file(hf,std::ios::binary);
		if(!header_file.good()) {
		LOG(ERROR) << "could not open " << hf;
		  return false;
		}
		header_file.write((char *)filesize,sizeof(long));
		
		fs::ifstream input_file(path,std::ios::binary);
		if(!input_file.good()) {
		LOG(ERROR) << "could not open " << hf;
		  header_file.close();
		  return false;
		}
		uint readbytes = 0;
		boost::shared_array<char> buffer = boost::shared_array<char>(new char[_blocksize]);
		std::string hash,block_path;
		do {
		  input_file.read(buffer.get(),_blocksize);
		  hash = an::crypto::Hash(buffer.get());
		  block_path = hash_to_path(hash);
		  {
		    fs::ofstream block_file(_path + block_path,std::ios::binary);
		    if(!input_file.good()) {
		      LOG(ERROR) << "could not open " << hf;
		      header_file.close();
		      input_file.close();
		      return false;
		    }
		    block_file.write(buffer.get(),_blocksize);
		    block_file.close();
		  }
		  header_file.write(hash.c_str(),512);
		  
		} while(readbytes < filesize);
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

bool Blockstore::get_file_path(const std::string& id,std::string& res) 
{ 
	res = _path + hash_to_path(id);
	return true;
}

bool Blockstore::remove_file(const std::string& id) 
{ 
	if(!fs::remove(_path + hash_to_path(id))) {
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
