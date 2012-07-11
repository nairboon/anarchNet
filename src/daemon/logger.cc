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

#include <cassert>
#include <iostream>
#include <fstream>

#include <boost/log/common.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/filters.hpp>


#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/empty_deleter.hpp>

#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/core/core.hpp>
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
	namespace log {
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

		void Logger::log(const std::string& msg, severity_level lvl)
		{
			BOOST_LOG_SEV(_logger,lvl) << msg;
		}
		bool Logger::init(const std::string& logfile,severity_level file_lvl, severity_level console_lvl) 
{
		  typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
		  shared_ptr< text_sink > fileSink(new text_sink);
		  {
		    
		    text_sink::locked_backend_ptr pBackend = fileSink->locked_backend();
		    
		    shared_ptr< std::ofstream > pStream2(new std::ofstream(logfile));
		    assert(pStream2->is_open());
		    pBackend->add_stream(pStream2);
		  }
		  shared_ptr< text_sink > cSink(new text_sink);
		  {
		    
		    text_sink::locked_backend_ptr pBackend = cSink->locked_backend();
		    
		    
		    shared_ptr< std::ostream > pStream(&std::cerr, logging::empty_deleter());
		    pBackend->add_stream(pStream);
		  }
		  logging::core::get()->add_sink(cSink);
		  cSink->set_filter(
		    flt::attr< severity_level >("Severity", std::nothrow) >= console_lvl);
		  logging::core::get()->add_sink(fileSink);
		  fileSink->set_filter(
		    flt::attr< severity_level >("Severity", std::nothrow) >= file_lvl);
		 
		   		    logging::core::get()->add_global_attribute("RecordID", RecordID);
		    logging::core::get()->add_global_attribute("TimeStamp", TimeStamp);

		  		 boost::log::formatters::fmt_format<char> fmtr = fmt::format("%1% %2% [%3%] %4%")
		    % fmt::attr("RecordID")
		    % fmt::date_time< boost::posix_time::ptime >("TimeStamp", "%d.%m.%Y %H:%M:%S.%f")
		    % fmt::attr< severity_level >("Severity", std::nothrow)
		   // % fmt::attr< std::string >("Tag")
		   // % fmt::named_scope("Scope", keywords::iteration = fmt::reverse, keywords::depth = 2)
		    % fmt::message();
		    cSink->set_formatter(fmtr);
		     fileSink->set_formatter(fmtr);
		     

		     BOOST_LOG_SEV(_logger,INFO) << "Logger ready";

		  return true;
		}
	}
}
