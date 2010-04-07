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

#ifndef __PROTOBUF_RPC_CHANNEL_H__
#define __PROTOBUF_RPC_CHANNEL_H__ 1

#include <string>
#include <map>
#include "protorpc.pb.h"

namespace awk { namespace protobuf { namespace jerpc {

class SocketRpcChannel : public awk::protobuf::RpcChannel {
public:
   SocketRpcChannel();

   virtual ~SocketRpcChannel();

   virtual bool Connect(const char *serverName, int portNumber);

   virtual bool QueryForService(awk::protobuf::Service* aService);
   virtual bool QueryForMethod(awk::protobuf::Service* aService, const google::protobuf::MethodDescriptor* method);

   // Call the given method of the remote service.  The signature of this
   // procedure looks the same as Service::CallMethod(), but the requirements
   // are less strict in one important way:  the request and response objects
   // need not be of any specific class as long as their descriptors are
   // method->input_type() and method->output_type().
   virtual void CallMethod(const google::protobuf::MethodDescriptor* method,
      awk::protobuf::RpcController* controller,
      const google::protobuf::Message* request,
      google::protobuf::Message* response,
      google::protobuf::Closure* done);

protected:
   int mSocket;
};

} } } // namespace awk::protobuf::jerpc

#endif // __PROTOBUF_RPC_H__
