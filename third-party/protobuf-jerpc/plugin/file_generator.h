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

#ifndef __FILE_GENERATOR_H__
#define __FILE_GENERATOR_H__

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/code_generator.h>

#include <google/protobuf/stubs/common.h>

namespace google {
namespace protobuf {
  class FileDescriptor;        // descriptor.h
  class ServiceDescriptor;     // descriptor.h
  namespace io {
    class Printer;             // printer.h
  }
}
}

class ServiceGenerator;

class FileGenerator {
 public:
  // See generator.cc for the meaning of dllexport_decl.
  explicit FileGenerator(const google::protobuf::FileDescriptor* file);
  ~FileGenerator();

  void GenerateHeaderIncludes(google::protobuf::io::Printer* printer);
  void GenerateHeader(google::protobuf::io::Printer* printer);
  void GenerateSource(google::protobuf::io::Printer* printer);

 private:
  // Generate the BuildDescriptors() procedure, which builds all descriptors
  // for types defined in the file.
  void GenerateBuildDescriptors(google::protobuf::io::Printer* printer);

  const google::protobuf::FileDescriptor* file_;

  google::protobuf::internal::scoped_array<google::protobuf::internal::scoped_ptr<ServiceGenerator> > service_generators_;

  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(FileGenerator);
};


#endif // __FILE_GENERATOR_H__
