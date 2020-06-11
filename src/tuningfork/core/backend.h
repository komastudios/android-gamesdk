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

#include <string>

#include "tuningfork/tuningfork.h"
#include "proto/protobuf_util.h"
#include "request.h"

namespace tuningfork {

class IBackend {
public:
    virtual ~IBackend() {};

    // Perform a blocking call to get fidelity parameters from the server.
    virtual TuningFork_ErrorCode GenerateTuningParameters(Request& request,
        const ProtobufSerialization* training_mode_fps,
        ProtobufSerialization& fidelity_params,
        std::string& experiment_id) = 0;

    // Perform a blocking call to upload telemetry info to a server.
    virtual TuningFork_ErrorCode UploadTelemetry(
        const std::string& tuningfork_log_event) = 0;

    // Perform a blocking call to upload debug info to a server.
    virtual TuningFork_ErrorCode UploadDebugInfo(Request& request)  = 0;

};

} // namespace tuningfork
