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

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/code_generator.h>

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/printer.h>

#include <google/protobuf/stubs/common.h>

#include <string>

#include "plugin.h"
#include "helpers.h"
#include "file_generator.h"
#include "service_generator.h"

using namespace google::protobuf;


int main(int argc, char* argv[]) {
  RPCCodeGenerator generator;
  return compiler::PluginMain(argc, argv, &generator);
}


bool
RPCCodeGenerator::Generate(const FileDescriptor * file,
         const std::string & parameter,
         compiler::OutputDirectory * output_directory,
         std::string * error) const
{
  std::string basename = StripProto(file->name());
  basename.append(".pb");

  FileGenerator file_generator(file);

  // Generate header includes.
  {
    internal::scoped_ptr<io::ZeroCopyOutputStream> output(
      output_directory->OpenForInsert(basename + ".h", "includes"));
    io::Printer printer(output.get(), '$');
    file_generator.GenerateHeaderIncludes(&printer);
  }

  // Generate header.
  {
    internal::scoped_ptr<io::ZeroCopyOutputStream> output(
      output_directory->OpenForInsert(basename + ".h", "namespace_scope"));
    io::Printer printer(output.get(), '$');
    file_generator.GenerateHeader(&printer);
  }

  // Generate cc file.
  {
    internal::scoped_ptr<io::ZeroCopyOutputStream> output(
      output_directory->OpenForInsert(basename + ".cc", "namespace_scope"));
    io::Printer printer(output.get(), '$');
    file_generator.GenerateSource(&printer);
  }

   return true;
}

