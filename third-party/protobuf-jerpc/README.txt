Protocol Buffers - Just Enough RPC
Copyright 2010 Andrew Kimpton
http://code.google.com/p/protobuf-jerpc

C++ Installation - Unix
=======================

Prerequisite - a working installation of the Protocol Buffers project for your platform

To build and install the C++ Just Enough RPC for Protocol Buffers
execute the following:

  $ cd plugin
  $ make
  $ cd rpc
  $ make

To test the RPC implementation launch test_server from the command line and
then launch test_client <server address> from a second command line.

Notes on Implemention
=====================

The plugin outputs essentially the same base classes (Service, RpcController & RpcChannel) as
the generic service mechanism included in the default protoc compiler. A different namespace is
used (awk::protobuf) to keep the definitions seperate (the namespace is configurable at
compilation time in the plugin).

To use the plugins output in your own services you need to include plugin/service.h where
appropriate and compile plugin/service.cc into your own applications. You should also set the
cc_generic_services = false option in your .proto files (see rpc/test_service.proto for an
example).

The concrete implementations of RpcChannel and RpcController (respectively SocketRpcChannel
socket RpcController), and the NetServer class implement a trivial RPC server where the
request  & response for a Service method is shipped over a socket connection wrapped in a
protocol buffer that includes an identifier for the Service and Method details. The buffer
is serialized and sent as a 'length then data' chunk over the socket.

NetServer also provides an implementation of the DiscoveryService so that a remote client can
issue an RPC to get a list of the services provided on a server. RpcChannel has methods that
call through to the DiscoveryService.

