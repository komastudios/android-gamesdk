/*
 * Copyright 2020 The Android Open Source Project
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

#include "annotation_map.h"

#include <functional>
#include <utility>

namespace {

// Since we expect the serializations to be small, use a simple Murmur hash.
inline uint32_t MurmurHash(const uint8_t* data, int len) {
    static std::__murmur2_or_cityhash<uint32_t> murmur;
    return murmur(data, len);
}

}  // anonymous namespace

namespace tuningfork {

TuningFork_ErrorCode AnnotationMap::GetOrInsert(
    const ProtobufSerialization& ser, AnnotationId& id) {
    id = MurmurHash(ser.data(), ser.size());
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = inner_map_.find(id);
    if (it == inner_map_.end()) inner_map_.insert({id, ser});
    return TUNINGFORK_ERROR_OK;
}

TuningFork_ErrorCode AnnotationMap::Get(AnnotationId id,
                                        ProtobufSerialization& ser) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = inner_map_.find(id);
    if (it == inner_map_.end()) return TUNINGFORK_ERROR_INVALID_ANNOTATION;
    ser = it->second;
    return TUNINGFORK_ERROR_OK;
}

}  // namespace tuningfork
