// Protocol Buffers - Google's data interchange format
// Copyright 2010 Andrew Kimpton & Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
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
//  Based on original Protocol Buffers design by
//  Kenton Varda, Sanjay Ghemawat, Jeff Dean, and others.

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/code_generator.h>

#include <google/protobuf/stubs/common.h>

#include <map>

namespace google {
namespace protobuf {
  class FileDescriptor;        // descriptor.h
  class ServiceDescriptor;     // descriptor.h
  namespace io {
    class Printer;             // printer.h
  }
}
}

class RPCCodeGenerator : public google::protobuf::compiler::CodeGenerator {
   public:
      RPCCodeGenerator() {};
      virtual ~RPCCodeGenerator() {};
      virtual bool Generate(const google::protobuf::FileDescriptor * file, const std::string & parameter, google::protobuf::compiler::OutputDirectory * output_directory, std::string * error) const;
};


#endif // __PLUGIN_H__
