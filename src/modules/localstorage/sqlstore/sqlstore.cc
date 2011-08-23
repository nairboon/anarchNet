#include "logger.h"
#include "puggKernel.h"
#include "db.h"
#include "sqlstore.h"
#include "config_manager.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/smart_ptr.hpp>


namespace fs = boost::filesystem;

using namespace pugg;
using namespace litesql;
using namespace db;

#define PATH "/sqlstore.db"

extern "C" //__declspec(dllexport)
void registerPlugin(Kernel &K) 
{
	Server<an::plgdrv::LocalStorage>* server = CastToServerType<an::plgdrv::LocalStorage>(K.getServer(PLG_LOCALSTORAGE_SERVER_NAME));
	assert(server != NULL);
	server->addDriver(new SqlstoreDriver(),PLG_LOCALSTORAGE_SERVER_VERSION);
}

time_t to_time_t(const boost::posix_time::ptime& pt)
{
  using namespace boost::posix_time;
  static ptime epoch(boost::gregorian::date(1970, 1, 1));
  time_duration diff(pt - epoch);
  return (diff.ticks() / diff.ticks_per_second());
}

bool Sqlstore::initialise() {
	_db_path = an::ConfigManager::instance().datadir()+PATH;
try {
	// check data
	if(!fs::exists(_db_path)) {
		if(!create_db()) {
				LOG(ERROR) << "could not create sqlite db";
				return false;
		}
		LOG(INFO) << "created new sqlitedb";
		return true;
	}
	_db = new SQLDB("sqlite3","database="+_db_path);
	_db->verbose = true;
	if(_db->needsUpgrade())
	_db->upgrade();
	return true;
}
catch(Except e) {
	LOG(ERROR) << "DBException: " << e;
	return false;
}
	LOG(INFO) << "sqlite store ready";
	return true;
}

