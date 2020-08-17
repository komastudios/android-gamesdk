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

#pragma once

#include <mutex>
#include <unordered_map>

#include "common.h"

namespace tuningfork {

// Stores the mapping from annotation serializations to annotation ids
// and back again.
class AnnotationMap {
    typedef std::unordered_map<AnnotationId, ProtobufSerialization>
        InnerContainer;
    typedef InnerContainer::iterator Iterator;
    InnerContainer inner_map_;
    std::mutex mutex_;

   public:
    TuningFork_ErrorCode GetOrInsert(const ProtobufSerialization& ser,
                                     AnnotationId& id);
    TuningFork_ErrorCode Get(AnnotationId id, ProtobufSerialization& ser);
};

}  // namespace tuningfork
