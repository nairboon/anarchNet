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

#include "client.h"


#include "info.h"

/*InfoResponse* getInfo(void)
{
	RPCService* service;
	Void request;
	InfoResponse *response = new InfoResponse;
	awk::protobuf::jerpc::SocketRpcController aController;
	
  service = new RPCService::Stub(an::anClient::instance().getChannel());
		
  // Execute the RPC.
  service->getInfo(&aController, &request, response, NULL);

	// printf("%s: response.anversion = %d\n", __FUNCTION__, response->anversion());
	
  delete service;
	return response;
}*/