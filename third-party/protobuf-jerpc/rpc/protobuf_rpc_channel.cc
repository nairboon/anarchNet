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

#include "protobuf_rpc.h"
#include "protobuf_rpc_channel.h"
#include "protobuf_rpc_socket.h"
#include "protorpc.pb.h"
#include "google/protobuf/descriptor.h"

using namespace google::protobuf;
using namespace awk::protobuf::jerpc;

SocketRpcChannel::SocketRpcChannel() : RpcChannel(), mSocket(0)
{
}

SocketRpcChannel::~SocketRpcChannel()
{
   if (mSocket) {
      socket_close(mSocket);
   }
}


bool SocketRpcChannel::QueryForService(awk::protobuf::Service* aService)
{
   bool isRegistered = false;
   QueryForServiceRequest request;
   QueryForServiceResponse response;
   SocketRpcController aController;
   DiscoveryService *discoveryService = new DiscoveryService::Stub(this);

   // Set up the request.
   request.set_service_name(aService->GetDescriptor()->full_name());

   // Execute the RPC.
   discoveryService->QueryForService(&aController, &request, &response, NULL /*NewCallback(&Done)*/);

   if (!aController.Failed()) {
      // Print the result
      isRegistered = response.registered();
   }
   delete discoveryService;

   return isRegistered;
}

bool SocketRpcChannel::QueryForMethod(awk::protobuf::Service* aService, const google::protobuf::MethodDescriptor* method)
{
   bool isAvailable = false;

   return isAvailable;
}


bool SocketRpcChannel::Connect(const char *serverName, int portNumber)
{
   mSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (mSocket < 0) {
      socket_log_error("Could not create socket");
      return false;
   }

   struct sockaddr_in serverAddr;
   memset(&serverAddr, 0, sizeof serverAddr);
   serverAddr.sin_family = AF_INET;
   if (!socket_address_for_name(&serverAddr, serverName)) {
      socket_log_error("Could not convert address");
      socket_close(mSocket);
      mSocket = -1;
      return false;
   }
   serverAddr.sin_port = htons(portNumber);
   if (connect(mSocket, (struct sockaddr*) &serverAddr, sizeof serverAddr) < 0) {
      socket_log_error("%s: Could not connect socket", __FUNCTION__);
      socket_close(mSocket);
      mSocket = 0;

      return false;
   }
   return true;
}


void SocketRpcChannel::CallMethod(const MethodDescriptor* method,
                                  awk::protobuf::RpcController* controller,
                                  const Message* request,
                                  Message* response,
                                  Closure* done)
{
   Request anRPCRequest;
   Response anRPCResponse;
   std::string requestRawString;
   std::string RPCResponseString;

   request->SerializeToString(&requestRawString);
   anRPCRequest.set_request_proto(requestRawString);
   anRPCRequest.set_service_name(method->service()->full_name());
   anRPCRequest.set_method_name(method->name());

   if (mSocket > -1) {
      uint32 bufferSize = anRPCRequest.ByteSize();
      send(mSocket, reinterpret_cast<char*>(&bufferSize), sizeof bufferSize, 0);
      uint8 *buffer = new uint8 [bufferSize];
      anRPCRequest.SerializeToArray(buffer, bufferSize);
      send(mSocket, reinterpret_cast<char*>(buffer), bufferSize, 0);
      delete buffer;

      recv(mSocket, reinterpret_cast<char*>(&bufferSize), sizeof bufferSize, 0);
      buffer = new uint8 [bufferSize];
      recv(mSocket, reinterpret_cast<char*>(buffer), bufferSize, 0);
      anRPCResponse.ParseFromArray(buffer, bufferSize);
      delete buffer;
   } else {
      fprintf(stderr, "%s: No Server or Socket set !\n", __FUNCTION__);
   }


   if (anRPCResponse.has_error_reason() && anRPCResponse.error_reason() != NO_ERROR_OCCURRED) {
      controller->SetFailed(anRPCResponse.error());
   } else if (anRPCResponse.response_proto().size() > 0) {
      // Build a response from the response_proto returned by the RPC.
      response->ParseFromString(anRPCResponse.response_proto());
   }

   if (done) {
      done->Run();
   }
}


