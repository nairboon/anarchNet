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

#include "singleton.h"

#ifndef SRC_DAEMON_RPC_MANAGER_H_
#define SRC_DAEMON_RPC_MANAGER_H_

#define identify LOG(INFO) << __FUNCTION__ << " request"

namespace an {

	namespace rpc {
	class Bootstrap {
	 public:
			/**
			 * \brief Reply with success.
			 * \param root JSON-RPC request
			 * \param response JSON-RPC response
			 * \return true if correctly processed, false otherwise
			 */
			bool BootstrapFromPeer(const boost::json::Value& root, boost::json::Value& response)
			{
				std::cout << "Receive query: " << root.get_str() << std::endl;
				response["jsonrpc"] = "2.0";
				response["id"] = root["id"];
				response["result"] = "success";
				return true;
			}
			
			bool BootstrapFromHostlist(const boost::json::Value& root, boost::json::Value& response)
			{
				std::cout << "Notification: " << root.get_str() << std::endl;
				return true;
			}
		};
	
	class LocalStorage {
	 public:
			bool CreateObject(const boost::json::Value& root, boost::json::Value& response);
			bool GetObject(const boost::json::Value& root, boost::json::Value& response);
			bool DeleteObject(const boost::json::Value& root, boost::json::Value& response);
			bool UpdateObject(const boost::json::Value& root, boost::json::Value& response);
		};
		
	class anStore {
	 public:
			bool kv_get(const boost::json::Value& root, boost::json::Value& response);
			bool kv_put(const boost::json::Value& root, boost::json::Value& response);
			bool kv_remove(const boost::json::Value& root, boost::json::Value& response);
			
			bool session_t_join(const boost::json::Value& root, boost::json::Value& response);
			bool session_t_leave(const boost::json::Value& root, boost::json::Value& response);
			bool session_t_update(const boost::json::Value& root, boost::json::Value& response);

		};
};

class RPCManager : public Singleton<RPCManager>	{
		friend class Singleton<RPCManager>;
 public:
		bool init();
		void run();
		void stop(){ _io_service.stop(); }
		~RPCManager();
		RPCManager() : _running(true) {}
 private:
		an::RpcServer *_server;
		boost::asio::io_service _io_service;
		bool _running;
		rpc::Bootstrap _bs;
		rpc::anStore _ans;
		rpc::LocalStorage _ls;
		//rpc::RemoteStorage _rs;
		//rpc::Session _s;
	};
	
	/*class anRPCService : public RPCService {
	public:
		explicit anRPCService() { cryobj_.set_hash_algorithm(crypto::SHA_512); }
		void getInfo(awk::protobuf::RpcController* controller,
								 const Void* request,
								 InfoResponse* response,
								 google::protobuf::Closure* done);
/		void get(awk::protobuf::RpcController* controller,
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
#endif  // SRC_DAEMON_RPC_MANAGER_H_
