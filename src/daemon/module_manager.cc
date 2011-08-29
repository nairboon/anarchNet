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

#include <string>
#include <boost/asio.hpp>
#include <boost/foreach.hpp>
#include "anarchNet.h"
#include "logger.h"
#include "module_manager.h"
#include "plugin_manager.h"
#include "config_manager.h"
#include "plugins/bootstrap.h"
#include "plugins/localstorage.h"
#include "plugins/remotestorage.h"
#include "plugins/session.h"

namespace an
{

bool ModuleManager::init()
{
	BOOST_FOREACH (std::string plugin_name, ConfigManager::instance().modules()) {
		if(!PluginManager::instance().loadPlugin(plugin_name,plugin_name+".dylib",plugin_name))
			return false;
	}
	
	std::vector<plgdrv::Bootstrap*> bs_drivers;
	dynamic_cast<pugg::Server<an::plgdrv::Bootstrap>*>(PluginManager::instance().get_kernel().getServer(PLG_BOOTSTRAP_SERVER_NAME))->getAllDrivers(bs_drivers);
	BOOST_FOREACH (plgdrv::Bootstrap* drv, bs_drivers) {
		plg::Bootstrap* plg = drv->createPlugin();
		LOG(INFO)<< plg->getName() << " created";
		plg->initialise();
		LOG(INFO)<< plg->getName() << " initialized";
		_bootstrapers.push_back(plg);
	}
	
	std::vector<plgdrv::LocalStorage*> ls_drivers;
	dynamic_cast<pugg::Server<an::plgdrv::LocalStorage>*>(PluginManager::instance().get_kernel().getServer(PLG_LOCALSTORAGE_SERVER_NAME))->getAllDrivers(ls_drivers);
	BOOST_FOREACH (plgdrv::LocalStorage* drv, ls_drivers) {
		plg::LocalStorage* plg = drv->createPlugin();
		LOG(INFO)<< plg->getName() << " created";
		plg->initialise();
		LOG(INFO)<< plg->getName() << " initialized";
		_localstorages.push_back(plg);
	}
	
	std::vector<plgdrv::RemoteStorage*> rs_drivers;
	dynamic_cast<pugg::Server<an::plgdrv::RemoteStorage>*>(PluginManager::instance().get_kernel().getServer(PLG_REMOTESTORAGE_SERVER_NAME))->getAllDrivers(rs_drivers);
	BOOST_FOREACH (plgdrv::RemoteStorage* drv, rs_drivers) {
		plg::RemoteStorage* plg = drv->createPlugin();
		LOG(INFO)<< plg->getName() << " created";
		plg->initialise();
		LOG(INFO)<< plg->getName() << " initialized";
		_remotestorages.push_back(plg);
	}
	
	std::vector<plgdrv::Session*> s_drivers;
	dynamic_cast<pugg::Server<an::plgdrv::Session>*>(PluginManager::instance().get_kernel().getServer(PLG_SESSION_SERVER_NAME))->getAllDrivers(s_drivers);
	BOOST_FOREACH (plgdrv::Session* drv, s_drivers) {
		plg::Session* plg = drv->createPlugin();
		LOG(INFO)<< plg->getName() << " created";
		plg->initialise();
		LOG(INFO)<< plg->getName() << " initialized";
		_sessions.push_back(plg);
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
			if( (*it)->getType() == plg::LocalStorage::PLAIN && (*it)->kv_put(key,value))
				return true;
		
		return false;
	}
	
	const std::string ModuleManager::kv_get(const std::string& key)
	{
		std::string res;
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->getType() == plg::LocalStorage::PLAIN && ( (res = (*it)->kv_get(key)) != ""))
				return res;
		
		return "";
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
	
	bool ModuleManager::get_file_path(const std::string& id,std::string& res) {
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->getType() == plg::LocalStorage::BINARY && (*it)->get_file_path(id,res))
				return true;
		
		return false;
	}
	
	bool ModuleManager::remove_file(const std::string& id) {
		for(std::vector<plg::LocalStorage*>::iterator it = _localstorages.begin(); it != _localstorages.end(); it++)
			if( (*it)->getType() == plg::LocalStorage::BINARY && (*it)->remove_file(id))
				return true;
		
		return false;
	}
	
	/*bool ModuleManager::http_request(const std::string& _url,std::string *_response)
	{
    std::size_t index = _url.find_first_of('/');
    std::string url_server = _url.substr(0,index);
		std::string url_file = _url.substr(index);
		
		LOG(INFO) << "bootstrapping " << url_file << " from " << url_server;
		
		std::stringstream out;
		try {
		boost::asio::io_service io_service;
		
    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(url_server, "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
		
    // Try each endpoint until we successfully establish a connection.
    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
      socket.close();
      socket.connect(*endpoint_iterator++, error);
    }
    if (error)
      throw boost::system::system_error(error);
		
    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET " << url_file << " HTTP/1.0\r\n";
    request_stream << "Host: " << url_server << "\r\n";
    request_stream << "Accept: *//*\r\n";
    request_stream << "Connection: close\r\n\r\n";
		
    // Send the request.
    boost::asio::write(socket, request);
		
    // Read the response status line.
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");
		
    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
      LOG(WARNING) << "http: Invalid response\n";
      return false;
    }
    if (status_code != 200)
    {
      LOG(WARNING) << "http: Response returned with status code " << status_code << "\n";
      return false;
    }
		
    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, response, "\r\n\r\n");
		
    // Write whatever content we already have to output.
			std::stringstream buffer;
    if (response.size() > 0)
      buffer << &response;
		
    // Read until EOF, writing data to output as we go.
    while (boost::asio::read(socket, response,
														 boost::asio::transfer_at_least(1), error))
      out << &response;
    if (error != boost::asio::error::eof)
      throw boost::system::system_error(error);
  }
  catch (const std::exception& e)
  {
    LOG(ERROR) << "http: Exception: " << e.what() << "\n";
		return false;
		}

	*_response = out.str();
return true;
}*/
}
