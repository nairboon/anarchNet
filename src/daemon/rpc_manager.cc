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
	
  _server->AddMethod(new Json::Rpc::RpcMethod<rpc::Bootstrap>(_bs, 
										&rpc::Bootstrap::BootstrapFromPeer, std::string("BootstrapFromPeer")));
  _server->AddMethod(new Json::Rpc::RpcMethod<rpc::Bootstrap>(_bs, 
										&rpc::Bootstrap::BootstrapFromHostlist, std::string("BootstrapFromHostlist")));

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
	
}
	
	
	
	
/*
void anRPCService::getInfo(awk::protobuf::RpcController* controller,
															 const Void* request,
															 InfoResponse* response,
															 google::protobuf::Closure* done)
{
	identify;
	response->set_anversion(ANARCHNET_VERSION);
#if ANARCHNET_PLATFORM == ANARCHNET_PLATFORM_APPLE
	response->set_operatingsystem(InfoResponse::osMac);
#elif ANARCHNET_PLATFORM == ANARCHNET_PLATFORM_WIN
	response->set_operatingsystem(InfoResponse::osWin);
#elif ANARCHNET_PLATFORM == ANARCHNET_PLATFORM_LINUX
	response->set_operatingsystem(InfoResponse::osLinux);
#elif defined (BSD)
	response->set_operatingsystem(InfoResponse::osBSD);
#else
	response->set_operatingsystem(InfoResponse::osOther);
#endif
	if (done)
		done->Run();
}*/
	
	
/*	void anRPCService::get(awk::protobuf::RpcController* controller,
					 const GetRequest* request,
					 GetResponse* response,
					 google::protobuf::Closure* done)
	{
		identify;
boost::bind(&anControlService::getCallback, this, _1, response, done));
		
	}
	void anControlService::getCallback(const std::string &result, GetResponse* response,google::protobuf::Closure* done)
	{
		kad::FindResponse msg;
		if (!msg.ParseFromString(result)) {
			printf("ERROR.  Invalid response. Kademlia Load Value key \n");
			
			response->mutable_status()->set_message("ERROR.  Invalid response.");
			response->mutable_status()->set_ok(false);
		} else if (msg.result() != kad::kRpcResultSuccess || msg.values_size() == 0) {
			printf("There is no value stored under key");
			response->mutable_status()->set_message("There is no value stored under key");
			response->mutable_status()->set_ok(false);
		} else {
			response->mutable_status()->set_ok(true);	
			
			/*
			 * \todo no iterating?
			 *
			for (int i = 0; i < msg.values_size(); ++i)
        response->add_values( msg.values(i) );
		}
		if(done)
			done->Run();
	}
	void anRPCService::put(awk::protobuf::RpcController* controller,
					 const PutRequest* request,
					 PutResponse* response,
					 google::protobuf::Closure* done)
	{
		identify;

		kad::KadId key;
		try {
			key = kad::KadId(request->key(), true);
		}
		catch(const kad::KadIdException&) {
			key = kad::KadId(cryobj_.Hash(request->key().c_str(), "", crypto::STRING_STRING,
																		false), false);
		}
			node_->StoreValue(key, request->value(),request->ttl(), boost::bind(
																											 &anControlService::putCallback, this, _1, response,done));
			}
	void anRPCService::putCallback(const std::string &result, PutResponse* response,google::protobuf::Closure* done)
	{
	
		
		kad::StoreResponse msg;
		if (!msg.ParseFromString(result)) {
						 response->mutable_status()->set_message("ERROR.  Invalid response.");
						 response->mutable_status()->set_ok(false);
		}
		else if (msg.result() != kad::kRpcResultSuccess) {
						 response->mutable_status()->set_message("Failed to store n copies of values");
						 response->mutable_status()->set_ok(false);
		} else {
			
				response->mutable_status()->set_ok(false);
		}
						 if(done)
						 done->Run();
	}

	void anControlService::storeObject(awk::protobuf::RpcController* controller,
									 const StoreObjectRequest* request,
									 CRUDResponse* response,
									 google::protobuf::Closure* done)
	{
		DBManager::instance().store_object(request);
		if (done)
			done->Run();
	}
	
	void anRPCService::deleteObject(awk::protobuf::RpcController* controller,
										const DeleteObjectRequest* request,
										CRUDResponse* response,
										google::protobuf::Closure* done)
	{
		DBManager::instance().delete_object(request);
		if (done)
			done->Run();
	}*/
}
