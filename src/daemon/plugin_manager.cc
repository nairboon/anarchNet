/*
* Copyright (C) 2010, 2011, 2012 Remo Hertig (nairboon)
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
#include "logger.h"
#include "anarchNet.h"
#include "module_manager.h"
#include "config_manager.h"
#include "plugin_manager.h"
#include "plugins/bootstrap.h"
#include "plugins/localstorage.h"
#include "plugins/remotestorage.h"
#include "plugins/session.h"
#include "plugins/util.h"


namespace an
{
  
  bool PluginManager::init()
  {
    pugg::Server<an::plgdrv::Bootstrap>* bootstrap_server = 
    new pugg::Server<plgdrv::Bootstrap>(PLG_BOOTSTRAP_SERVER_NAME,PLG_BOOTSTRAP_SERVER_VERSION);
    pugg::Server<an::plgdrv::LocalStorage>* localstorage_server = 
    new pugg::Server<plgdrv::LocalStorage>(PLG_LOCALSTORAGE_SERVER_NAME,PLG_LOCALSTORAGE_SERVER_VERSION);
    pugg::Server<an::plgdrv::RemoteStorage>* remotestorage_server = 
    new pugg::Server<plgdrv::RemoteStorage>(PLG_REMOTESTORAGE_SERVER_NAME,PLG_REMOTESTORAGE_SERVER_VERSION);
    pugg::Server<an::plgdrv::Session>* session_server = 
    new pugg::Server<plgdrv::Session>(PLG_SESSION_SERVER_NAME,PLG_SESSION_SERVER_VERSION);
    pugg::Server<an::plgdrv::Util>* util_server = 
    new pugg::Server<plgdrv::Util>(PLG_UTIL_SERVER_NAME,PLG_UTIL_SERVER_VERSION);
    
    _plugin_kernel.addServer(bootstrap_server);
    _plugin_kernel.addServer(localstorage_server);
    _plugin_kernel.addServer(remotestorage_server);
    _plugin_kernel.addServer(session_server);
    _plugin_kernel.addServer(util_server);
    
    return true;
  }
  
  bool PluginManager::loadPlugin(const std::string& plugin_name, const std::string& plugin_path,const std::string& plugin_working_directory)
  {
    LOG(INFO) << "load plugin " << plugin_name;
    if(!_plugin_kernel.loadPlugin(plugin_path)) {
      LOG(ERROR) << "could not load plugin: " << plugin_path;
      return false; 
    }
    
    
    return true;
  }
  
  bool Plugin::get_config() {
    try {
      _config = an::ConfigManager::instance().modules_config(getName());
    }
    catch(boost::property_tree::ptree_error& e)
    {
      return false;
    }
    return true;
  }
}
