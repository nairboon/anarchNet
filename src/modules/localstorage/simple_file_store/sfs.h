#ifndef PLG_SFS_H
#define PLG_SFS_H

#include "plugins/localstorage.h"

#define PLG_SFS_VERSION 1

class SFS : public an::plg::LocalStorage
{
public:
	const std::string getName() {return "simple_file_store";}
	const unsigned int getVersion() { return PLG_SFS_VERSION; }
	const StorageType getType() { return LocalStorage::BINARY; }
	bool initialise();
	void shutdown();
	
	bool store_file(const std::string& path, std::string& res);
	bool get_file_path(const std::string& id,std::string& res);
	bool remove_file(const std::string& id);
private:
	std::string hash_to_path(const std::string& inp);
	std::string _path;
};

class SFSDriver : public an::plgdrv::LocalStorage
{
public: 
	const std::string getName() { return "simple_file_store_driver"; }
	SFS* createPlugin() {return new SFS();}
};

#endif // PLG_SFS_H