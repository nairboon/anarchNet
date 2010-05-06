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

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <glog/logging.h>

#include "protobuf_rpc_socket.h"
#include "maidsafe/transport/transportudt.h"
#include "maidsafe/protobuf/contact_info.pb.h"

#include "anarchNet.h"
#include "daemon.h"
#include "control_service.h"
#include "config_manager.h"
#include "connection_manager.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using std::string;

namespace an {
	

	
bool anDaemon::init(const string& directory)
{
	
	if(!ConfigManager::instance().init(directory))
		return false;
	
	if(!ConnectionManager::instance().init())
		return false;
	
	awk::protobuf::jerpc::socket_initialize();
	return true;
}
void anDaemon::run()
{
	control_service_ = new anControlService(ConnectionManager::instance().node());
	server_.RegisterService(control_service_);
	LOG(INFO) << "main run";
	server_.RunServer();
}

}
