#ifndef PLG_SQLSTORE_H
#define PLG_SQLSTORE_H

#include <boost/shared_ptr.hpp>
#include "plugins/localstorage.h"
#include "sqldb.h"

#define PLG_SQLSTORE_VERSION 1

class Sqlstore : public an::plg::LocalStorage
{
public:
	const std::string getName() {return "sqlstore";}
	const unsigned int getVersion() { return PLG_SQLSTORE_VERSION; }
	bool initialise();
	void shutdown();
	
	bool db_store_obj(an::db::ObjPtr obj);
	bool db_store_snapshot(an::db::SnapshotPtr sn);
	bool db_store_diff(an::db::DiffPtr diff);
	bool db_remove(const an::db::ObjID& id);
	bool db_get_snapshot(const an::db::ObjID& id,an::db::SnapshotPtr res);
	bool db_get_diff(const an::db::ObjID& id,an::db::DiffPtr res);
	bool db_get_obj(const an::db::ObjID& id,an::db::ObjPtr res);
	
private:
	db::SQLDB *_db;
	std::string _db_path;
	bool create_db();
	boost::shared_ptr<db::Diff> _db_store_diff(an::db::DiffPtr diff);
	boost::shared_ptr<db::Snapshot> _db_store_snapshot(an::db::SnapshotPtr ip);
	bool _db_get_diff(const an::db::ObjID& id, db::Diff& diff, an::db::DiffPtr res);
	bool _db_get_snapshot(const an::db::ObjID& id, db::Snapshot& ss, an::db::SnapshotPtr res);
	boost::shared_ptr<db::ObjID> _db_store_id(const an::db::ObjID& id);
};

class SqlstoreDriver : public an::plgdrv::LocalStorage
{
public: 
	const std::string getName() { return "sqlstore_driver"; }
	Sqlstore* createPlugin() {return new Sqlstore();}
};

#endif // PLG_SQLSTORE_H