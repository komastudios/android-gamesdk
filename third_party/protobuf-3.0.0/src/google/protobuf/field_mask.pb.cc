// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/protobuf/field_mask.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include <google/protobuf/field_mask.pb.h>

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace google {
namespace protobuf {

namespace {

const ::google::protobuf::Descriptor* FieldMask_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  FieldMask_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_google_2fprotobuf_2ffield_5fmask_2eproto() GOOGLE_ATTRIBUTE_COLD;
void protobuf_AssignDesc_google_2fprotobuf_2ffield_5fmask_2eproto() {
  protobuf_AddDesc_google_2fprotobuf_2ffield_5fmask_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "google/protobuf/field_mask.proto");
  GOOGLE_CHECK(file != NULL);
  FieldMask_descriptor_ = file->message_type(0);
  static const int FieldMask_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FieldMask, paths_),
  };
  FieldMask_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      FieldMask_descriptor_,
      FieldMask::default_instance_,
      FieldMask_offsets_,
      -1,
      -1,
      -1,
      sizeof(FieldMask),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FieldMask, _internal_metadata_),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FieldMask, _is_default_instance_));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_google_2fprotobuf_2ffield_5fmask_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      FieldMask_descriptor_, &FieldMask::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_google_2fprotobuf_2ffield_5fmask_2eproto() {
  delete FieldMask::default_instance_;
  delete FieldMask_reflection_;
}

void protobuf_AddDesc_google_2fprotobuf_2ffield_5fmask_2eproto() GOOGLE_ATTRIBUTE_COLD;
void protobuf_AddDesc_google_2fprotobuf_2ffield_5fmask_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n google/protobuf/field_mask.proto\022\017goog"
    "le.protobuf\"\032\n\tFieldMask\022\r\n\005paths\030\001 \003(\tB"
    "Q\n\023com.google.protobufB\016FieldMaskProtoP\001"
    "\240\001\001\242\002\003GPB\252\002\036Google.Protobuf.WellKnownTyp"
    "esb\006proto3", 170);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "google/protobuf/field_mask.proto", &protobuf_RegisterTypes);
  FieldMask::default_instance_ = new FieldMask();
  FieldMask::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_google_2fprotobuf_2ffield_5fmask_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_google_2fprotobuf_2ffield_5fmask_2eproto {
  StaticDescriptorInitializer_google_2fprotobuf_2ffield_5fmask_2eproto() {
    protobuf_AddDesc_google_2fprotobuf_2ffield_5fmask_2eproto();
  }
} static_descriptor_initializer_google_2fprotobuf_2ffield_5fmask_2eproto_;

// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int FieldMask::kPathsFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

FieldMask::FieldMask()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:google.protobuf.FieldMask)
}

void FieldMask::InitAsDefaultInstance() {
  _is_default_instance_ = true;
}

FieldMask::FieldMask(const FieldMask& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:google.protobuf.FieldMask)
}

void FieldMask::SharedCtor() {
    _is_default_instance_ = false;
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
}

FieldMask::~FieldMask() {
  // @@protoc_insertion_point(destructor:google.protobuf.FieldMask)
  SharedDtor();
}

void FieldMask::SharedDtor() {
  if (this != default_instance_) {
  }
}

void FieldMask::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* FieldMask::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return FieldMask_descriptor_;
}

const FieldMask& FieldMask::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_google_2fprotobuf_2ffield_5fmask_2eproto();
  return *default_instance_;
}

FieldMask* FieldMask::default_instance_ = NULL;

FieldMask* FieldMask::New(::google::protobuf::Arena* arena) const {
  FieldMask* n = new FieldMask;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void FieldMask::Clear() {
// @@protoc_insertion_point(message_clear_start:google.protobuf.FieldMask)
  paths_.Clear();
}

bool FieldMask::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:google.protobuf.FieldMask)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated string paths = 1;
      case 1: {
        if (tag == 10) {
         parse_paths:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_paths()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->paths(this->paths_size() - 1).data(),
            this->paths(this->paths_size() - 1).length(),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "google.protobuf.FieldMask.paths"));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(10)) goto parse_paths;
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:google.protobuf.FieldMask)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:google.protobuf.FieldMask)
  return false;
