#include <cstdio>
#include "logger.h"
#include "puggKernel.h"
#include "db.h"
#include <boost/thread.hpp>
#include "pdb_updater.h"
#include "crypto.h"
#include "config_manager.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/log/common.hpp>



namespace fs = boost::filesystem;

using namespace pugg;
using namespace an;

#define PATH "/sqlite_store.db"
extern "C" //__declspec(dllexport)
void registerPlugin(Kernel &K) 
{
	Server<an::plgdrv::Util>* server = CastToServerType<an::plgdrv::Util>(K.getServer(PLG_UTIL_SERVER_NAME));
	assert(server != NULL);
	server->addDriver(new PDBDriver(),PLG_UTIL_SERVER_VERSION);
}

  bool PDB::initialise() {
	//_path = an::ConfigManager::instance().datadir() + "/filestore";
	_run = true;
	// create checker thread
	// run
	_checker = boost::shared_ptr<boost::thread>(new boost::thread(&PDB::check,this));
	//checker.detach();
	//sleep(2);
	return true;
}

void PDB::check() {
  do {
    


  boost::this_thread::sleep(boost::posix_time::seconds(1));
  } while(_run);

}
void PDB::shutdown() {
  _mutex.lock();
    _run=false;
  _mutex.unlock();
  _checker->join();
}


bool PDB::on_kv_put(const db::ObjID &key) { return false;}
bool PDB::on_kv_remove(const db::ObjID &key) { return false;}
bool PDB::on_db_update(const db::ObjID &key) { return false;}