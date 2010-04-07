// Protocol Buffers Just Enough RPC - RPC with Google Protocol Buffers and Services
// Copyright 2010 Andrew Kimpton.  All rights reserved.
// http://code.google.com/p/protobuf-jerpc
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: awk@awkward.org

#ifndef __PROTOBUF_RPC_SERVER_H__
#define __PROTOBUF_RPC_SERVER_H__ 1

#include <string>
#include <map>
#include <set>
#include "protorpc.pb.h"

namespace awk { namespace protobuf { namespace jerpc {

class SocketRpcController;

struct RPCCompletionStatus {
   SocketRpcController *rpcController;
   Response *response;
};

class Server {
   public:
      Server();
      virtual ~Server() {};

      virtual void RegisterService(awk::protobuf::Service *aService);
      virtual awk::protobuf::Service* ServiceRegisteredForName(const std::string &serviceName);

      virtual std::string PerformRPC(const std::string &rpcBuffer);

   protected:
      virtual Response ExecuteRPCRequest(const Request &rpcRequest);
      virtual void ExecuteRPCDone(RPCCompletionStatus *completionStatus);

      std::map<const std::string, awk::protobuf::Service*> mServiceInstanceMap;
};

class NetServer : public Server {
   public:
      NetServer();
      virtual ~NetServer();

      virtual void RunServer();

   protected:
      virtual bool HandleRequestFromClient(int clientSocket);
      virtual int AcceptClientConnection(int listeningSocket);

   private:
      int mServerListeningSocket;
      std::set<int> mSocketSet;
};

} // namespace jerpc
} // namespace protobuf
} // namespace awk

#endif // __PROTOBUF_RPC_SERVER_H__
