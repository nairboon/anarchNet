#ifndef PLG_SFS_H
#define PLG_SFS_H

#include <boost/smart_ptr.hpp>
#include "plugins/localstorage.h"
#include "lru_cache.h"


/*
 *  Fileformat
 *  id -> first block (meta block)
 * 
 * 
 * 
 * */
#define PLG_BLOCKSTORE_VERSION 1

typedef const std::string id;
	
class public_file
{
};
typedef boost::shared_ptr<public_file> smart_pf;

class block
{
  
};

typedef boost::shared_ptr<block> smart_block;


class Blockstore : public an::plg::LocalStorage
{
public:
	const std::string getName() {return "blockstore";}
	const unsigned int getVersion() { return PLG_BLOCKSTORE_VERSION; }
	const StorageType getType() { return LocalStorage::BINARY; }
	bool initialise();
	void shutdown();
	
	bool store_file(const std::string& path, std::string& res);
	bool get_file_path(const std::string& id,std::string& res);
	bool remove_file(const std::string& id);
private:
	std::string hash_to_path(const std::string& inp);
	

	smart_block load_block(id& bid);
	smart_pf load_file(id& fid);
	std::string _path;
	std::string _pub_dir;
	std::string _data_dir;
	uint _blocksize;
	
	typedef lru_cache_using_boost_set<std::string,smart_block>::type  block_cache_type;
	typedef lru_cache_using_boost_set<std::string,smart_pf>::type  file_cache_type;

	boost::shared_ptr<block_cache_type> _block_cache;
	boost::shared_ptr<file_cache_type> _pubfile_cache;
};

class BlockstoreDriver : public an::plgdrv::LocalStorage
{
public: 
	const std::string getName() { return "blockstore_driver"; }
	Blockstore* createPlugin() {return new Blockstore();}
};

#endif // PLG_SFS_H