#undef DO_
}

void FieldMask::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:google.protobuf.FieldMask)
  // repeated string paths = 1;
  for (int i = 0; i < this->paths_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->paths(i).data(), this->paths(i).length(),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "google.protobuf.FieldMask.paths");
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->paths(i), output);
  }

  // @@protoc_insertion_point(serialize_end:google.protobuf.FieldMask)
}

::google::protobuf::uint8* FieldMask::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:google.protobuf.FieldMask)
  // repeated string paths = 1;
  for (int i = 0; i < this->paths_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->paths(i).data(), this->paths(i).length(),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "google.protobuf.FieldMask.paths");
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(1, this->paths(i), target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:google.protobuf.FieldMask)
  return target;
}

int FieldMask::ByteSize() const {
// @@protoc_insertion_point(message_byte_size_start:google.protobuf.FieldMask)
  int total_size = 0;

  // repeated string paths = 1;
  total_size += 1 * this->paths_size();
  for (int i = 0; i < this->paths_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->paths(i));
  }

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void FieldMask::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:google.protobuf.FieldMask)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  const FieldMask* source = 
      ::google::protobuf::internal::DynamicCastToGenerated<const FieldMask>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:google.protobuf.FieldMask)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:google.protobuf.FieldMask)
    MergeFrom(*source);
  }
}

void FieldMask::MergeFrom(const FieldMask& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:google.protobuf.FieldMask)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  paths_.MergeFrom(from.paths_);
}

void FieldMask::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:google.protobuf.FieldMask)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void FieldMask::CopyFrom(const FieldMask& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:google.protobuf.FieldMask)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool FieldMask::IsInitialized() const {

  return true;
}

void FieldMask::Swap(FieldMask* other) {
  if (other == this) return;
  InternalSwap(other);
}
void FieldMask::InternalSwap(FieldMask* other) {
  paths_.UnsafeArenaSwap(&other->paths_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata FieldMask::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = FieldMask_descriptor_;
  metadata.reflection = FieldMask_reflection_;
  return metadata;
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// FieldMask

// repeated string paths = 1;
int FieldMask::paths_size() const {
  return paths_.size();
}
void FieldMask::clear_paths() {
  paths_.Clear();
}
 const ::std::string& FieldMask::paths(int index) const {
  // @@protoc_insertion_point(field_get:google.protobuf.FieldMask.paths)
  return paths_.Get(index);
}
 ::std::string* FieldMask::mutable_paths(int index) {
  // @@protoc_insertion_point(field_mutable:google.protobuf.FieldMask.paths)
  return paths_.Mutable(index);
}
 void FieldMask::set_paths(int index, const ::std::string& value) {
  // @@protoc_insertion_point(field_set:google.protobuf.FieldMask.paths)
  paths_.Mutable(index)->assign(value);
}
 void FieldMask::set_paths(int index, const char* value) {
  paths_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:google.protobuf.FieldMask.paths)
}
 void FieldMask::set_paths(int index, const char* value, size_t size) {
  paths_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:google.protobuf.FieldMask.paths)
}
 ::std::string* FieldMask::add_paths() {
  // @@protoc_insertion_point(field_add_mutable:google.protobuf.FieldMask.paths)
  return paths_.Add();
}
 void FieldMask::add_paths(const ::std::string& value) {
  paths_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:google.protobuf.FieldMask.paths)
}
 void FieldMask::add_paths(const char* value) {
  paths_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:google.protobuf.FieldMask.paths)
}
 void FieldMask::add_paths(const char* value, size_t size) {
  paths_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:google.protobuf.FieldMask.paths)
}
 const ::google::protobuf::RepeatedPtrField< ::std::string>&
FieldMask::paths() const {
  // @@protoc_insertion_point(field_list:google.protobuf.FieldMask.paths)
  return paths_;
}
 ::google::protobuf::RepeatedPtrField< ::std::string>*
FieldMask::mutable_paths() {
  // @@protoc_insertion_point(field_mutable_list:google.protobuf.FieldMask.paths)
  return &paths_;
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)
