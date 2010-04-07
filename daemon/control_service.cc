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
#include "glog/logging.h"
#include "control_service.h"
#include "version.h"
using namespace std;
namespace an
{

void anControlService::getInfo(awk::protobuf::RpcController* controller,
															 const Void* request,
															 InfoResponse* response,
															 google::protobuf::Closure* done)
{
	identify;
	response->set_anversion(ANARCHNET_VERSION);
	response->set_anprotocolversion(ANARCHNET_PROTOCOL_VERSION);
#if defined (__APPLE__)
	response->set_operatingsystem(InfoResponse::osMac);
#elif defined (_WIN32)
	response->set_operatingsystem(InfoResponse::osWin);
#elif defined (__LINUX)
	response->set_operatingsystem(InfoResponse::osLinux);
#elif defined (BSD)
	response->set_operatingsystem(InfoResponse::osBSD);
#else
	response->set_operatingsystem(InfoResponse:osOther);
#endif
	if (done)
		done->Run();
}
}
