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
		NetManager() : running_(true) {}
		~NetManager();
		void stop(){ mutex.lock(); running_ = false; mutex.unlock();}
	private:
		Json::Rpc::TcpServer *_server;
		boost::mutex mutex;
		bool running_;
	};
	
	namespace p2p {
		class Hello
			{
			public:
				/**
				 * \brief Reply with success.
				 * \param root JSON-RPC request
				 * \param response JSON-RPC response
				 * \return true if correctly processed, false otherwise
				 */
				bool Hello(const Json::Value& root, Json::Value& response)
				{
					
				}
				bool GetPeers(const Json::Value& root, Json::Value& response)
				{
					
				}
			};
	}
}
#endif  // DAEMON_NET_MANAGER_H_
