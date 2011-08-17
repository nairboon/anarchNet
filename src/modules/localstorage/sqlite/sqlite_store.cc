#include "puggKernel.h"
#include "sqlite_store.h"

using namespace pugg;

extern "C" //__declspec(dllexport)
void registerPlugin(Kernel &K) 
{
	Server<an::plgdrv::LocalStorage>* server =CastToServerType<an::plgdrv::LocalStorage>(K.getServer(PLG_LOCALSTORAGE_SERVER_NAME));
	assert(server != NULL);
	server->addDriver(new SqliteDriver(),PLG_LOCALSTORAGE_SERVER_VERSION);
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