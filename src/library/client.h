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
#include <queue>
#include "singleton.h"

#ifndef LIBRARY_CLIENT_H_
#define LIBRARY_CLIENT_H_
namespace an {
/*
class anClient : public Singleton<anClient>
{
	friend class Singleton<anClient>;
public:
	bool init();
	awk::protobuf::jerpc::SocketRpcChannel* getChannel() { if(!init_)init(); return channel_; }
private:
	anClient() : init_(false) { }
	awk::protobuf::jerpc::SocketRpcChannel* channel_;
	bool init_;
};
	/**
	 * @brief generic rpc request
	 * @param name name of the rpc method
	 *
	google::protobuf::Message* Query(const std::string& name, const google::protobuf::Message* request);
}*/
}
#endif  // LIBRARY_CLIENT_H_
