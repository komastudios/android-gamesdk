
#include "dynamicproto.h"

#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>

#include <iostream>
#include <memory>
#include <exception>

using namespace google::protobuf::compiler;
using namespace google::protobuf;

namespace {

class ErrorCollector : public MultiFileErrorCollector {
 public:
  virtual void AddError(const std::string& filename, int line, int column,
                        const std::string& message) {
    std::cerr << "Error in: " << filename << " line: " << line << " column: " << column << " message: " << message << std::endl;
  }
};

DiskSourceTree sourceTree;
ErrorCollector error_collector;
std::unique_ptr<Importer> importer;
DynamicMessageFactory factory; // This needs to be kept around

}

void warnNotInitialized() {
  std::cerr << "Warning: dynamicproto has not been initalized" << std::endl;
}
namespace dynamicproto {

void init(const std::vector<std::string>& includePaths) {
  for(auto& path: includePaths) {
    sourceTree.MapPath("", path);
  }
  importer = std::unique_ptr<Importer>(new Importer(&sourceTree, &error_collector)); // No make_unique on C++11
}

Message* newMessageUsingProto(const std::string& filename, const std::string& basefilename, const std::string& messagename) {
  if(!importer) {
    warnNotInitialized();
    return nullptr;
  }
  const FileDescriptor* base = importer->Import(basefilename);
  if(base) {
    // Loading this gives us the name of the extensions
    const FileDescriptor* fdesc = importer->Import(filename);
    if(fdesc) {
      const Descriptor* desc = base->FindMessageTypeByName(messagename);
      if(desc) {
        auto p = factory.GetPrototype(desc);
        if(p) {
          Message* t = p->New();
          return t;
        }
        //      std::cerr << "Loaded " << t->DebugString() << std::endl;
      }
    }
  }
  std::cerr << "Couldn't find " << messagename << " in " << filename << " or " << basefilename << std::endl;
  return nullptr;
}
void fileDescriptorDependencies(const FileDescriptor* fdesc, std::vector<const FileDescriptor*>& result) {
  if(fdesc) {
    size_t n = fdesc->dependency_count();
    for(size_t i=0; i<n; ++i) {
      fileDescriptorDependencies(fdesc->dependency(i), result);
    }
    result.push_back(fdesc);
  }
}

// Get a FileDescriptor for the file and all its dependencies, the latter of which appear at the beginning of the vector.
std::vector<const FileDescriptor*> fileDescriptors(const std::string& filename) {
  std::vector<const FileDescriptor*> result;
  if(importer) {
    const FileDescriptor* fdesc = importer->Import(filename);
    fileDescriptorDependencies(fdesc, result);
  }
  else
    warnNotInitialized();
  return result;
}

void extensionsOf(const FileDescriptor* desc, const std::string& name, std::vector<const FieldDescriptor*>& result) {
  // TODO: look in all messages that may contain inner extensions
  if(desc) {
      int n = desc->extension_count();
      for(int i=0;i<n;++i) {
        auto e = desc->extension(i);
        if(e->containing_type()->full_name()==name)
          result.push_back(e);
      }
      n = desc->dependency_count();
      for(int i=0;i<n;++i) {
        extensionsOf(desc->dependency(i), name, result);
      }
  }
}

std::vector<const EnumDescriptor*> enums(const FileDescriptor* extDesc) {
  std::vector<const EnumDescriptor*> result;
  if(extDesc) {
      int n = extDesc->enum_type_count();
      //      std::cerr << n << "\n";
      for(int i=0;i<n;++i) {
        auto e = extDesc->enum_type(i);
        result.push_back(e);
      }
  }
  return result;
}

void print(const EnumDescriptor* e, std::ostream& o) {
  std::cerr << "enum " << e->name() << " { ";
  int n = e->value_count();
  bool notFirst = false;
  for(int i=0; i<n; ++i) {
    if(notFirst)
      o << ", ";
    else
      notFirst=true;
    o << e->value(i)->name() << "=" << (i+1);
  }
  std::cerr << "}";
}

Message* newMessage(const FileDescriptor* base, const std::string& name) {
  auto desc = base->FindMessageTypeByName(name);
  if(desc) {
    auto p = factory.GetPrototype(desc);
    if(p)
      return p->New();
  }
  return nullptr;
}

}
