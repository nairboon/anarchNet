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
#include <sys/stat.h>
#include <iostream>
#include <cstdio>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include "logger.h"
#include "anarchnet.h"
#include "version.h"
#include "daemon.h"
#include <boost/log/trivial.hpp>

namespace po = boost::program_options;
using std::string;

an::anDaemon* g_daemon;

/**
 * \brief Signal management.
 * \param code signal code
 */
static void signal_handler(int code)
{
  switch(code)
  {
    case SIGINT:
		case SIGHUP:
		case SIGQUIT:
		case SIGKILL:
    case SIGTERM:
			LOG(ERROR)<<"shuting down";
      g_daemon->stop();
      break;
    default:
      break;
  }
}


int main(int argc, char* argv[])
{

	//an::Logger::instance().init("daemon.log");

	g_daemon = NULL;
	try {

		po::options_description generic("Generic options");
		generic.add_options()
		("version,v", "print version string")
		("help,h", "produce help message")
		("daemonize,d","daemonize anDaemon")
		("directory", po::value<string>()->default_value("."), "prefix directory file");


		po::positional_options_description p;
		p.add("directory", -1);

		po::variables_map vm;
		store(po::command_line_parser(argc, argv).
					options(generic).positional(p).run(), vm);
		
		po::notify(vm);

		if (vm.count("help")) {
			std::cerr << "Usage: anDaemon [Options] directory" << std::endl;
			std::cerr << generic << std::endl;
			return 1;
		}
		if (vm.count("version")) {
			printf("anarchNet v%d.%d.%d\n", ANARCHNET_VERSION_MAJOR,
						 ANARCHNET_VERSION_MINOR, ANARCHNET_VERSION_PATCH);
			return 0;
		}

		g_daemon = new an::anDaemon();
		LOG(INFO) << "initializing daemon";
		
		std::string directory;
		std::string directory_path = vm["directory"].as<string>();
		if (directory_path[0] == '~') 
			directory = string(getenv("HOME")) + directory_path.substr(1);
		else
			directory = directory_path;
		
		if (!g_daemon->init(directory)) {
			LOG(FATAL) << "init failed";
			return EXIT_FAILURE;
		}
		//daemonize
		if (vm.count("daemonize")) {
			LOG(INFO) << "forking...";

		 pid_t pid = fork();
		 if ( pid < 0)
			 exit(EXIT_FAILURE);

		 if (pid > 0)
			exit(EXIT_SUCCESS);

		 umask(0);
		 pid_t sid = setsid();
		 if ( sid < 0)
			 exit(EXIT_FAILURE);
			
			LOG(INFO) << "success";
		}

		if(signal(SIGTERM, signal_handler) == SIG_ERR)
		{
			std::cout << "Error signal SIGTERM will not be handled" << std::endl;
		}
		
		if(signal(SIGINT, signal_handler) == SIG_ERR)
		{
			std::cout << "Error signal SIGINT will not be handled" << std::endl;
		}
		
		g_daemon->run();
		LOG(INFO) << "done";		
	}

	catch(const std::exception& e) {
		LOG(ERROR) << "Exception: " << e.what();
		return EXIT_FAILURE;
	}
	catch(...) {
		LOG(ERROR) << "Exception of unknown type!";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
