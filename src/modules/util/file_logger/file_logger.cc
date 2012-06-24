#include <sstream>
#include "config_manager.h"
#include "db.h"
#include "file_logger.h"
#include "puggKernel.h"
#include "db.h"
#include "crypto.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/log/common.hpp>


namespace fs = boost::filesystem;

using namespace pugg;

extern "C" //__declspec(dllexport)
void registerPlugin(Kernel &K) 
{
	Server<an::plgdrv::Generic>* server = CastToServerType<an::plgdrv::Generic>(K.getServer(PLG_UTIL_SERVER_NAME));
	assert(server != NULL);
	server->addDriver(new LoggerDriver(),PLG_UTIL_SERVER_VERSION);
}

bool Logger::initialise() {
	_path = an::ConfigManager::instance().logdir();
	
	return true;
}
void Logger::shutdown() {}

bool Logger::log(an::log::severity_level level, const std::string message)
{
  if(level >= an::ConfigManager::instance().config().get("modules.log.stdout",0))
    std::cout << level,message;
  
  return true;
}

std::string Logger::_log(an::log::severity_level lvl, const std::string& message)
{
  std::stringstream strm;
  static const char* const str[] =
		{
			"debug",
			"info",
			"warning",
			"error",
			"fatal"
		};
		if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
			strm << str[lvl];
		else
			strm << static_cast< int >(lvl);
	
	strm << ": " << message << std::endl;
    return strm.str();
}
