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

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <glog/logging.h>

#include "protobuf_rpc_socket.h"

#include "anarchNet.h"
#include "daemon.h"
#include "rpc_service.h"
#include "config_manager.h"
#include "module_manager.h"
#include "db_manager.h"
#include "plugin_manager.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;
using std::string;

namespace an {
	

	
bool anDaemon::init(const string& directory)
{
	
	LOG(INFO) << "load config_manager";
	if(!ConfigManager::instance().init(directory))
		return false;

//	if(!DBManager::instance().init(directory))
//		return false;
	
	LOG(INFO) << "load plugin_manager";
	if(!PluginManager::instance().init())
		return false;
	
	LOG(INFO) << "load module_manager";
	if(!ModuleManager::instance().init())
		return false;
	
	awk::protobuf::jerpc::socket_initialize();
	return true;
}
	
void anDaemon::run() 
{
	rpc_service_ = new anRPCService();
	server_.RegisterService(rpc_service_);
	LOG(INFO) << "main run";
	server_.RunServer();
}

}
