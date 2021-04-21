/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @defgroup Protobuf_nano_util Utility functions for nanopb-c
 *
 * These structures are useful when serializing and deserializing either vectors
 * of bytes or malloc'd byte streams, using `pb_encode` and `pb_decode`.
 *
 * @see external/nanopb-c
 * @{
 */

#include "protobuf_util_internal.h"

#include <map>

#include "nano/descriptor.pb.h"
#include "pb_decode.h"
#include "proto/protobuf_nano_util.h"
#include "tuningfork_utils.h"

namespace tuningfork {

// Structures and functions needed for the decoding of a
// dev_tuningfork.descriptor.
namespace file_descriptor {

static bool DecodeField(pb_istream_t* stream, const pb_field_t* field,
                        void** arg) {
    MessageType* message = static_cast<MessageType*>(*arg);
    google_protobuf_FieldDescriptorProto f =
        google_protobuf_FieldDescriptorProto_init_default;
    EnumField ef;
    f.name.funcs.decode = DecodeString;
    f.name.arg = &ef.name;
    f.type_name.funcs.decode = DecodeString;
    f.type_name.arg = &ef.type_name;
    if (!pb_decode(stream, google_protobuf_FieldDescriptorProto_fields, &f))
        return false;
    if (f.type == google_protobuf_FieldDescriptorProto_Type_TYPE_ENUM) {
        ef.number = f.number;
        message->fields.push_back(ef);
    }
    return true;
}

static bool DecodeMessageType(pb_istream_t* stream, const pb_field_t* field,
                              void** arg) {
    File* d = static_cast<File*>(*arg);
    google_protobuf_DescriptorProto desc =
        google_protobuf_DescriptorProto_init_default;
    MessageType t;
    desc.name.funcs.decode = DecodeString;
    desc.name.arg = &t.name;
    desc.field.funcs.decode = DecodeField;
    desc.field.arg = &t;
    if (!pb_decode(stream, google_protobuf_DescriptorProto_fields, &desc))
        return false;
    d->message_type.push_back(t);
    return true;
}

static bool DecodeValue(pb_istream_t* stream, const pb_field_t* field,
                        void** arg) {
    EnumType* e = static_cast<EnumType*>(*arg);
    google_protobuf_EnumValueDescriptorProto desc =
        google_protobuf_EnumValueDescriptorProto_init_default;
    std::string name;
    desc.name.funcs.decode = DecodeString;
    desc.name.arg = &name;
    if (!pb_decode(stream, google_protobuf_EnumValueDescriptorProto_fields,
                   &desc))
        return false;
    if (desc.has_number) e->value.push_back({name, desc.number});
    return true;
}

static bool DecodeEnumType(pb_istream_t* stream, const pb_field_t* field,
                           void** arg) {
    File* d = static_cast<File*>(*arg);
    google_protobuf_EnumDescriptorProto desc =
        google_protobuf_EnumDescriptorProto_init_default;
    EnumType e;
    desc.name.funcs.decode = DecodeString;
    desc.name.arg = &e.name;
    desc.value.funcs.decode = DecodeValue;
    desc.value.arg = &e;
    if (!pb_decode(stream, google_protobuf_EnumDescriptorProto_fields, &desc))
        return false;
    d->enum_type.push_back(e);
    return true;
}

static bool DecodeFile(pb_istream_t* stream, const pb_field_t* field,
                       void** arg) {
    File* d = static_cast<File*>(*arg);
    google_protobuf_FileDescriptorProto fdesc =
        google_protobuf_FileDescriptorProto_init_default;
    fdesc.message_type.funcs.decode = DecodeMessageType;
    fdesc.message_type.arg = d;
    fdesc.enum_type.funcs.decode = DecodeEnumType;
    fdesc.enum_type.arg = d;
    fdesc.package.funcs.decode = DecodeString;
    fdesc.package.arg = &d->package;
    return pb_decode(stream, google_protobuf_FileDescriptorProto_fields,
                     &fdesc);
}

bool DecodeString(pb_istream_t* stream, const pb_field_t* field, void** arg) {
    std::string* out_str = static_cast<std::string*>(*arg);
    out_str->resize(stream->bytes_left);
    char* d = const_cast<char*>(
        out_str->data());  // C++17 would remove the need for cast
    while (stream->bytes_left) {
        uint64_t x;
        if (!pb_decode_varint(stream, &x)) return false;
        *d++ = (char)x;
    }
    return true;
}

bool GetTuningForkFileDescriptorSerialization(
    ProtobufSerialization& descriptor_ser) {
    static ProtobufSerialization descriptor_ser_cache;
    if (descriptor_ser_cache.size() != 0) {
        descriptor_ser = descriptor_ser_cache;
        return true;
    }
    return apk_utils::GetAssetAsSerialization(
        "tuningfork/dev_tuningfork.descriptor", descriptor_ser);
}

bool GetTuningForkFileDescriptor(File& file) {
    ProtobufSerialization descriptor_ser;
    if (!GetTuningForkFileDescriptorSerialization(descriptor_ser)) return false;
    ByteStream str{const_cast<uint8_t*>(descriptor_ser.data()),
                   descriptor_ser.size(), 0};
    pb_istream_t stream = {ByteStream::Read, &str, descriptor_ser.size()};
    google_protobuf_FileDescriptorSet descriptor =
        google_protobuf_FileDescriptorSet_init_default;
    descriptor.file.funcs.decode = DecodeFile;
    descriptor.file.arg = &file;
    return pb_decode(&stream, google_protobuf_FileDescriptorSet_fields,
                     &descriptor);
}

}  // namespace file_descriptor

}  // namespace tuningfork
