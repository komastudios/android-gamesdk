/*
 * Copyright 2019 The Android Open Source Project
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

#include <string>

#include "tuningfork/tuningfork.h"
#include "proto/protobuf_util.h"
#include "core/request_info.h"
#include "core/id_provider.h"
#include "core/prong.h"

namespace tuningfork {

class JsonSerializer {
public:
    static void SerializeEvent(const ProngCache& t,
                               const RequestInfo& device_info,
                               std::string& evt_json_ser);

    static TuningFork_ErrorCode DeserializeAndMerge(
        const std::string& evt_json_ser,
        IdProvider& id_provider,
        ProngCache& pc);
};

} // namespace tuningfork
