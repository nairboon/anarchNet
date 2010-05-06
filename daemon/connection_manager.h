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

#include <vector>
#include "singleton.h"
#include "maidsafe/maidsafe-dht.h"


#ifndef DAEMON_CONNECTION_MANAGER_H_
#define DAEMON_CONNECTION_MANAGER_H_
namespace an {
	
class ConnectionManager : public Singleton<ConnectionManager>
	{
		friend class Singleton<ConnectionManager>;
public:
		bool init();
		
		bool bootstrapp_from_cache();
		bool bootstrapp_from_server();
		
		// once the network is big enough (serveral thousend nodes)
		// we could bootstrapp with a random ip search/dns profile *gnunet
		//bool bootstrapp_from_randomsearch();
		~ConnectionManager() {if(chmanager_ != NULL) delete chmanager_; if(node_ != NULL) delete node_;}
		kad::KNode *node() { return node_;}
		
		static bool http_request(std::string url,std::string& response);
protected:
		ConnectionManager() : chmanager_(NULL), node_(NULL) {}
private:
		class JoinCallback {
		public:
			JoinCallback() : result_arrived_(false), success_(false) {}
			void Callback(const std::string &result) {
				base::GeneralResponse msg;
				if (!msg.ParseFromString(result))
					success_ = false;
				else if (msg.result() == kad::kRpcResultSuccess)
					success_ = true;
				else
					success_ = false;
				result_arrived_ = true;
			}
			bool result_arrived() const { return result_arrived_; }
			bool success() const { return success_; }
		private:
			bool result_arrived_, success_;
		};
		
		bool kad_init();
		bool kad_shutdown();
		void create_network(JoinCallback* cb);
		bool kadconfig_empty(const std::string &path);
		bool write_to_kadconfig(const std::string &path, const std::string &node_id,
														const std::string &ip, const boost::uint16_t &port,
														const std::string &local_ip, const boost::uint16_t &local_port);
		
		transport::TransportHandler trans_handler_;
		rpcprotocol::ChannelManager *chmanager_;
		kad::KNode *node_;
		boost::int16_t trans_id_;
	};
}
#endif
