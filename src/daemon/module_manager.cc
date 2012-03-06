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

	bool failed = false;
	BOOST_FOREACH(std::string plugin_name, ConfigManager::instance().modules()) {
		if(!PluginManager::instance().loadPlugin(plugin_name,"./"+plugin_name+plugin_suffix,plugin_name))
			failed = true;
	}

	if(failed)
	  return false;

	std::vector<plgdrv::Bootstrap*> bs_drivers;
	dynamic_cast<pugg::Server<an::plgdrv::Bootstrap>*>(PluginManager::instance().get_kernel().getServer(PLG_BOOTSTRAP_SERVER_NAME))->getAllDrivers(bs_drivers);
	BOOST_FOREACH(plgdrv::Bootstrap* drv, bs_drivers) {
		plg::Bootstrap* plg = drv->createPlugin();
		LOG(INFO)<< plg->getName() << " created";
		if(!plg->initialise())
		  return false;
		LOG(INFO)<< plg->getName() << " initialized";
		_bootstrapers.push_back(plg);
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

	std::vector<plgdrv::RemoteStorage*> rs_drivers;
	dynamic_cast<pugg::Server<an::plgdrv::RemoteStorage>*>(PluginManager::instance().get_kernel().getServer(PLG_REMOTESTORAGE_SERVER_NAME))->getAllDrivers(rs_drivers);
	BOOST_FOREACH(plgdrv::RemoteStorage* drv, rs_drivers) {
		plg::RemoteStorage* plg = drv->createPlugin();
		LOG(INFO)<< plg->getName() << " created";
		if(!plg->initialise())
		  return false;
		LOG(INFO)<< plg->getName() << " initialized";
		_remotestorages.push_back(plg);
	}

	std::vector<plgdrv::Session*> s_drivers;
	dynamic_cast<pugg::Server<an::plgdrv::Session>*>(PluginManager::instance().get_kernel().getServer(PLG_SESSION_SERVER_NAME))->getAllDrivers(s_drivers);
	BOOST_FOREACH(plgdrv::Session* drv, s_drivers) {
		plg::Session* plg = drv->createPlugin();
		LOG(INFO)<< plg->getName() << " created";
		if(!plg->initialise())
		  return false;
		LOG(INFO)<< plg->getName() << " initialized";
		_sessions.push_back(plg);
	}

	std::vector<plgdrv::Util*> u_drivers;
	dynamic_cast<pugg::Server<an::plgdrv::Util>*>(PluginManager::instance().get_kernel().getServer(PLG_UTIL_SERVER_NAME))->getAllDrivers(u_drivers);
	BOOST_FOREACH(plgdrv::Util* drv, u_drivers) {
		plg::Util* plg = drv->createPlugin();
		LOG(INFO)<< plg->getName() << " created";
		if(!plg->initialise())
		  return false;
		LOG(INFO)<< plg->getName() << " initialized";
		_utils.push_back(plg);
	}
	return true;
}


	bool ModuleManager::bootstrapFromPeer(const std::string&ip,int port)
	{
		for(std::vector<plg::Bootstrap*>::iterator it = _bootstrapers.begin(); it != _bootstrapers.end(); it++)
			if( (*it)->bootstrapFromPeer(ip,port))
				return true;

		return false;
	}

	bool ModuleManager::bootstrapFromHostlist(const std::string&url)
	{
		for(std::vector<plg::Bootstrap*>::iterator it = _bootstrapers.begin(); it != _bootstrapers.end(); it++)
			if( (*it)->bootstrapFromHostlist(url))
				return true;

		return false;
	}


	bool ModuleManager::kv_put(const std::string& key, const std::string& value)
	{
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->kv_put(key,value))
				return true;

		return false;
	}

	bool ModuleManager::kv_get(const std::string& key,KV_ResPtr& res)
	{
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if(  (*it)->kv_get(key,res))
				return true;

		return false;
	}

	bool ModuleManager::kv_get_unique(const std::string& key,std::string& res)
	{
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->kv_get_unique(key,res))
				return true;

		return false;
	}
	bool ModuleManager::kv_remove(const std::string& key)
	{
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if(  (*it)->kv_remove(key))
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

	bool ModuleManager::store_file(const std::string& path, std::string& res) {
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->getType() == plg::LocalStorage::BINARY && (*it)->store_file(path,res))
				return true;

		return false;
	}

	bool ModuleManager::get_file(const std::string& id,std::string& res) {
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->getType() == plg::LocalStorage::BINARY && (*it)->get_file(id,res))
				return true;

		return false;
	}

	bool ModuleManager::remove_file(const std::string& id) {
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->getType() == plg::LocalStorage::BINARY && (*it)->remove_file(id))
				return true;

		return false;
	}


	/* UTIL */
	bool ModuleManager::log(log::severity_level level, const std::string message)
	{
		bool worked = false;
		for(std::vector<plg::Util*>::iterator it = _utils.begin(); it != _utils.end(); it++)
		  if((*it)->log(level,message))
		     worked = true;
	      if(!worked)
		std::cerr << message << std::endl;
	}


	bool ModuleManager::on_db_update(const db::ObjID &key)
	{
		for(std::vector<plg::Util*>::iterator it = _utils.begin(); it != _utils.end(); it++)
			if( (*it)->on_db_update(key))
				return true;

		return false;
	}

	bool ModuleManager::on_kv_put(const db::ObjID &key)
	{
		for(std::vector<plg::Util*>::iterator it = _utils.begin(); it != _utils.end(); it++)
			if( (*it)->on_kv_put(key))
				return true;

		return false;
	}

	bool ModuleManager::on_kv_remove(const db::ObjID &key)
	{
		for(std::vector<plg::Util*>::iterator it = _utils.begin(); it != _utils.end(); it++)
			if( (*it)->on_kv_remove(key))
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
