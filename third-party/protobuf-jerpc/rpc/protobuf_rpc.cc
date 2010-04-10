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

#include <cstdio>
#include "protobuf_rpc.h"
#include "protobuf_rpc_server.h"
#include "protobuf_rpc_socket.h"
#include "protorpc.pb.h"
#include "google/protobuf/descriptor.h"

using namespace google::protobuf;
using namespace awk::protobuf::jerpc;

SocketRpcController::SocketRpcController() : RpcController(), errorReasonCode(NO_ERROR_OCCURRED)
{
}

SocketRpcController::~SocketRpcController()
{
}

// Client-side methods ---------------------------------------------
// These calls may be made from the client side only.  Their results
// are undefined on the server side (may crash).

void SocketRpcController::Reset()
{
   errorReasonCode = NO_ERROR_OCCURRED;
   errorReasonString = "";
}

bool SocketRpcController::Failed() const
{
   return errorReasonCode != NO_ERROR_OCCURRED;
}


string SocketRpcController::ErrorText() const
{
   return errorReasonString;
}


void SocketRpcController::StartCancel()
{
   printf("%s\n", __FUNCTION__);
}


// Server-side methods ---------------------------------------------
// These calls may be made from the server side only.  Their results
// are undefined on the client side (may crash).

void SocketRpcController::SetFailed(const string& reason)
{
   errorReasonString = reason;
   errorReasonCode = RPC_FAILED;
}

bool SocketRpcController::IsCanceled() const
{
   printf("%s\n", __FUNCTION__);
   return false;
}


void SocketRpcController::NotifyOnCancel(Closure* callback)
{
   printf("%s - closure = %p\n", __FUNCTION__, callback);
}


