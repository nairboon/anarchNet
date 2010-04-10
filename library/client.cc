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
#include <cstdio>
#include "client.h"
#include "anarchNet.h"

namespace an {
bool anClient::init()
{
	awk::protobuf::jerpc::socket_initialize();
	channel_ = new awk::protobuf::jerpc::SocketRpcChannel();
  if (!channel_->Connect("127.0.0.1", RPC_CONTROL_PORT)) {
		fprintf(stderr, "%s: Unable to connect to remote server\n", __FUNCTION__);
  }
	return true;
}
}
