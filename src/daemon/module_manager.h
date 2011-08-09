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
#include "plugins/bootstrap.h"


#ifndef DAEMON_CONNECTION_MANAGER_H_
#define DAEMON_CONNECTION_MANAGER_H_
namespace an {
	
class ModuleManager : public Singleton<ModuleManager>
	{
		friend class Singleton<ModuleManager>;
public:
		bool init();
		
			~ModuleManager() {}
		
		bool bootstrapFromPeer(const std::string&ip,int port);
		bool bootstrapFromHostlist(const std::string&url);

		
		static bool http_request(const std::string& url,std::string* response);
protected:
		ModuleManager() {}
private:
		std::vector<plg::Bootstrap*> _bootstrapers;
	//	transport::TransportHandler trans_handler_;
	//	rpcprotocol::ChannelManager *chmanager_;
		boost::int16_t trans_id_;
	};
}
#endif  // DAEMON_CONNECTION_MANAGER_H_
