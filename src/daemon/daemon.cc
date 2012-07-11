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

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/json/json.hpp>
#include "logger.h"
#include "anarchNet.h"
#include "daemon.h"
#include "rpc_server.h"
#include "rpc_manager.h"
#include "config_manager.h"
#include "module_manager.h"
#include "db_manager.h"
#include "plugin_manager.h"
#include "net_manager.h"
#include "storage.h"


namespace an {
bool anDaemon::init(const std::string& directory)
{
	
	if(!ConfigManager::instance().init(directory))
		return false;
	
	if(!log::Logger::instance().init("ad.log",log::severity_level::DEBUG,log::severity_level::DEBUG))
		return false;
	
	LOG(INFO) << "load plugin_manager";
	if(!PluginManager::instance().init())
		return false;
	
	LOG(INFO) << "load module_manager";
	if(!ModuleManager::instance().init())
		return false;

	LOG(INFO) << "load storage_manager";
	if(!StorageManager::instance().init(""))
		return false;
	
	LOG(INFO) << "load net_manager";
	if(!NetManager::instance().init())
		return false;
	
	LOG(INFO) << "load rpc_manager";
	if(!RPCManager::instance().init())
		return false;
	
	return true;
}
	
void anDaemon::run() 
{
	//boost::thread network(boost::bind(&NetManager::run,boost::ref(NetManager::instance())));
	boost::thread rpc(boost::bind(&RPCManager::run,boost::ref(RPCManager::instance())));

	LOG(INFO)<<"waiting..";
	/*network.join();
	LOG(INFO) << "network joind";*/
	rpc.join();
	LOG(INFO) << "rpc joind";
}

	void anDaemon::stop() {
		NetManager::instance().stop();
		RPCManager::instance().stop();
		DBManager::instance().kill();
		ModuleManager::instance().kill();
	}
	
	anDaemon::~anDaemon() {
	}
}
