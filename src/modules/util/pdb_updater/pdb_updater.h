#ifndef PLG_PDB_H
#define PLG_PDB_H

#include "plugins/util.h"

#define PLG_PDB_VERSION 1

class PDB : public an::plg::Util
{
	bool _run;
	boost::mutex _mutex;
	boost::shared_ptr<boost::thread> _checker;
public:
	const std::string getName() {return "pdb updater";}
	const unsigned int getVersion() { return PLG_PDB_VERSION; }
	bool initialise();
	void shutdown();
	
	bool on_kv_put(const an::db::ObjID &key);
	bool on_kv_remove(const an::db::ObjID &key);
	bool on_db_update(const an::db::ObjID &key);
	
	void check();
};

class PDBDriver : public an::plgdrv::Util
{
public: 
	const std::string getName() { return "pdb updater"; }
	PDB* createPlugin() {return new PDB();}
};

#endif // PLG_PDB_H