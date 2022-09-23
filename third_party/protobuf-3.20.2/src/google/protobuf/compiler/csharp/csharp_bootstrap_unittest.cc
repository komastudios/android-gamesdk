// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// https://developers.google.com/protocol-buffers/
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

// This test insures that
// csharp/src/Google.Protobuf/Reflection/Descriptor.cs  match exactly
// what would be generated by the protocol compiler.  The file is not
// generated automatically at build time.
//
// If this test fails, run the script
// "generate_descriptor_proto.sh" and add the changed files under
// csharp/src/ to your changelist.

#include <map>

#include <google/protobuf/compiler/csharp/csharp_generator.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/stubs/map_util.h>
#include <google/protobuf/stubs/stl_util.h>
#include <google/protobuf/stubs/strutil.h>
#include <google/protobuf/stubs/substitute.h>

#include <google/protobuf/testing/file.h>
#include <google/protobuf/testing/file.h>
#include <google/protobuf/testing/googletest.h>
#include <gtest/gtest.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace csharp {

namespace {

class MockErrorCollector : public MultiFileErrorCollector {
 public:
  MockErrorCollector() {}
  ~MockErrorCollector() {}

  std::string text_;

  // implements ErrorCollector ---------------------------------------
  void AddError(const std::string& filename, int line, int column,
                const std::string& message) {
    strings::SubstituteAndAppend(&text_, "$0:$1:$2: $3\n",
                                 filename, line, column, message);
  }
};

class MockGeneratorContext : public GeneratorContext {
 public:
  void ExpectFileMatches(const std::string& virtual_filename,
                         const std::string& physical_filename) {
    auto it = files_.find(virtual_filename);
    ASSERT_TRUE(it != files_.end())
      << "Generator failed to generate file: " << virtual_filename;
    std::string expected_contents = *it->second;

    std::string actual_contents;
    GOOGLE_CHECK_OK(
        File::GetContentsAsText(TestSourceDir() + "/" + physical_filename,
                          &actual_contents, true))
        << "Unable to get " << physical_filename;
    EXPECT_TRUE(actual_contents == expected_contents)
      << physical_filename << " needs to be regenerated.  Please run "
         "generate_descriptor_proto.sh. Then add this file "
         "to your CL.";
  }

  // implements GeneratorContext --------------------------------------

  virtual io::ZeroCopyOutputStream* Open(const std::string& filename) {
    auto& map_slot = files_[filename];
    map_slot.reset(new std::string);
    return new io::StringOutputStream(map_slot.get());
  }

 private:
  std::map<std::string, std::unique_ptr<std::string>> files_;
};

class GenerateAndTest {
 public:
  GenerateAndTest() {}
  void Run(const FileDescriptor* proto_file, std::string file1,
           std::string file2) {
    ASSERT_TRUE(proto_file != NULL) << TestSourceDir();
    ASSERT_TRUE(generator_.Generate(proto_file, parameter_,
                                    &context_, &error_));
    context_.ExpectFileMatches(file1, file2);
  }
  void SetParameter(std::string parameter) {
    parameter_ = parameter;
  }

 private:
  Generator generator_;
  MockGeneratorContext context_;
  std::string error_;
  std::string parameter_;
};

TEST(CsharpBootstrapTest, GeneratedCsharpDescriptorMatches) {
  // Skip this whole test if the csharp directory doesn't exist (i.e., a C++11
  // only distribution).
  std::string descriptor_file_name =
      "../csharp/src/Google.Protobuf/Reflection/Descriptor.cs";
  if (!File::Exists(TestSourceDir() + "/" + descriptor_file_name)) {
    return;
  }

  MockErrorCollector error_collector;
  DiskSourceTree source_tree;
  Importer importer(&source_tree, &error_collector);
  GenerateAndTest generate_test;

  generate_test.SetParameter("base_namespace=Google.Protobuf");
  source_tree.MapPath("", TestSourceDir());
  generate_test.Run(importer.Import("google/protobuf/descriptor.proto"),
                    "Reflection/Descriptor.cs",
                    "../csharp/src/Google.Protobuf/Reflection/Descriptor.cs");
  generate_test.Run(importer.Import("google/protobuf/any.proto"),
                    "WellKnownTypes/Any.cs",
                    "../csharp/src/Google.Protobuf/WellKnownTypes/Any.cs");
  generate_test.Run(importer.Import("google/protobuf/api.proto"),
                    "WellKnownTypes/Api.cs",
                    "../csharp/src/Google.Protobuf/WellKnownTypes/Api.cs");
  generate_test.Run(importer.Import("google/protobuf/duration.proto"),
                    "WellKnownTypes/Duration.cs",
                    "../csharp/src/Google.Protobuf/WellKnownTypes/Duration.cs");
  generate_test.Run(importer.Import("google/protobuf/empty.proto"),
                    "WellKnownTypes/Empty.cs",
                    "../csharp/src/Google.Protobuf/WellKnownTypes/Empty.cs");
  generate_test.Run(importer.Import("google/protobuf/field_mask.proto"),
                    "WellKnownTypes/FieldMask.cs",
                    "../csharp/src/Google.Protobuf/WellKnownTypes/FieldMask.cs");
  generate_test.Run(importer.Import("google/protobuf/source_context.proto"),
                    "WellKnownTypes/SourceContext.cs",
                    "../csharp/src/Google.Protobuf/WellKnownTypes/SourceContext.cs");
  generate_test.Run(importer.Import("google/protobuf/struct.proto"),
                    "WellKnownTypes/Struct.cs",
                    "../csharp/src/Google.Protobuf/WellKnownTypes/Struct.cs");
  generate_test.Run(importer.Import("google/protobuf/timestamp.proto"),
                    "WellKnownTypes/Timestamp.cs",
                    "../csharp/src/Google.Protobuf/WellKnownTypes/Timestamp.cs");
  generate_test.Run(importer.Import("google/protobuf/type.proto"),
                    "WellKnownTypes/Type.cs",
                    "../csharp/src/Google.Protobuf/WellKnownTypes/Type.cs");
  generate_test.Run(importer.Import("google/protobuf/wrappers.proto"),
                    "WellKnownTypes/Wrappers.cs",
                    "../csharp/src/Google.Protobuf/WellKnownTypes/Wrappers.cs");

  generate_test.SetParameter("");
  source_tree.MapPath("", TestSourceDir() + "/../conformance");
  generate_test.Run(importer.Import("conformance.proto"),
                    "Conformance.cs",
                    "../csharp/src/Google.Protobuf.Conformance/Conformance.cs");

  EXPECT_EQ("", error_collector.text_);
}

}  // namespace

}  // namespace csharp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
