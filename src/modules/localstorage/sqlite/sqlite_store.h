#ifndef PLG_SQLITE_H
#define PLG_SQLITE_H

#include "plugins/localstorage.h"

#define PLG_SQLITE_VERSION 1

class Sqlite : public an::plg::LocalStorage
{
public:
	const std::string getName() {return "sqlite";}
	const unsigned int getVersion() { return PLG_SQLITE_VERSION; }
	bool initialise();
	void shutdown();
};

class SqliteDriver : public an::plgdrv::LocalStorage
{
public: 
	std::string getName() { return "sqlite_driver"; }
	Sqlite* createPlugin() {return new Sqlite();}
};

#endif // PLG_SQLITE_H