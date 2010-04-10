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

#include "protobuf_rpc_socket.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif // _WIN32


int awk::protobuf::jerpc::socket_initialize()
{
#ifdef _WIN32
   WORD wVersionRequested;
   WSADATA wsaData;
   int err;

   wVersionRequested = MAKEWORD(2, 2);

   err = WSAStartup( wVersionRequested, &wsaData );
   if (err != 0) {
       /*
        * Tell the user that we could not find a usable
        * WinSock DLL.
        */
       return 1;
   }

   /*
    * Confirm that the WinSock DLL supports 2.2.
    * Note that if the DLL supports versions greater
    * than 2.2 in addition to 2.2, it will still return
    * 2.2 in wVersion since that is the version we
    * requested.
    */

   if (LOBYTE(wsaData.wVersion) != 2 ||
       HIBYTE(wsaData.wVersion) != 2) {
       /*
        * Tell the user that we could not find a usable
        * WinSock DLL.
        */
       WSACleanup( );
       return 1;
   }

   // The WinSock DLL is acceptable. Proceed.
#endif // _WIN32
   return 0;
}

int awk::protobuf::jerpc::socket_close(int socket)
{
#ifdef _WIN32
   return closesocket(socket);
#else
   return close(socket);
#endif // _WIN32
}

void awk::protobuf::jerpc::socket_log_error(const char *fmt, ...)
{
   va_list ap;
   va_start (ap, fmt);
   vfprintf (stderr, fmt, ap);
   va_end (ap);

#ifdef _WIN32
   fprintf(stderr, " WSAGetLastErr = %d\n", WSAGetLastError());
#else
   fprintf(stderr, " err = %d\n", errno);
#endif // _WIN32
}

bool awk::protobuf::jerpc::socket_address_for_name(struct sockaddr_in *address, const char *name)
{
#ifdef _WIN32
   unsigned long addr;
   if ((addr = inet_addr(name)) == INADDR_NONE) {
      return false;
   }
   address->sin_addr.s_addr = addr;
#else
#ifdef HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
   address->sin_len = sizeof *address;
#endif
   if (inet_aton(name, &address->sin_addr) == 0)  {
      return false;
   }
#endif
   return true;
}
