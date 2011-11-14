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

#include <map>

#include "singleton.h"

#ifndef SRC_DAEMON_RPC_MANAGER_H_
#define SRC_DAEMON_RPC_MANAGER_H_

#define identify LOG(INFO) << __FUNCTION__ << " request"
#include <boost/json/value.hpp>

namespace an {

	namespace rpc {


	  class RPC_Response {
	     boost::json::Value _json;

	     inline void _init() {
	       boost::json::Object obj,err,data;
	       obj.push_back(boost::json::Pair("jsonrpc","2.0"));
	       obj.push_back(boost::json::Pair("data",data));
	       obj.push_back(boost::json::Pair("err",""));

	       _json = obj;
	     }
	    public:
	      RPC_Response() {
		_init();
	      }
	      RPC_Response(int id) { _init(); boost::json::Config::add(_json.get_obj(),"id",id); }
	      boost::json::Value& json() { return _json; }
	      boost::json::Object& data() { return _json["data"].get_obj(); }
	  };

	  class RPC_Request {
	    boost::json::Value _json;
	    std::string _error;
	    public:
	      typedef std::map<std::string,boost::json::Value_type> Parameters;
	    RPC_Request() {}
	    RPC_Request(const boost::json::Value& inp) : _json(inp) { LOG(INFO) << boost::json::write(inp); }

	    bool valid( Parameters& param);
	    boost::json::Value& json() { return _json; }
	    boost::json::Value& params() { return _json["params"]; }
	    RPC_Response createResponse() { return RPC_Response(_json["id"].get_int()); }
	    RPC_Response createErrorResponse() { RPC_Response res(_json["id"].get_int()); res.json()["err"] = _error; return res; }
	  };


	class Util {
	 public:
			/**
			 * \brief  Use another peer to bootstrap
			 * \param ip IP of the User
			 * \param port port of the User
			 * \return true if correctly processed, false otherwise
			 */
			bool BootstrapFromPeer(const boost::json::Value& root, boost::json::Value& response);
			bool BootstrapFromHostlist(const boost::json::Value& root, boost::json::Value& response);
			bool RuntimeInfo(const boost::json::Value& root, boost::json::Value& response);
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
		rpc::Util _bs;
		rpc::anStore _ans;
		rpc::LocalStorage _ls;
		//rpc::RemoteStorage _rs;
		//rpc::Session _s;
	};
}
#endif  // SRC_DAEMON_RPC_MANAGER_H_
