#include "logger.h"
#include "puggKernel.h"
#include "db.h"
#include "sqlite_store.h"
#include "config_manager.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>



namespace fs = boost::filesystem;

using namespace pugg;

#define PATH "/sqlite_store.db"
extern "C" //__declspec(dllexport)
void registerPlugin(Kernel &K) 
{
	Server<an::plgdrv::LocalStorage>* server = CastToServerType<an::plgdrv::LocalStorage>(K.getServer(PLG_LOCALSTORAGE_SERVER_NAME));
	assert(server != NULL);
	server->addDriver(new SqliteDriver(),PLG_LOCALSTORAGE_SERVER_VERSION);
}

time_t to_time_t(const boost::posix_time::ptime& pt)
{
  using namespace boost::posix_time;
  static ptime epoch(boost::gregorian::date(1970, 1, 1));
  time_duration diff(pt - epoch);
  return (diff.ticks() / diff.ticks_per_second());
}

bool Sqlite::initialise() {
	_db_path = an::ConfigManager::instance().datadir()+PATH;
try {
	// check data
	if(!fs::exists(_db_path)) {
		if(!create_db()) {
				LOG(ERROR) << "could not create sqlite db";
				return false;
		}
		LOG(INFO) << "created new sqlitedb";
	}
	_db.open(_db_path.c_str());
}
catch(CppSQLite3Exception& e) {
	LOG(ERROR) << "DBException: " << e.errorCode()<< ": " << e.errorMessage();
	return false;
}
	LOG(INFO) << "sqlite store ready";
	return true;
}

bool Sqlite::db_store_snapshot(const an::db::Snapshot& sn) {
	BOOST_FOREACH (boost::shared_ptr<an::db::Diff> diff, sn.diffs) {
		if(!db_store_diff(*diff))
			return false;
	}
	CppSQLite3Statement stmt = _db.compileStatement("insert into snapshots values (NULL,?,?,?,?);");
	stmt.bind(1,sn.id.c_str());
	stmt.bind(2,sn.based.c_str());
	stmt.bind(3,sn.content.c_str());
	stmt.bind(4,(int)to_time_t(sn.time));	
	stmt.execDML();
	stmt.reset();
	
	return true;
}
bool Sqlite::db_store_diff(const an::db::Diff& diff) {
}
bool Sqlite::db_remove(const an::db::ObjID& id)
{
}



bool Sqlite::create_db() {
	_db.open(_db_path.c_str());
	CppSQLite3Query q = _db.execQuery("CREATE TABLE diff_history ( \
																		diff INTEGER , \
																		previous INTEGER );");
	q = _db.execQuery("CREATE TABLE diffs ( \
																		pk INTEGER PRIMARY KEY, \
																		id TEXT , \
																		snapshot TEXT );");
	q = _db.execQuery("CREATE TABLE object_diffs ( \
																		obj_id INTEGER , \
																		diff_id INTEGER );");
	q = _db.execQuery("CREATE TABLE object_snapshots ( \
																		obj_id INTEGER , \
																		snapshot_id INTEGER );");
	q = _db.execQuery("CREATE TABLE objects ( \
																		pk INTEGER PRIMARY KEY, \
																		id TEXT );");
	q = _db.execQuery("CREATE TABLE snapshots ( \
																		pk INTEGER PRIMARY KEY, \
																		id TEXT , \
																		based TEXT , \
																		content TEXT , \
																		time INTEGER ); \
																		");
		_db.close();
	return true;
}

void Sqlite::shutdown() {
	
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