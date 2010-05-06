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

#include "service.h"
#include "protobuf_rpc_server.h"
#include "maidsafe/protobuf/general_messages.pb.h"

#ifndef DAEMON_DAEMON_H
#define DAEMON_DAEMON_H
namespace an {

	
class anDaemon
{
public:
	anDaemon(): control_service_(NULL)  {}
	~anDaemon() { if(control_service_ != NULL) delete control_service_; }
	bool init(const std::string&);
	void run();
private:
	awk::protobuf::Service* control_service_; 
	awk::protobuf::jerpc::NetServer server_;
	
};
}
#endif
