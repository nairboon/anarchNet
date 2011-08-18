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

#include "config.h"
#include "singleton.h"
#include "maidsafe/base/crypto.h"
#include <json/json.h>
#include "jsonrpc.h"


#ifndef DAEMON_RPC_SERVICE_H_
#define DAEMON_RPC_SERVICE_H_

#define identify LOG(INFO) << __FUNCTION__ << " request"

namespace an {
	
	class RPCManager : public Singleton<RPCManager>
	{
		friend class Singleton<RPCManager>;
	public:
		bool init();
		void run();
		void stop(){ _mutex.lock(); _running = false; _server->Close(); _mutex.unlock();}
		~RPCManager();
		RPCManager() : _running(true) {}
	private:
		Json::Rpc::TcpServer *_server;
		boost::mutex _mutex; 
		bool _running;
	};
	
	
	namespace rpc {
	class Bootstrap
		{
		public:
			/**
			 * \brief Reply with success.
			 * \param root JSON-RPC request
			 * \param response JSON-RPC response
			 * \return true if correctly processed, false otherwise
			 */
			bool BootstrapFromPeer(const Json::Value& root, Json::Value& response)
			{
				std::cout << "Receive query: " << root << std::endl;
				response["jsonrpc"] = "2.0";
				response["id"] = root["id"];
				response["result"] = "success";
				return true;
			}
			
			bool BootstrapFromHostlist(const Json::Value& root, Json::Value& response)
			{
				std::cout << "Notification: " << root << std::endl;
				response = Json::Value::null;
				return true;
			}
		};
	};

	/*class anRPCService : public RPCService {
	public:
		explicit anRPCService() { cryobj_.set_hash_algorithm(crypto::SHA_512); }
		void getInfo(awk::protobuf::RpcController* controller,
								 const Void* request,
								 InfoResponse* response,
								 google::protobuf::Closure* done);
/*		void get(awk::protobuf::RpcController* controller,
								 const GetRequest* request,
								 GetResponse* response,
								 google::protobuf::Closure* done);
		void put(awk::protobuf::RpcController* controller,
						 const PutRequest* request,
						 PutResponse* response,
						 google::protobuf::Closure* done);
		void storeObject(awk::protobuf::RpcController* controller,
						 const StoreObjectRequest* request,
						 CRUDResponse* response,
						 google::protobuf::Closure* done);
		void deleteObject(awk::protobuf::RpcController* controller,
						 const DeleteObjectRequest* request,
						 CRUDResponse* response,
						 google::protobuf::Closure* done);*
private:
	
//	void getCallback(const std::string &result, GetResponse* response,google::protobuf::Closure* done);
//	void putCallback(const std::string &result, PutResponse* response,google::protobuf::Closure* done);

	crypto::Crypto cryobj_;

};*/
}
#endif  // DAEMON_RPC_SERVICE_H_
