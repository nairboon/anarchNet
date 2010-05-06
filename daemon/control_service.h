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

#include "control_service.pb.h"
#include "maidsafe/maidsafe-dht.h"
#include "maidsafe/base/crypto.h"


#ifndef DAEMON_CONTROL_SERVICE_H
#define DAEMON_CONTROL_SERVICE_H

#define identify LOG(INFO) << __FUNCTION__ << " request"

namespace an {
	
	class anControlService : public ControlService {
	public:
		anControlService(	kad::KNode *n) : node_(n) { cryobj_.set_hash_algorithm(crypto::SHA_512); }
		void getInfo(awk::protobuf::RpcController* controller,
								 const Void* request,
								 InfoResponse* response,
								 google::protobuf::Closure* done);
		void get(awk::protobuf::RpcController* controller,
								 const GetRequest* request,
								 GetResponse* response,
								 google::protobuf::Closure* done);
		void put(awk::protobuf::RpcController* controller,
						 const PutRequest* request,
						 PutResponse* response,
						 google::protobuf::Closure* done);

private:
	
	void getCallback(const std::string &result, GetResponse* response,google::protobuf::Closure* done);
	void putCallback(const std::string &result, PutResponse* response,google::protobuf::Closure* done);

	kad::KNode *node_;
	crypto::Crypto cryobj_;

};
}
#endif