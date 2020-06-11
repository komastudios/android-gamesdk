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

#include "tuningfork/tuningfork.h"
#include "core/common.h"
#include "proto/protobuf_util.h"

class AAsset;

namespace tuningfork {

class IdProvider {
  public:
    virtual ~IdProvider() {}
    virtual uint64_t DecodeAnnotationSerialization(const ProtobufSerialization& ser,
                                                   bool* loading = nullptr) const = 0;
    virtual TuningFork_ErrorCode MakeCompoundId(InstrumentationKey k,
                                       uint64_t annotation_id,
                                       uint64_t& id) = 0;
};

} // namespace tuningfork
