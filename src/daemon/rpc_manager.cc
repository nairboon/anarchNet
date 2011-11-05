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

#include <iostream>
#include <boost/asio.hpp>
#include "version.h"
#include "rpc_server.h"
#include "rpc_manager.h"
#include "config_manager.h"
#include "logger.h"

namespace an
{

bool	RPCManager::init() {
	_server = new RpcServer(_io_service, ConfigManager::instance().rpc_port());

  _server->AddMethod(new Json::Rpc::RpcMethod<rpc::Util>(_bs,
										&rpc::Util::RuntimeInfo, std::string("an.info")));
  _server->AddMethod(new Json::Rpc::RpcMethod<rpc::Util>(_bs,
										&rpc::Util::BootstrapFromPeer, std::string("an.BootstrapFromPeer")));
  _server->AddMethod(new Json::Rpc::RpcMethod<rpc::Util>(_bs,
										&rpc::Util::BootstrapFromHostlist, std::string("an.BootstrapFromHostlist")));

	/*
	  @param: data content of the object
	  @param: owner id of the creator
	  @return: id the object id
	*/
	_server->AddMethod(new Json::Rpc::RpcMethod<rpc::LocalStorage>(_ls,
										&rpc::LocalStorage::CreateObject, std::string("object.create")));
	_server->AddMethod(new Json::Rpc::RpcMethod<rpc::LocalStorage>(_ls,
										&rpc::LocalStorage::GetObject, std::string("object.get")));
	_server->AddMethod(new Json::Rpc::RpcMethod<rpc::LocalStorage>(_ls,
										&rpc::LocalStorage::UpdateObject, std::string("object.update")));
	_server->AddMethod(new Json::Rpc::RpcMethod<rpc::LocalStorage>(_ls,
										&rpc::LocalStorage::DeleteObject, std::string("object.delete")));

	_server->AddMethod(new Json::Rpc::RpcMethod<rpc::anStore>(_ans,
										&rpc::anStore::kv_get, std::string("kv.get")));
	_server->AddMethod(new Json::Rpc::RpcMethod<rpc::anStore>(_ans,
										&rpc::anStore::kv_put, std::string("kv.put")));
	_server->AddMethod(new Json::Rpc::RpcMethod<rpc::anStore>(_ans,
										&rpc::anStore::kv_remove, std::string("kv.remove")));
	_server->AddMethod(new Json::Rpc::RpcMethod<rpc::anStore>(_ans,
										&rpc::anStore::session_t_join, std::string("session.t.join")));
	_server->AddMethod(new Json::Rpc::RpcMethod<rpc::anStore>(_ans,
										&rpc::anStore::session_t_leave, std::string("session.t.leave")));
	_server->AddMethod(new Json::Rpc::RpcMethod<rpc::anStore>(_ans,
										&rpc::anStore::session_t_update, std::string("session.t.update")));

	return true;
}
	void RPCManager::run()
	{
		try {
			LOG(INFO) << "Start local rpc server";
			_io_service.run();
			LOG(INFO) << "Stop JSON-RPC TCP server";
		}
		catch(std::exception& e)
		{
			LOG(ERROR) << "Exception: " << e.what() << "\n";
		}
	}

	RPCManager::~RPCManager() {
		//delete _server;
	}
namespace rpc {
	bool Util::BootstrapFromPeer(const boost::json::Value& root, boost::json::Value& response)
	{
		std::cout << "Receive query: " << root.get_str() << std::endl;
		response["jsonrpc"] = "2.0";
		response["id"] = root["id"];
		response["result"] = "success";
		return true;
	}

	bool Util::BootstrapFromHostlist(const boost::json::Value& root, boost::json::Value& response)
	{
	std::cout << "Notification: " << root.get_str() << std::endl;
		return true;
	}

	bool Util::RuntimeInfo(const boost::json::Value& root, boost::json::Value& response)
	{
	std::cout << "Notification: " << root.get_str() << std::endl;
			response["jsonrpc"] = "2.0";
		response["id"] = root["id"];
		response["result"] = "success";
		response["status"] = "running";
		return true;
	}

	bool LocalStorage::CreateObject(const boost::json::Value& root, boost::json::Value& response)
	{
		return false;
	}

	bool LocalStorage::GetObject(const boost::json::Value& root, boost::json::Value& response)
	{
		return false;
	}

	bool LocalStorage::DeleteObject(const boost::json::Value& root, boost::json::Value& response)
	{
		return false;
	}

	bool LocalStorage::UpdateObject(const boost::json::Value& root, boost::json::Value& response)
	{
		return false;
	}

	bool anStore::kv_get(const boost::json::Value& root, boost::json::Value& response)
	{

		return false;
	}

	bool anStore::kv_put(const boost::json::Value& root, boost::json::Value& response)
	{

		return false;
	}

	bool anStore::kv_remove(const boost::json::Value& root, boost::json::Value& response)
	{

		return false;
	}

	bool anStore::session_t_join(const boost::json::Value& root, boost::json::Value& response)
	{

		return false;
	}
	bool anStore::session_t_leave(const boost::json::Value& root, boost::json::Value& response)
	{

		return false;
	}
	bool anStore::session_t_update(const boost::json::Value& root, boost::json::Value& response)
	{

		return false;
	}



      bool RPC_Request::valid( Parameters& param) {
      	    //valid rpc
	    if(_json["jsonrpc"].is_null() || _json["jsonrpc"].type() != boost::json::str_type || _json["jsonrpc"].get_str() != "2.0") {
	      _error = "jsonrpc != 2.0";
	      LOG(INFO) << "rpcrequest: " << _error;
	      return false;
	    }

	    if(_json["id"].is_null() || _json["id"].type() != boost::json::str_type || _json["id"].get_str() == "") {
	      _error = "no id";
	      LOG(INFO) << "rpcrequest: " << _error;
	      return false;
	    }

	    // valid req
	      for(Parameters::iterator it = param.begin(); it != param.end(); it++) {
		if( _json[(*it).first].type() != (*it).second) {
		  _error = (*it).first + " has the wrong type";
		  return false;
		}
	      }
	      return true;
      }
}
}
