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

#include "file_generator.h"
#include "helpers.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

#include <google/protobuf/io/printer.h>

#include "service_generator.h"

using namespace google::protobuf;

const char kThickSeparator[] =
  "// ===================================================================\n";
const char kThinSeparator[] =
  "// -------------------------------------------------------------------\n";

// Should we generate generic services for this file?
inline bool HasServices(const FileDescriptor *file) {
  return file->service_count() > 0 &&
         file->options().optimize_for() != FileOptions::LITE_RUNTIME;
}

// Do message classes in this file have descriptor and reflection methods?
inline bool HasDescriptorMethods(const FileDescriptor *file) {
  return file->options().optimize_for() != FileOptions::LITE_RUNTIME;
}

// Return the name of the AssignDescriptors() function for a given file.
string GlobalAssignDescriptorsName(const string& filename) {
  return "protobuf_rpc_AssignDesc_" + FilenameIdentifier(filename);
}




FileGenerator::FileGenerator(const FileDescriptor* file)
  : file_(file),
    service_generators_(
      new scoped_ptr<ServiceGenerator>[file->service_count()])
{
  // Construct ServiceGenerator instances for each service listed in this file
  for (int i = 0; i < file->service_count(); i++) {
    service_generators_[i].reset(
      new ServiceGenerator(file->service(i)));
  }
}

FileGenerator::~FileGenerator() {}

void FileGenerator::GenerateHeaderIncludes(io::Printer *printer) {
  // Place an include for the header that includes the pure virtual interfaces our
  // service instances must inherit from.
  if (HasServices(file_)) {
    printer->Print(
      "#include \"service.h\"\n");
  }
}

void FileGenerator::GenerateHeader(io::Printer* printer) {
  printer->Print("\n");
  printer->Print(kThickSeparator);
  printer->Print("\n");

  if (HasServices(file_)) {
    // Generate service definitions.
    for (int i = 0; i < file_->service_count(); i++) {
      if (i > 0) {
        printer->Print("\n");
        printer->Print(kThinSeparator);
        printer->Print("\n");
      }
      service_generators_[i]->GenerateDeclarations(printer);
    }

    printer->Print("\n");
    printer->Print(kThickSeparator);
    printer->Print("\n");
  }
}

void FileGenerator::GenerateSource(io::Printer* printer) {
  // Place a global pointer for the service descriptor for each service in our source file.
  if (HasServices(file_)) {
    for (int i = 0; i < file_->service_count(); i++) {
      printer->Print(
        "const ::google::protobuf::ServiceDescriptor* $name$_descriptor_ = NULL;\n",
        "name", file_->service(i)->name());
    }
  }

  // Define our externally-visible BuildDescriptors() function.  (For the lite
  // library, all this does is initialize default instances.)
  GenerateBuildDescriptors(printer);

  if (HasServices(file_)) {
    // Generate services.
    for (int i = 0; i < file_->service_count(); i++) {
      if (i == 0) printer->Print("\n");
      printer->Print(kThickSeparator);
      printer->Print("\n");
      service_generators_[i]->GenerateImplementation(printer);
    }
  }

  printer->Print(
    "\n");
}

void FileGenerator::GenerateBuildDescriptors(io::Printer* printer) {
  // AddDescriptors() is a file-level procedure which adds the encoded
  // FileDescriptorProto for this .proto file to the global DescriptorPool
  // for generated files (DescriptorPool::generated_pool()).  It always runs
  // at static initialization time, so all files will be registered before
  // main() starts.  This procedure also constructs default instances and
  // registers extensions.
  //
  // Its sibling, AssignDescriptors(), actually pulls the compiled
  // FileDescriptor from the DescriptorPool and uses it to populate all of
  // the global variables which store pointers to the descriptor objects.
  // It also constructs the reflection objects.  It is called the first time
  // anyone calls descriptor() or GetReflection() on one of the types defined
  // in the file.

  // In optimize_for = LITE_RUNTIME mode, we don't generate AssignDescriptors()
  // and we only use AddDescriptors() to allocate default instances.

  // Note that the $assigndescriptorsname$ variable value here is different from that used
  // in the built in generic services compiler. We're constructing a seperate initializer
  // here that is called seperately from the generated service subclass methods.
  if (HasDescriptorMethods(file_)) {
    printer->Print(
      "\n"
      "void $assigndescriptorsname$() {\n",
      "assigndescriptorsname", GlobalAssignDescriptorsName(file_->name()));
    printer->Indent();

    // Get the file's descriptor from the pool.
    printer->Print(
      "const ::google::protobuf::FileDescriptor* file =\n"
      "  ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(\n"
      "    \"$filename$\");\n"
      // Note that this GOOGLE_CHECK is necessary to prevent a warning about "file"
      // being unused when compiling an empty .proto file.
      "GOOGLE_CHECK(file != NULL);\n",
      "filename", file_->name());

    if (HasServices(file_)) {
      for (int i = 0; i < file_->service_count(); i++) {
        service_generators_[i]->GenerateDescriptorInitializer(printer, i);
      }
    }

    printer->Outdent();
    printer->Print(
      "}\n"
      "\n");

    // ---------------------------------------------------------------

    // protobuf_AssignDescriptorsOnce():  The first time it is called, calls
    // AssignDescriptors().  All later times, waits for the first call to
    // complete and then returns.
   printer->Print(
      "namespace {\n"
      "\n"
      "GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_rpc_AssignDescriptors_once_);\n"
      "inline void protobuf_rpc_AssignDescriptorsOnce() {\n"
      "  ::google::protobuf::GoogleOnceInit(&protobuf_rpc_AssignDescriptors_once_,\n"
      "                 &$assigndescriptorsname$);\n"
      "}\n"
      "\n",
      "assigndescriptorsname", GlobalAssignDescriptorsName(file_->name()));

    printer->Print(
      "}  // namespace\n");
  }

  // -----------------------------------------------------------------
}


