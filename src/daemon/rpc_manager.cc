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
#include <istream>
#include <boost/asio.hpp>
#include <boost/foreach.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#define BUFFERSIZE 16777216
#include <b64/encode.h>
#include <b64/decode.h>
#include "db_manager.h"
#include "version.h"
#include "rpc_server.h"
#include "rpc_manager.h"
#include "config_manager.h"
#include "db.h"
#include "logger.h"
#include "module_manager.h"

namespace ai = boost::archive::iterators;

namespace an
{

bool	RPCManager::init() {
	_server = new RpcServer(_io_service, ConfigManager::instance().rpc_port(),ConfigManager::instance().threads());

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

	_server->AddMethod(new Json::Rpc::RpcMethod<rpc::LocalStorage>(_ls,
										&rpc::LocalStorage::file_put, std::string("file.put")));	
	_server->AddMethod(new Json::Rpc::RpcMethod<rpc::LocalStorage>(_ls,
										&rpc::LocalStorage::file_get, std::string("file.get")));
	
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
			_server->run();
			LOG(INFO) << "Stop JSON-RPC TCP server";
		}
		catch(std::exception& e)
		{
			LOG(ERROR) << "Exception: " << e.what() << "\n";
		}
	}

	RPCManager::~RPCManager() {
		delete _server;
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
	LOG(INFO) << "runtime info";
	RPC_Response res = RPC_Request(root).createResponse();
	boost::json::Config::add(res.data(),"uptime","1");
	response = res.json();
	return true;
	}

	bool LocalStorage::file_put(const boost::json::Value& root, boost::json::Value& response)
	{
	  RPC_Request req(root);
	  rpc::RPC_Request::Parameters rules;
	  rules["path"] = boost::json::str_type;
	  if(!req.valid(rules)) {
	    LOG(INFO) << "invalid request";
	    response = req.createErrorResponse().json();
	    return false;
	  }
	  
	  std::string path = req.params()["path"].get_str();
	  std::string id;
	  RPC_Response res = req.createResponse();
	  LOG(INFO) << "put file: " << id;
	  if(!ModuleManager::instance().store_file(path,id)) {
	    LOG(INF) << "could not store file: "<< id;
	    res = req.createErrorResponse();
	    res.json()["error"] = "store file failed";
	    response = res.json();
	    return false;
	  }
	  boost::json::Config::add(res.data(),"id",id);
	  
	  response = res.json();
	  
	  return true;
	}
	
	bool LocalStorage::file_get(const boost::json::Value& root, boost::json::Value& response)
	{
	  RPC_Request req(root);
	  rpc::RPC_Request::Parameters rules;
	  rules["id"] = boost::json::str_type;
	  if(!req.valid(rules)) {
	    LOG(INFO) << "invalid request";
	    response = req.createErrorResponse().json();
	    return false;
	  }
	  
	  std::string id = req.params()["id"].get_str();
	  std::string path;
	  RPC_Response res = req.createResponse();
	  LOG(INFO) << "get file: " << id;
	  if(!ModuleManager::instance().get_file(id,path)) {
	    LOG(INF) << "could not get file: "<< id;
	    res = req.createErrorResponse();
	    res.json()["error"] = "get file failed";
	    response = res.json();
	    return false;
	  }

	  boost::json::Config::add(res.data(),"path",path);
	  
	  response = res.json();
	  return true;
	}
			
	bool LocalStorage::CreateObject(const boost::json::Value& root, boost::json::Value& response)
	{
	  	RPC_Request req(root);
		rpc::RPC_Request::Parameters rules;
		rules["content"] = boost::json::str_type;
		if(!req.valid(rules)) {
		  LOG(INFO) << "invalid request";
		 response = req.createErrorResponse().json();
		  return false;
		}
		
		db::ObjPtr obj(new db::Object());
		
		 if(!req.params()["id"].is_null() && req.params()["id"].type() == boost::json::str_type) {
		  std::string custom_id = req.params()["id"].get_str();
		    LOG(INFO) << "using custom id: " << custom_id;
		  
		   // is it a new object or update?
		    if(DBManager::instance().get_object_head(custom_id,obj)) {
		      LOG(INFO) << "updating object....";
		      if(!DBManager::instance().update_object(obj,req.decode_base64("content"))) {
			LOG(INFO) << "could not update object";
			 RPC_Response res = req.createErrorResponse();
			res.json()["err"] = "update object failed";
			  response = res.json();
			  return false;
		      }
		      else {
			LOG(INFO) << "obj updated";
			RPC_Response res = req.createResponse();
			boost::json::Config::add(res.data(),"id",obj->id);
			response = res.json();
			return true;
		      }
		    }
		    else {
		      //its a new object
		      obj->create(req.decode_base64("content"),custom_id);
		    }
		   
		}
		else
		  obj->create(req.decode_base64("content"));
		
		LOG(INFO) << "stored: " << req.decode_base64("content");
		LOG(INFO) << "o: " << req.params()["content"].get_str();
		if(!DBManager::instance().create_object(obj)) {
			LOG(INFO) << "could not store";

		 RPC_Response res = req.createErrorResponse();
		 res.json()["err"] = "create object failed";
		 response = res.json();
		 return false;
		}

		RPC_Response res = req.createResponse();
		LOG(INFO) << "stored under: " << obj->id << " : " << obj->head;
		boost::json::Config::add(res.data(),"id",obj->id);
		response = res.json();
		return true;
	}

	bool LocalStorage::GetObject(const boost::json::Value& root, boost::json::Value& response)
	{
	  	RPC_Request req(root);
		rpc::RPC_Request::Parameters rules;
		rules["id"] = boost::json::str_type;
		if(!req.valid(rules)) {
		  LOG(INFO) << "invalid request";
		 response = req.createErrorResponse().json();
		  return false;
		}
		RPC_Response res = req.createResponse();

		db::ObjPtr obj(new db::Object());
		db::ObjID id = req.params()["id"].get_str();
		LOG(INFO) << "get obj: " << id;
		if(!DBManager::instance().get_object_head(id,obj)) {
			LOG(INF) << "could not get obj: "<< id;
		 res = req.createErrorResponse();
		 res.json()["error"] = "get object failed";
		 response = res.json();
		 return false;
		}

		bool full = false;
		if(req.params().contains("full")) {
		  full = req.params()["full"].get_bool();
		}

		if(full) {
		RPC_Response res = req.createResponse();
		LOG(INFO) << "got object: " << obj->id;
		boost::json::Array diffs,snapshots;
		/*BOOST_FOREACH (db::SnapshotPtr ss, obj->snapshots) {
		  diffs.push_back(ss->c);
		  //boost::json::Config::add(snapshots,
			boost::shared_ptr<db::Snapshot> s = _db_store_snapshot(ss);
			dobj.snapshots().link(*s);
		}
		BOOST_FOREACH (an::db::DiffPtr ss, obj->diffs) {
			boost::shared_ptr<db::Snapshot> s = _db_store_snapshot(ss);
			dobj.snapshots().link(*s);

		}*/
				boost::json::Config::add(res.data(),"content",obj->id);

		response = res.json();
		}
		else {
		  std::string content = obj->head;
		  
		  std::string econtent = res.encode_base64(content);
		  LOG(INFO) << content.size() << " (byes) -> "<<econtent.size() << " (bytes,hex) sending: " << content;
		  boost::json::Config::add(res.data(),"content",econtent);
		}
		response = res.json();
		return true;
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
	  	RPC_Request req(root);
		rpc::RPC_Request::Parameters rules;
		rules["key"] = boost::json::str_type;
		if(!req.valid(rules)) {
		  LOG(INFO) << "invalid request";
		 response = req.createErrorResponse().json();
		  return false;
		}
	  std::string value;
	  an::KV_ResPtr kv_res;
		if(!ModuleManager::instance().kv_get(req.params()["key"].get_str(),kv_res)) {
			LOG(INF) << "could not get";

		 RPC_Response res = req.createErrorResponse();
		 res.json()["error"] = "kv.get failed";
		 response = res.json();
		 return false;
		}
		
		RPC_Response res = req.createResponse();
		LOG(INFO) << "got " << kv_res->size() << " keys";
		value = (*kv_res)[req.params()["key"].get_str()];
		value = res.encode_base64(value);
		LOG(INFO) << "got value for key: " << req.params()["key"].get_str()<< " " << value;
		boost::json::Config::add(res.data(),"content",value);
		response = res.json();
		return true;
	}

	bool anStore::kv_put(const boost::json::Value& root, boost::json::Value& response)
	{
	  	RPC_Request req(root);
		rpc::RPC_Request::Parameters rules;
		rules["key"] = boost::json::str_type;
		rules["value"] = boost::json::str_type;
		if(!req.valid(rules)) {
		  LOG(INFO) << "invalid request";
		 response = req.createErrorResponse().json();
		  return false;
		}

		std::string content = req.decode_base64("value");
		if(!ModuleManager::instance().kv_put(req.params()["key"].get_str(),content)) {
			LOG(INF) << "could not store";

		 RPC_Response res = req.createErrorResponse();
		 res.json()["error"] = "kv.put failed";
		 response = res.json();
		 return false;
		}

		RPC_Response res = req.createResponse();
		LOG(INFO) << "stored key: " << req.params()["key"].get_str();
		response = res.json();
		return true;
	}

	bool anStore::kv_remove(const boost::json::Value& root, boost::json::Value& response)
	{
	  	RPC_Request req(root);
		rpc::RPC_Request::Parameters rules;
		rules["key"] = boost::json::str_type;
		if(!req.valid(rules)) {
		  LOG(INFO) << "invalid request";
		 response = req.createErrorResponse().json();
		  return false;
		}
		
		if(!ModuleManager::instance().kv_remove(req.params()["key"].get_str())) {
			LOG(INF) << "could not remove";

		 RPC_Response res = req.createErrorResponse();
		 res.json()["error"] = "kv.remove failed";
		 response = res.json();
		 return false;
		}

		RPC_Response res = req.createResponse();
		LOG(INFO) << "removed key: " << req.params()["key"].get_str();
		response = res.json();
		return true;
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

	    if(_json["id"].is_null() || _json["id"].type() != boost::json::int_type || _json["id"].get_int() == 0) {
	      _error = "no id";
	      LOG(INFO) << "rpcrequest: " << _error;
	      return false;
	    }

	    boost::json::Value params = _json["params"];
	    // valid req
	      for(Parameters::iterator it = param.begin(); it != param.end(); it++) {
		if( params[(*it).first].type() != (*it).second) {
		  LOG(INFO) << params[(*it).first].type() << " != " << (*it).second;
		  _error = (*it).first + " has the wrong type";
		  return false;
		}
	      }
	      return true;
      }
      
      
      
      std::string RPC_Request::decode_base64(const std::string& id)
      {
	// field here?
	if(_json["params"][id].is_null() || _json["params"][id].type() != boost::json::str_type) {
	  _error = id + " not found";
	  LOG(INFO) << "rpcrequest: " << _error;
	  throw _error; //FIXME
	}
	
	
	std::stringstream res;
	std::istringstream i(_json["params"][id].get_str());
	//LOG(INFO) << "decode_base64: " << i.str();
	base64::decoder D;
	D.decode(i,res);
	//LOG(INFO) << res.str();
	return res.str();
      }
      
      std::string RPC_Response::encode_base64(std::string& inp)
      {
	std::stringstream res;
	std::istringstream i(inp);
	base64::encoder E;
	E.encode(i,res);
	return res.str();
      }
}
}
