#include "logger.h"
#include "puggKernel.h"
#include "db.h"
#include "ffstore.h"
#include "crypto.h"
#include "config_manager.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/smart_ptr.hpp>


namespace fs = boost::filesystem;

using namespace pugg;

extern "C" //__declspec(dllexport)
void registerPlugin(Kernel &K) 
{ 
	Server<an::plgdrv::LocalStorage>* server = CastToServerType<an::plgdrv::LocalStorage>(K.getServer(PLG_LOCALSTORAGE_SERVER_NAME));
	assert(server != NULL);
	server->addDriver(new FFStoreDriver(),PLG_LOCALSTORAGE_SERVER_VERSION);
}


bool FFStore::initialise() {
  
  an::ModuleManager::instance().kv_put.connect(boost::bind(&FFStore::kv_put,this,_1,_2));
  
 if(!get_config()) {
    LOG(ERROR) << "could not load config";
   // return false;
  }
  _path = an::ConfigManager::instance().datadir() + "/ffstore";
  if(!fs::exists(_path)) {
    if(!fs::create_directory(_path)) {
      LOG(ERROR) << "could not create filestore directory";
      return false;
    }
  }
  _path += "/";
  
  _link_dir = _config.get("linkdir",_path + "link");
  if(!fs::exists(_link_dir)) {
    if(!fs::create_directory(_link_dir)) {
      LOG(ERROR) << "could not create filestore directory: " << _link_dir;
      return false;
    }
  }
  _link_dir += "/";
  
  _data_dir = _config.get("datadir",_path + "raw");
  if(!fs::exists(_data_dir)) {
    if(!fs::create_directory(_data_dir)) {
      LOG(ERROR) << "could not create filestore directory: " << _data_dir;
      return false;
    }
  }
  _data_dir += "/";
return true;
}

void FFStore::shutdown() {
	
}


bool FFStore::kv_put(const std::string& _key, const std::string& value)
{
  LOG(INFO) << "kv_put: " << _key;
  std::string key =  validate_key(_key);
  std::string hash = an::crypto::toHex(an::crypto::Hash(value));
  
  
if(!store_block(value))
   return false;
	
//  if(key == hash) //unique store
//      return false;
  
  // create link
  
  fs::path ppath = fs::path(_link_dir + hash_to_path(key)+"/x").remove_filename();
  if(!fs::exists(ppath))
    if(!fs::create_directories(ppath)) {
      LOG(ERROR) << "could not create directories" << std::endl;
      return false;
    }
  //if(!
  fs::path symlink = _link_dir+hash_to_path(key)+"/"+hash;
  if(!fs::exists(symlink))
    fs::create_symlink(_data_dir+hash_to_path(hash), symlink );/*) {
    LOG(ERROR) << "could not create symlink " << _link_dir+hash_to_path(key)+"/"+hash;
    return false;
  }*/
  return true;
}

bool FFStore::store_block(const std::string& content)
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

bool FFStore::kv_get(const std::string& _key,an::KV_ResPtr& res)
{
  std::string key =  validate_key(_key);
  
  res = an::KV_ResPtr(new an::KV_ResMap);
  
  try {
    std::string mid_d = _link_dir+hash_to_path(key);
    if(!fs::is_directory(mid_d)) {
      LOG(ERROR) << "not a directory " << _link_dir+hash_to_path(key);
      return false;
    }
    
    fs::directory_iterator dir(mid_d),end;
    BOOST_FOREACH(const fs::path& p, std::make_pair(dir, end)) {
      fs::ifstream file(p,std::ios::in|std::ios::binary|std::ios::ate);
      if(!file.good()) {
	LOG(ERROR) << "could not open: " << p.native();
	return false;
      }
      uint size = fs::file_size(p);
      char *buffer = new char [size];
      file.seekg (0, std::ios::beg);
      file.read (buffer, size);
      file.close();
      std::string cont(buffer,size);
      delete[] buffer;
      //LOG(INFO) << "added: " << cont <<" << " << an::crypto::toHex(an::crypto::Hash(cont));
      res->insert(std::make_pair(p.filename().native(),cont));
      
      return true;
    }
    
  }
  catch (const fs::filesystem_error& ex) {
    LOG(ERROR) << "fs error: "<< ex.what();
    return false;
  }
  
}

bool FFStore::kv_remove(const std::string& _key)
{
  std::string key =  validate_key(_key);
  
  if(!fs::remove_all(_link_dir + hash_to_path(key))) {
   // LOG(INFO) << "could not remove unique file, mid key?";
    
    fs::directory_iterator dir(_link_dir + hash_to_path(key)),end;
    BOOST_FOREACH(const fs::path& p, std::make_pair(dir, end)) {
      if(!fs::remove(p)) {
	LOG(ERROR) << "could not remove: "<< p.native();
	return false;
      }
      
    }
    if(!fs::remove(_link_dir + hash_to_path(key))) {
      LOG(ERROR) << "could not mid remove dir";
      return false;
    }
    
  }
  return true;
}

std::string FFStore::hash_to_path(const std::string& inp)
{
  std::string res = inp;
  res.insert(4,"/").insert(2,"/");
  return res;
}

std::string FFStore::validate_key(std::string key)
{
 // LOG(INFO) << "validate: " << key;
  if(key.size() < 128) // hash it
    return an::crypto::toHex(an::crypto::Hash(key));
  else
    return key;
}