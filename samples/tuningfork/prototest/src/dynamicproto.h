#pragma once

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

// Utility functions for dealing with protos loaded from file rather than compiled with protoc
namespace dynamicproto {

using namespace google::protobuf;

void init(const std::vector<std::string>& includeDirs);

std::vector<const FileDescriptor*> fileDescriptors(const std::string& filename);

void extensionsOf(const FileDescriptor* extDesc, const std::string& name, std::vector<const FieldDescriptor*>& exts);

std::vector<const EnumDescriptor*> enums(const FileDescriptor* extDesc);

void print(const EnumDescriptor* e, std::ostream& o);

Message* newMessage(const FileDescriptor* base, const std::string& name);

}
