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
#include <string>
#include <glog/logging.h>
#include "anarchNet.h"
#include "plugin_manager.h"

using std::string;

namespace an
{
	
	bool PluginManager::init()
	{
		dhtServer_ = new pugg::Server<DHTpluginDriver>(PLG_DHT_SERVER_NAME,PLG_DHT_SERVER_VERSION);
		plugin_kernel_.addServer(dhtServer_);
		return true;
	}
	
	bool PluginManager::loadPlugin(const std::string& plugin_name, const std::string& plugin_path,const std::string& plugin_working_directory)
	{
		if(!plugin_kernel_.loadPlugin(plugin_path)) 
			return false; 
		
		std::vector<DHTpluginDriver*> drv;
		dhtServer_->getAllDrivers(drv);
		std::cerr << drv.size() << " drivers register";
		foreach(DHTpluginDriver* p,drv)
		LOG(INFO) << p->createPlugin()->getName();
		//	if(driver == NULL)
		//return false;
		LOG(FATAL) << "plg";
		//	dhtPlugins_.push_back(driver->createPlugin());
		return true;
	}
}
