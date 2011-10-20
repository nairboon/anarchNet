/*
 * Copyright (C) 2010, 2011 Remo Hertig (nairboon)
 * https://launchpad.net/anarchNet
 *
 * This file is part of anarchNet.
 *
 * anarchNet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * anarchNet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with anarchNet.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "singleton.h"
#include <iostream>
//#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>


#ifndef SRC_DAEMON_LOGGER_H_
#define SRC_DAEMON_LOGGER_H_


namespace an
{
	namespace log {
	enum severity_level
	{
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		FATAL
	};

	
	class Logger : public Singleton<Logger> {
		friend class Singleton<Logger>;
	 public:

		bool init(const std::string& logfile);
		boost::log::sources::severity_logger_mt< severity_level >& get() { return _logger; }
		void log(const std::string& msg, severity_level lvl);
	 private:
		boost::log::sources::severity_logger_mt< severity_level > _logger;
	};
}
//#define LOG(lvl)
//BOOST_LOG_STREAM_WITH_PARAMS(an::Logger::instance().get(),
//(::boost::log::keywords::severity = ::an::lvl))
#define INFO an::log::INFO
#define ERROR an::log::ERROR
#define LOG(lvl) std::cerr << std::endl; std::cerr
//#define LOG(lvl) an::ModuleManager::instance().log(
}
#endif  // SRC_DAEMON_LOGGER_H_
