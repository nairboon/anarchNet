#ifndef PLG_SQLITE_H
#define PLG_SQLITE_H

#include "plugins/localstorage.h"
#include "cpp_sqlite3.h"

#define PLG_SQLITE_VERSION 1

class Sqlite : public an::plg::LocalStorage
{
public:
	const std::string getName() {return "sqlite";}
	const unsigned int getVersion() { return PLG_SQLITE_VERSION; }
	bool initialise();
	void shutdown();
private:
	CppSQLite3DB _db;
	std::string _db_path;
	bool create_db();
};

class SqliteDriver : public an::plgdrv::LocalStorage
{
public: 
	const std::string getName() { return "sqlite_driver"; }
	Sqlite* createPlugin() {return new Sqlite();}
};

#endif // PLG_SQLITE_H