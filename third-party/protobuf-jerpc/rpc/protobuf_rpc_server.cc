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

#include <sstream>
#include "protobuf_rpc_server.h"
#include "protobuf_rpc.h"

#include "service.h"
#include "google/protobuf/descriptor.h"
#include "anarchNet.h"

#include "protobuf_rpc_socket.h"

using namespace google::protobuf;
using namespace awk::protobuf::jerpc;

class ConcreteDiscoveryService : public DiscoveryService {
 public:
  ConcreteDiscoveryService(Server *aServer) : DiscoveryService(), mServer(aServer) {};

  void QueryForService(awk::protobuf::RpcController* controller,
                       const QueryForServiceRequest* request,
                       QueryForServiceResponse* response,
                       Closure* done);

   private:
      Server *mServer;
};

Server::Server()
{
   awk::protobuf::Service *discoveryService = new ConcreteDiscoveryService(this);
   RegisterService(discoveryService);

}

void Server::RegisterService(awk::protobuf::Service *aService)
{
   mServiceInstanceMap[aService->GetDescriptor()->full_name()] = aService;
}

awk::protobuf::Service *Server::ServiceRegisteredForName(const std::string &serviceName)
{
   return mServiceInstanceMap[serviceName];
}

std::string Server::PerformRPC(const std::string &RPCRequestString)
{
   Request anRPCRequest;
   Response anRPCResponse;

   anRPCRequest.ParseFromString(RPCRequestString);

   anRPCResponse = ExecuteRPCRequest(anRPCRequest);

   std::string RPCResponseString;
   anRPCResponse.SerializeToString(&RPCResponseString);

   return RPCResponseString;
}

void Server::ExecuteRPCDone(RPCCompletionStatus *completionStatus)
{
   if (completionStatus->rpcController->Failed()) {
      completionStatus->response->set_error_reason(completionStatus->rpcController->GetErrorReason());
      completionStatus->response->set_error(completionStatus->rpcController->ErrorText());
   }
}

Response Server::ExecuteRPCRequest(const Request &rpcRequest)
{
   SocketRpcController controller;
   awk::protobuf::Service* service = NULL;
   Response anRPCResponse;

   service = mServiceInstanceMap[rpcRequest.service_name().c_str()];
   if (!service) {
      std::ostringstream errorString;
      errorString << "Service " << rpcRequest.service_name() << " not registered.";
      anRPCResponse.set_error_reason(SERVICE_NOT_FOUND);
      anRPCResponse.set_error(errorString.str());
      return anRPCResponse;
   }

   const MethodDescriptor* method = service->GetDescriptor()->FindMethodByName(rpcRequest.method_name());

   Message *request = service->GetRequestPrototype(method).New();
   Message *response = service->GetResponsePrototype(method).New();;
   request->ParseFromString(rpcRequest.request_proto());
   RPCCompletionStatus *completionStatus = new RPCCompletionStatus;
   completionStatus->rpcController = &controller;
   completionStatus->response = &anRPCResponse;
   service->CallMethod(method, &controller, request, response, NewCallback(this, &Server::ExecuteRPCDone, completionStatus));

   delete request;
   delete completionStatus;

   std::string responseString;
   response->SerializeToString(&responseString);
   delete response;

   anRPCResponse.set_response_proto(responseString);

   return anRPCResponse;
}

NetServer::NetServer() : Server()
{
}

NetServer::~NetServer()
{
}

