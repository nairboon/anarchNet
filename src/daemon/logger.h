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
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/counter.hpp>
#include <boost/log/formatters.hpp>


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

		bool init(const std::string& logfile,severity_level file_lvl = severity_level::INFO, severity_level console_lvl= severity_level::WARNING);
		boost::log::sources::severity_logger_mt< severity_level >& get() { return _logger; }
		void log(const std::string& msg, severity_level lvl);
	protected:
	    Logger() : RecordID(0) {}
	 private:
		boost::log::sources::severity_logger_mt< severity_level > _logger;
		 boost::log::attributes::local_clock TimeStamp;
		 boost::log::attributes::counter< unsigned int > RecordID;
	};
}
/*#define LOG(lvl)\
    BOOST_LOG_STREAM_WITH_PARAMS(::boost::log::trivial::logger::get(),\
        (::boost::log::keywords::severity = ::boost::log::trivial::lvl))
*/
//#define LOG(lvl) std::cerr << std::endl; std::cerr
#define UNIMPLEMENTED << "unimplemented function: " << __func__ << ": " << :__file__
#define LOG(lvl) BOOST_LOG_STREAM_WITH_PARAMS((an::log::Logger::instance().get()), (::boost::log::keywords::severity = an::log::lvl)) 
}
#endif  // SRC_DAEMON_LOGGER_H_
