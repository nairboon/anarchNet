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

#include <string>
#include <boost/foreach.hpp>
#include "anarchNet.h"
#include "logger.h"
#include "module_manager.h"
#include "config_manager.h"
#include "plugins/bootstrap.h"
#include "plugins/localstorage.h"
#include "plugins/remotestorage.h"
#include "plugins/session.h"
#include "plugins/util.h"
#include "plugin_manager.h"


namespace an
{

bool ModuleManager::init()
{
	#ifdef ANARCHNET_PLATFORM_MACOS
	  std::string plugin_suffix = ".dylib";
	#endif
	#ifdef ANARCHNET_PLATFORM_LINUX
	  std::string plugin_suffix = ".so";
	#endif

	BOOST_FOREACH(std::string plugin_name, ConfigManager::instance().modules()) {
		if(!PluginManager::instance().loadPlugin(plugin_name,"./"+plugin_name+plugin_suffix,plugin_name))
			return false;
	}


	std::vector<plgdrv::LocalStorage*> ls_drivers;
	dynamic_cast<pugg::Server<an::plgdrv::LocalStorage>*>(PluginManager::instance().get_kernel().getServer(PLG_LOCALSTORAGE_SERVER_NAME))->getAllDrivers(ls_drivers);
	BOOST_FOREACH(plgdrv::LocalStorage* drv, ls_drivers) {
		plg::LocalStorage* plg = drv->createPlugin();
		LOG(INFO)<< plg->getName() << " created";
		if(!plg->initialise())
		  return false;
		LOG(INFO)<< plg->getName() << " initialized";
		_localstorages.push_back(plg);
	}

	std::vector<plgdrv::Generic*> u_drivers;
	dynamic_cast<pugg::Server<an::plgdrv::Generic>*>(PluginManager::instance().get_kernel().getServer(PLG_UTIL_SERVER_NAME))->getAllDrivers(u_drivers);
	BOOST_FOREACH(plgdrv::Generic* drv, u_drivers) {
		plg::Generic* plg = drv->createPlugin();
		LOG(INFO)<< plg->getName() << " created";
		if(!plg->initialise())
		  return false;
		LOG(INFO)<< plg->getName() << " initialized";
		_utils.push_back(plg);
	}
	return true;
}


	bool ModuleManager::kv_get_unique(const std::string& key,std::string& res)
	{
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->kv_get_unique(key,res))
				return true;

		return false;
	}



		bool ModuleManager::kv_get_stats(const std::string& key, KV_Stat& res) {
		    return false;
		}
		bool ModuleManager::kv_get_unsuccessful(int n, KV_StatsPtr& res) {
		    return false;

		}
		bool ModuleManager::kv_get_unchecked_since(boost::posix_time::time_duration t , KV_StatsPtr& res)
		{
		    return false;
		}


	bool ModuleManager::db_store_snapshot(db::SnapshotPtr ss) {
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->getType() == plg::LocalStorage::PLAIN && (*it)->db_store_snapshot(ss))
				return true;

		return false;
	}

	bool ModuleManager::db_store_diff(db::DiffPtr diff) {
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->getType() == plg::LocalStorage::PLAIN && (*it)->db_store_diff(diff))
				return true;

		return false;
	}

	bool ModuleManager::db_store_obj(db::ObjPtr obj) {
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->getType() == plg::LocalStorage::PLAIN && (*it)->db_store_obj(obj))
				return true;

		return false;
	}

	bool ModuleManager::db_get_snapshot(const db::ObjID& id,db::SnapshotPtr res) {
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->getType() == plg::LocalStorage::PLAIN && (*it)->db_get_snapshot(id,res))
				return true;

		return false;
	}

	bool ModuleManager::db_get_diff(const db::ObjID& id,db::DiffPtr res) {
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->getType() == plg::LocalStorage::PLAIN && (*it)->db_get_diff(id,res))
				return true;

		return false;
	}

	bool ModuleManager::db_get_obj(const db::ObjID& id,db::ObjPtr res) {
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->getType() == plg::LocalStorage::PLAIN && (*it)->db_get_obj(id,res))
				return true;

		return false;
	}

	bool ModuleManager::db_remove(const db::ObjID& id) {
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->getType() == plg::LocalStorage::PLAIN && (*it)->db_remove(id))
				return true;

		return false;
	}


	
	plg::LocalStorage * ModuleManager::get_ls_plugin(std::string name)
	{

	  BOOST_FOREACH(plg::LocalStorage *x,_localstorages)
	  if( x->getName() == name) {
	    return x;
	  }

	  return NULL;
	}
	
}