void NetServer::RunServer()
{
   int maxFD = 0;
   int result = 0;
   bool done = false;

   mServerListeningSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (mServerListeningSocket < 0) {
      socket_log_error("Error creating socket");
      return;
   }

   struct sockaddr_in address;
   memset(&address, 0, sizeof address);

   address.sin_family = AF_INET;
   address.sin_port = htons(RPC_CONTROL_PORT);
   if (!socket_address_for_name(&address, "0.0.0.0")) {
      socket_log_error("Could not convert address");
      socket_close(mServerListeningSocket);
      return;
   }
   if (bind(mServerListeningSocket, (struct sockaddr *) &address, sizeof address) < 0) {
      socket_log_error("Error binding socket");
      socket_close(mServerListeningSocket);
      return;
   }

   if (listen(mServerListeningSocket, 5) < 0) {
      socket_log_error("Error listening on socket");
      socket_close(mServerListeningSocket);
      return;
   }

   mSocketSet.insert(mServerListeningSocket);

   while (!done) {
      fd_set socketDescriptors;
      FD_ZERO(&socketDescriptors);
      maxFD = 0;
      for (std::set<int>::iterator it = mSocketSet.begin(); it != mSocketSet.end(); it++) {
         FD_SET(*it, &socketDescriptors);
         if (*it > maxFD) {
            maxFD = *it;
         }
      }
      result = select(maxFD+1, &socketDescriptors, NULL, NULL, NULL);

      for (std::set<int>::iterator it = mSocketSet.begin(); it != mSocketSet.end(); it++) {
         if (FD_ISSET(*it, &socketDescriptors)) {
            if (*it == mServerListeningSocket) {
               if (AcceptClientConnection(mServerListeningSocket) < 0) {
                  fprintf(stderr, "%s: AcceptClientConnection returned <0\n", __FUNCTION__);
               }
            } else {
               if (!HandleRequestFromClient(*it)) {
                  socket_close(*it);
                  mSocketSet.erase(it);
                  it = mSocketSet.begin();
               }
            }
         }
      }
   }
   if (result < 0) {
      socket_log_error("Error from select");
      socket_close(mServerListeningSocket);
      return;
   }
}

int NetServer::AcceptClientConnection(int listeningSocket)
{

   struct sockaddr_in clientAddress;
#ifdef _WIN32
   int clientAddressLen = sizeof clientAddress;
#else
   socklen_t clientAddressLen = sizeof clientAddress;
#endif // _WIN32
   int clientSocket;
   if ((clientSocket = accept(listeningSocket, (struct sockaddr*) &clientAddress, &clientAddressLen)) < 0) {
      socket_log_error("Error accepting on socket");
      return 0;
   }

   printf("%s: Accepted connection from %s:%d\n", __FUNCTION__, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
   mSocketSet.insert(clientSocket);
   return clientSocket;
}

bool NetServer::HandleRequestFromClient(int clientSocket)
{
   int32 bufferSize;

   if (recv(clientSocket, reinterpret_cast<char *>(&bufferSize), sizeof bufferSize, 0) != sizeof bufferSize) {
      return false;
   }
   uint8 *buffer = new uint8 [bufferSize];
   if (recv(clientSocket, reinterpret_cast<char *>(buffer), bufferSize, 0) != bufferSize) {
      delete buffer;
      return false;
   }

   Request rpcRequest;
   rpcRequest.ParseFromArray(buffer, bufferSize);
   delete buffer;

   Response rpcResponse;
   rpcResponse = ExecuteRPCRequest(rpcRequest);
   bufferSize = rpcResponse.ByteSize();
   if (send(clientSocket, reinterpret_cast<char *>(&bufferSize), sizeof bufferSize, 0) != sizeof bufferSize) {
      return false;
   }
   buffer = new uint8 [bufferSize];
   rpcResponse.SerializeToArray(buffer, bufferSize);
   if (send(clientSocket, reinterpret_cast<char*>(buffer), bufferSize, 0) != bufferSize) {
      delete buffer;
      return false;
   }

   delete buffer;
   return true;
}

void ConcreteDiscoveryService::QueryForService(awk::protobuf::RpcController* controller,
                                    const QueryForServiceRequest* request,
                                    QueryForServiceResponse* response,
                                    Closure* done)
{
   assert(mServer);
   awk::protobuf::Service *aService = mServer->ServiceRegisteredForName(request->service_name());

   if (aService) {
      response->set_registered(true);
      response->set_service_name(aService->GetDescriptor()->full_name());
   } else {
      response->set_registered(false);
   }
}