bool Sqlstore::db_store_snapshot(an::db::SnapshotPtr sn) {
	try {
		//_db->begin();
		Snapshot ss(*_db);
		ss.based = sn->based;
		ss.anID = sn->id;
		ss.id = 0;
		ss.content = sn->content;
		ss.time = to_time_t(sn->time);
		BOOST_FOREACH (an::db::DiffPtr diff, sn->diffs) {
			boost::shared_ptr<db::Diff> d = _db_store_diff(diff);
			ss.diffs().link(*d);
		}
		ss.update();
		//_db->commit();
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}

boost::shared_ptr<db::ObjID> Sqlstore::_db_store_id(const an::db::ObjID& id)
{
	boost::shared_ptr<db::ObjID> oi(new db::ObjID(*_db));
	oi->anID = id;
	oi->update();
	return oi;
}

boost::shared_ptr<db::Diff> Sqlstore::_db_store_diff(an::db::DiffPtr ip)
{
	boost::shared_ptr<db::Diff> diff(new db::Diff(*_db));
	diff->snapshot = ip->snapshot;
	diff->anID = ip->id;
	diff->content = ip->content;
	diff->time = to_time_t(ip->time);
	BOOST_FOREACH (an::db::ObjID prev, ip->prev) {
		diff->prev().link(*_db_store_id(prev));
	}
	diff->update();
	return diff;
}

bool Sqlstore::db_store_diff(an::db::DiffPtr diff) 
{
	try {
		_db->begin();
		_db_store_diff(diff);
		_db->commit();
		return true;
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}

bool Sqlstore::db_remove(const an::db::ObjID& id)
{
	try {
		_db->begin();
		db::ObjID oi = select<db::ObjID>(*_db,db::ObjID::AnID == id).one();
		oi.del();
		_db->commit();
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}

bool Sqlstore::db_get_snapshot(const an::db::ObjID& id,an::db::SnapshotPtr res)
{
	try {
		db::Snapshot ss = select<Snapshot>(*_db,Snapshot::AnID == id).one();
		res->id = id;
		res->based = ss.based.value();
		res->content = ss.content.value();
		res->time = boost::posix_time::from_time_t(ss.time.value().timeStamp());		
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}

bool Sqlstore::db_get_diff(const an::db::ObjID& id,an::db::DiffPtr res) 
{
	try {
		db::Diff diff = select<Diff>(*_db,Diff::AnID == id).one();
		res->id = id;
		res->snapshot = diff.snapshot.value();
		res->content = diff.content.value();
		res->time = boost::posix_time::from_time_t(diff.time.value().timeStamp());		
		std::vector<ObjID> prevs = diff.prev().get().all();
		BOOST_FOREACH (db::ObjID previd, prevs) {
			res->prev.push_back(previd.anID.value());
		}		
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}
bool Sqlstore::db_get_obj(const an::db::ObjID& id, an::db::ObjPtr res)
{
	try {

	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}
bool Sqlstore::db_update_obj(const an::db::ObjID& id, an::db::ObjPtr obj)
{
	try {
		_db->begin();
		
		_db->commit();
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}
bool Sqlstore::db_store_obj(an::db::ObjPtr obj)
{
	try {
		_db->begin();
		
		_db->commit();
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}

bool Sqlstore::create_db() {
	_db = new SQLDB("sqlite3","database="+_db_path);
	_db->verbose = true;
	_db->create();
	return true;
}

void Sqlstore::shutdown() {
	
}



/*	class online_checker
 {
 public:
 online_checker(boost::asio::io_service& io,int seconds)
 : timer_(io, boost::posix_time::seconds(seconds))
 {
 timer_.async_wait(boost::bind(&online_checker::check, this));
 }
 
 void check()
 {
 std::vector<std::string> tc = DBManager::instance().get_unchecked_keys_since(time(0)-ConfigManager::instance().config()["db-recheck"].as<int>());
 LOG(INFO) << "checking " << tc.size() << " items...";
 
 // restart timer
 timer_.expires_at(timer_.expires_at() + 
 boost::posix_time::seconds(ConfigManager::instance().config()["db-recheck"].as<int>()));
 timer_.async_wait(boost::bind(&online_checker::check, this));
 }
 
 private:
 boost::asio::deadline_timer timer_;
 };
 */

/*
 init
 db_.open((db_path+"/"+ConfigManager::instance().config()["db"].as<std::string>()).c_str());
 
 // cache apps & protocols
 // to avoid frequent queries to this "static" table
 CppSQLite3Table t = db_.getTable("select * from aplist");
 
 for (int row = 0; row < t.numRows(); row++)
 {
 t.setRow(row);
 if (t.fieldIsNull(0) || t.fieldIsNull(1)) {
 LOG(ERROR) << "db corrupt: NULL in aplist";
 return false;
 }
 int id = t.getIntField(0);
 ap_list_[t.getStringField(1)] = id;
 }
 boost::thread thread(boost::bind(&DBManager::run_checker,this));*/


/*bool DBManager::store_object(const StoreObjectRequest* req)
 {
 //test if object is already stored
 CppSQLite3Statement stmt = db_.compileStatement("select key from data where key=?;");
 stmt.bind(1,req->key().c_str());
 CppSQLite3Query q = stmt.execQuery();
 stmt.reset();
 if (q.numFields() == 1) {
 LOG(INFO) << "object exists already for key: " << req->key();
 return true; // exists
 }
 
 stmt = db_.compileStatement("insert into data values (?,?,?,?,?,?,?);");
 stmt.bind(1,req->key().c_str());
 stmt.bind(2,req->value().c_str());
 stmt.bind(3,req->time());
 stmt.bind(4,req->app().c_str());
 stmt.bind(5,req->protocol().c_str());
 stmt.bind(6,req->public_() ? 1 : 0);
 stmt.bind(7,req->time());
 
 stmt.execDML();
 stmt.reset();
 return true;
 }
 
 bool DBManager::delete_object(const DeleteObjectRequest* req)
 {
 CppSQLite3Statement stmt =db_.compileStatement("delete from data where key=?;");
 stmt.bind(1,req->key().c_str());
 stmt.execDML();
 stmt.reset();
 return true;
 }
 void DBManager::run_checker()
 {
 boost::asio::io_service io;
 online_checker p(io,ConfigManager::instance().config()["db-recheck"].as<int>());
 io.run();
 }
 
 std::vector<std::string> DBManager::get_unchecked_keys_since(int lc)
 {
 //test if object is already stored
 CppSQLite3Statement stmt = db_.compileStatement("select key from data where lastcheck<?;");
 stmt.bind(1,lc);
 CppSQLite3Query q = stmt.execQuery();
 stmt.reset();
 std::vector<std::string> res;
 while(!q.eof()) {
 res.push_back(q.fieldValue(0));
 }
 return res;
 }*/