#include "logger.h"
#include "puggKernel.h"
#include "db.h"
#include "sfs.h"
#include "crypto.h"
#include "config_manager.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/log/common.hpp>


namespace fs = boost::filesystem;

using namespace pugg;

#define PATH "/sqlite_store.db"
extern "C" //__declspec(dllexport)
void registerPlugin(Kernel &K) 
{
	Server<an::plgdrv::LocalStorage>* server = CastToServerType<an::plgdrv::LocalStorage>(K.getServer(PLG_LOCALSTORAGE_SERVER_NAME));
	assert(server != NULL);
	server->addDriver(new SFSDriver(),PLG_LOCALSTORAGE_SERVER_VERSION);
}

bool SFS::initialise() {
	_path = an::ConfigManager::instance().datadir() + "/filestore";
	if(!fs::exists(_path)) {
		if(!fs::create_directory(_path)) {
			LOG(ERROR) << "could not create filestore directory";
			return false;	
		}
	}
	_path += "/";
	return true;
}
void SFS::shutdown() {}

bool SFS::store_file(const std::string& path, std::string& res) 
{ 
	BOOST_LOG_SEV(an::log::Logger::instance().get(),INFO) << "test";
	
	std::string hash = an::crypto::toHex(an::crypto::HashFile(path));
	std::string hpath =hash_to_path(hash);
	try {
		if(fs::exists(_path + hpath)) {
			LOG(INFO) << "file already stored: " << hash;
			res = hash;
			return true;
		}
		fs::path ppath = fs::path(_path + hpath).remove_filename();
		LOG(INFO) << "from: " << path << " to: " << _path + hpath << std::endl;
		if(!fs::exists(ppath)) 
		if(!fs::create_directories(ppath)) {
			LOG(ERROR) << "could not create directories" << std::endl;
			return false;
		}
			
		fs::copy_file(path,_path + hpath);
	}
	catch (const fs::filesystem_error& ex) {
				LOG(ERROR) << "could not copy file"<< ex.what();
		return false; 
	}
	res = hash;
	return true;
}

bool SFS::get_file_path(const std::string& id,std::string& res) 
{ 
	res = _path + hash_to_path(id);
	return true;
}

bool SFS::remove_file(const std::string& id) 
{ 
	if(!fs::remove(_path + hash_to_path(id))) {
		LOG(ERROR) << "could not remove file";
		return false; 
	}
	return true;
}

std::string SFS::hash_to_path(const std::string& inp)
{
	std::string res = inp;
	res.insert(4,"/").insert(2,"/");
	return res;
}
