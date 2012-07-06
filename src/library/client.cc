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
#include <cstdio>

#include "client.h"
#include "anarchNet.h"

//namespace an {
/*bool anClient::init()
{
	if(init_) //avoid double init
		return true;
	
	awk::protobuf::jerpc::socket_initialize();
	channel_ = new awk::protobuf::jerpc::SocketRpcChannel();
  if (!channel_->Connect("127.0.0.1", ANARCHNET_RPC_PORT)) {
		LOG(ERROR)<<"init(): Unable to connect to anDaemon: "<< ANARCHNET_RPC_PORT;
  }
	init_ = true;
	return true;
}
	
	google::protobuf::Message* Query(const std::string& name, const google::protobuf::Message*req)
	{
		RPCService* service;
		awk::protobuf::jerpc::SocketRpcController aController;
		service = new RPCService::Stub(an::anClient::instance().getChannel());
		const google::protobuf::MethodDescriptor* method = service->GetDescriptor()->FindMethodByName(name);
		
		if(method != NULL)		
			LOG(INFO) << "method: " << method->full_name();
		else 
			return NULL;
		
		google::protobuf::Message* res;
		if(method->name() == "getInfo")
			res = new InfoResponse;
	/*	else if (method->name() == "get")
			res = new GetResponse;
		else if (method->name() == "put")
			res = new PutResponse;
		else if (method->name() == "storeObject" || method->name() == "deleteObject")
			res = new CRUDResponse;*
		else {
			LOG(FATAL) << "unregistered method @liban";
			return NULL;
		}
		if(anClient::instance().getChannel()->QueryForMethod(service,method)) {
			 //method found
			 anClient::instance().getChannel()->CallMethod(method,&aController,req,res,NULL);
			 delete service;
			 return res;
		}
		else {
			 delete service;
			 LOG(WARNING) << "RPC-call to undefined method: " << name;
			 return NULL;
		}
	}*/
//}
