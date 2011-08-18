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

#include "logger.h"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "version.h"
#include "anarchNet.h"
#include "db_manager.h"
#include <json/json.h>
#include "jsonrpc.h"
#include "config_manager.h"
#include "rpc_server.h"
#include "net_manager.h"

namespace an
{
	
	bool	NetManager::init() {
		_server = new RpcServer(_io_service, ConfigManager::instance().port());
		
		
		p2p::Discovery hs;
		_server->AddMethod(new Json::Rpc::RpcMethod<p2p::Discovery>(hs, &p2p::Discovery::Hello, std::string("Hello")));
		_server->AddMethod(new Json::Rpc::RpcMethod<p2p::Discovery>(hs, &p2p::Discovery::GetPeers, std::string("GetPeers")));
		return true;
	}	
	void NetManager::run() 
	{
		try {
			LOG(INFO) << "Start p2p networking";
			_io_service.run();
			LOG(INFO) << "Finished NetManager";
		}
		catch (std::exception& e)
		{
			LOG(ERROR) << "Exception: " << e.what() << "\n";
		}	
	}
	
	NetManager::~NetManager() {
		delete _server;
	}	
		
}
