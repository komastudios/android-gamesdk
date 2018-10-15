#include "dynamicproto.h"

#include "proto/tuningfork.pb.h"

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <iostream>
#include <fstream>
#include <numeric>
#include <functional>
#include <algorithm>

using namespace google::protobuf;
using namespace google::protobuf::io;

using ::com::google::tuningfork::Settings;
using ::com::google::tuningfork::Settings_Histogram;

namespace {

void usage() {
  std::cerr << "Usage: sztool <your_tf_proto_name> <your_settings>\n";
}

constexpr size_t ERROR = -1;

size_t fieldSize(const FieldDescriptor* fd) {
  switch(fd->type()) {
    case FieldDescriptor::TYPE_BOOL:
      std::cerr << fd->name() << " " << fd->type_name() << std::endl;
      // Do we want this?
      return 2;
    case FieldDescriptor::TYPE_ENUM:
      std::cerr << fd->name() << " ";
      dynamicproto::print(fd->enum_type(), std::cerr);
      std::cerr << std::endl;
      return fd->enum_type()->value_count() + 1; // NB each field is optional, so their is another possibility, with the value unknown
    default:
      return ERROR;
    }
}

}

int main(int argc, char *argv[]) {

  if(argc<3) {
    usage();
    return 1;
  }
  dynamicproto::init({".", "proto"});
  auto fdescs = dynamicproto::fileDescriptors(argv[1]);
  if(fdescs.size()<2) {
    std::cerr << "The proto file has errors" << std::endl;
    return -2;
  }

  // Get the annotation extensions and count the combinations
  std::vector<const FieldDescriptor*> annotations;
  dynamicproto::extensionsOf(fdescs.back(), "com.google.tuningfork.Annotation", annotations);
  auto enums = dynamicproto::enums(fdescs.back());
  std::vector<size_t> annotationSize(annotations.size());
  std::transform(annotations.begin(), annotations.end(), annotationSize.begin(), fieldSize);
  for(auto a: annotationSize) {
    if(a==ERROR) {
      std::cerr << "Bad annotation (only bool and enum are supported)" << std::endl;
      return 2;
    }
  }
  size_t nAnnotationCombinations = std::accumulate(annotationSize.begin(), annotationSize.end(), 1, std::multiplies<size_t>());
  std::cerr << "Total number of annotation combinations = " << nAnnotationCombinations << std::endl;

  // Get the settings
  std::string settingsFileName = argv[2];
  ifstream settingsFile(settingsFileName);
  IstreamInputStream settingsStream(&settingsFile);
  if(!settingsFile.good()) {
    std::cerr << "Bad file: " << argv[2] << std::endl;
    return 2;
  }
  Settings settings;
  TextFormat::Parse(&settingsStream, &settings);
  std::cerr << "Read settings file " << settingsFileName << " :\n";
  std::cerr << settings.DebugString() << "\n";
  //  size_t maxInstrumentationKeys = settings.aggregation_strategy().max_instrumentation_keys();

  const size_t BucketSizeBytes = sizeof(uint32_t);
  // For each histogram, calculate the size in bytes
  int n = settings.histograms_size();
  size_t totalSize = 0;
  for(int i=0;i<n;++i) {
    const Settings_Histogram& h = settings.histograms(i);
    size_t sz = h.n_buckets()*nAnnotationCombinations*BucketSizeBytes;
    std::cerr << "Histogram for ikey " << h.instrument_key() << " size = " << h.n_buckets()*nAnnotationCombinations*BucketSizeBytes << " bytes\n";
    totalSize += sz;
  }
  std::cerr << "Total packet size: "<< totalSize <<" bytes\n";
  return 0;
}
