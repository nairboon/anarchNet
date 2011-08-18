/*
 * Copyright (C) 2010 Remo Hertig (nairboon)
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

#include <boost/log/common.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/filters.hpp>

#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/common_attributes.hpp>

#include <boost/log/attributes/timer.hpp>
#include "logger.h"


namespace logging = boost::log;
namespace fmt = boost::log::formatters;
namespace flt = boost::log::filters;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

using boost::shared_ptr;


namespace an
{
	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT >& operator<< (
																													 std::basic_ostream< CharT, TraitsT >& strm, severity_level lvl)
	{
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
		return strm;
	}
	
/*	bool Logger::init(const std::string& logfile) {
		//logging::init_log_to_console(std::clog, keywords::format = "%TimeStamp%: %_%");
		
		// One can also use lambda expressions to setup filters and formatters
		logging::init_log_to_file
		(
		 logfile,
		 keywords::filter = flt::attr< severity_level >("Severity", std::nothrow) >= INFO,
		 keywords::format = fmt::format("%1% [%2%] <%3%> %4%")
		 % fmt::date_time("TimeStamp", std::nothrow)
		 % fmt::time_duration("Uptime", std::nothrow)
		 % fmt::attr< severity_level >("Severity", std::nothrow)
		 % fmt::message()
		 );
		
		// Also let's add some commonly used attributes, like timestamp and record counter.
		logging::add_common_attributes();
		_logger.add_attribute("Uptime", attrs::timer());
		return true;
	}*/

}