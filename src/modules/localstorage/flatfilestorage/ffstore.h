#ifndef PLG_FLATFILESTORE_H
#define PLG_FLATFILESTORE_H

#include "plugins/localstorage.h"

#define PLG_FLATFILESTORE_VERSION 1

class FFStore : public an::plg::LocalStorage
{
public:
	const std::string getName() {return "ffstore";}
	const unsigned int getVersion() { return PLG_FLATFILESTORE_VERSION; }
	bool initialise();
	void shutdown();
	
	bool kv_put(const std::string& key, const std::string& value);
	bool kv_get(const std::string& id, an::KV_ResPtr& res);
	bool kv_remove(const std::string& id);
private:
  	std::string hash_to_path(const std::string& inp);
	std::string validate_key(std::string key);
		bool store_block(const std::string& content);
	std::string _path;
	std::string _link_dir;
	std::string _data_dir;
};

class FFStoreDriver : public an::plgdrv::LocalStorage
{
public: 
	const std::string getName() { return "FFstore_driver"; }
	FFStore* createPlugin() {return new FFStore();}
};

#endif // PLG_FLATFILESTORE_H