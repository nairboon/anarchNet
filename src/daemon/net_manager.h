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


#ifndef DAEMON_NET_MANAGER_H_
#define DAEMON_NET_MANAGER_H_

namespace an {
	
	class NetManager : public Singleton<NetManager>
	{
		friend class Singleton<NetManager>;
	public:
		bool init();
		void run();
		NetManager() : _running(true) {}
		~NetManager();
		void stop(){ _mutex.lock(); _running = false; _io_service.stop(); _mutex.unlock();}
	private:
		an::RpcServer *_server;
		boost::asio::io_service _io_service;
		boost::mutex _mutex;
		bool _running;
	};
	
	namespace p2p {
		class Discovery
			{
			public:
				/**
				 * \brief Hello request used to bootstrap from other peers
				 * \param root JSON-RPC request
				 * \param response JSON-RPC response
				 * \return true if correctly processed, false otherwise
				 */
				bool Hello(const boost::json::Value& root, boost::json::Value& response)
				{
					
				}
				bool GetPeers(const boost::json::Value& root, boost::json::Value& response)
				{
					
				}
			};
	}
}
#endif  // DAEMON_NET_MANAGER_H_
