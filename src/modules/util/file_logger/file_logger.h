#ifndef PLG_SFS_H
#define PLG_SFS_H

#include "logger.h"
#include "plugins/util.h"
#include <ostream>

#define PLG_LOGGER_VERSION 1

		
class Logger : public an::plg::Generic
{
public:
	const std::string getName() {return "simple_file_store";}
	const unsigned int getVersion() { return PLG_LOGGER_VERSION; }
	bool initialise();
	void shutdown();
	

	bool log(an::log::severity_level level, const std::string message);

private:
      std::string _log(an::log::severity_level level, const std::string& message);
	std::string _path;
};

class LoggerDriver : public an::plgdrv::Generic
{
public: 
	const std::string getName() { return "simple_file_store_driver"; }
	Logger* createPlugin() {return new Logger();}
};

#endif // PLG_LOGGER_H