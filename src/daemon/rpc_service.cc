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

#include <glog/logging.h>
#include <iostream>
#include "version.h"
#include "anarchNet.h"
#include "db_manager.h"
#include <json/json.h>
#include "rpc_service.h"
#include "config_manager.h"

namespace an
{
	
bool	RPCManager::init() {
	_server = new Json::Rpc::TcpServer(std::string("127.0.0.1"), ConfigManager::instance().config()["rpc-port"].as<int>());
	
  if(!networking::init())
  {
    LOG(ERROR) << "Networking initialization failed";
		return false;
  }
	
	rpc::Bootstrap bs;
  _server->AddMethod(new Json::Rpc::RpcMethod<rpc::Bootstrap>(bs, &rpc::Bootstrap::BootstrapFromPeer, std::string("BootstrapFromPeer")));
  _server->AddMethod(new Json::Rpc::RpcMethod<rpc::Bootstrap>(bs, &rpc::Bootstrap::BootstrapFromHostlist, std::string("BootstrapFromHostlist")));
	return true;
}	
	void RPCManager::run() 
	{
		
		if(!_server->Bind())
		{
			LOG(ERROR) << "Bind failed";
			exit(EXIT_FAILURE);
		}
		
		if(!_server->Listen())
		{
			LOG(ERROR) << "Listen failed";
			exit(EXIT_FAILURE);
		}
		
		
		LOG(INFO) << "Start JSON-RPC server";
		
		while(running)
		{
			_server->WaitMessage(1000);
		}
		
		_server->Close();
	}
	
	RPCManager::~RPCManager() {
		LOG(INFO) << "Stop JSON-RPC TCP server";
		networking::cleanup();
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