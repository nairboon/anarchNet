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
#include "plugins/bootstrap.h"
#include "plugins/localstorage.h"
#include "plugins/remotestorage.h"
#include "plugins/session.h"


namespace an
{
	
	bool PluginManager::init()
	{
		pugg::Server<an::plgdrv::Bootstrap>* bootstrap_server = new pugg::Server<plgdrv::Bootstrap>(PLG_BOOTSTRAP_SERVER_NAME,PLG_BOOTSTRAP_SERVER_VERSION);
		pugg::Server<an::plgdrv::LocalStorage>* localstorage_server = new pugg::Server<plgdrv::LocalStorage>(PLG_LOCALSTORAGE_SERVER_NAME,PLG_LOCALSTORAGE_SERVER_VERSION);
		pugg::Server<an::plgdrv::RemoteStorage>* remotestorage_server = new pugg::Server<plgdrv::RemoteStorage>(PLG_REMOTESTORAGE_SERVER_NAME,PLG_REMOTESTORAGE_SERVER_VERSION);
		pugg::Server<an::plgdrv::Session>* session_server = new pugg::Server<plgdrv::Session>(PLG_SESSION_SERVER_NAME,PLG_SESSION_SERVER_VERSION);

		plugin_kernel_.addServer(bootstrap_server);
		plugin_kernel_.addServer(localstorage_server);
		plugin_kernel_.addServer(remotestorage_server);
		plugin_kernel_.addServer(session_server);

		return true;
	}
	
	bool PluginManager::loadPlugin(const std::string& plugin_name, const std::string& plugin_path,const std::string& plugin_working_directory)
	{
		LOG(INFO) << "load plugin " << plugin_name;
		if(!plugin_kernel_.loadPlugin(plugin_path)) {
			LOG(ERROR) << "could not load plugin: " << plugin_path;
			return false; 
		}
		
	/*	std::vector<DHTpluginDriver*> drv;
		dhtServer_->getAllDrivers(drv);
		std::cerr << drv.size() << " drivers register";
		foreach(DHTpluginDriver* p,drv)
		LOG(INFO) << p->createPlugin()->getName();*/
		//	if(driver == NULL)
		//return false;
		//LOG(FATAL) << "plg";
		//	dhtPlugins_.push_back(driver->createPlugin());
		return true;
	}
}
