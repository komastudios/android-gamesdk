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

#pragma once

#include "tuningfork/tuningfork.h"
#include "tuningfork/tuningfork_extra.h"
#include "proto/protobuf_util.h"

#include "core/backend.h"
#include "core/common.h"
#include "core/extra_upload_info.h"
#include "core/id_provider.h"
#include "core/meminfo_provider.h"
#include "core/request.h"
#include "core/settings.h"
#include "core/time_provider.h"

class AAsset;

// These functions are implemented in tuningfork.cpp
namespace tuningfork {

// If no backend is passed, the default backend, which uploads to the google endpoint is used.
// If no timeProvider is passed, std::chrono::steady_clock is used.
// If no env is passed, there can be no upload or download.
TuningFork_ErrorCode Init(const Settings& settings,
                 const ExtraUploadInfo* extra_info = nullptr,
                 IBackend* backend = 0,
                 ITimeProvider* time_provider = nullptr,
                 IMemInfoProvider* meminfo_provider = nullptr);

// Use save_dir to initialize the persister if it's not already set
void CheckSettings(Settings& c_settings, const std::string& save_dir);

// Blocking call to get fidelity parameters from the server.
// Returns true if parameters could be downloaded within the timeout, false otherwise.
// Note that once fidelity parameters are downloaded, any timing information is recorded
//  as being associated with those parameters.
// If you subsequently call GetFidelityParameters, any data that is already collected will be
// submitted to the backend.
TuningFork_ErrorCode GetFidelityParameters(const ProtobufSerialization& default_params,
                                  ProtobufSerialization& params, uint32_t timeout_ms);

// Protobuf serialization of the current annotation
TuningFork_ErrorCode SetCurrentAnnotation(
    const ProtobufSerialization& annotation);

// Record a frame tick that will be associated with the instrumentation key and the current
//   annotation
TuningFork_ErrorCode FrameTick(InstrumentationKey id);

// Record a frame tick using an external time, rather than system time
TuningFork_ErrorCode FrameDeltaTimeNanos(InstrumentationKey id, Duration dt);

// Start a trace segment
TuningFork_ErrorCode StartTrace(InstrumentationKey key, TraceHandle& handle);

// Record a trace with the key and annotation set using startTrace
TuningFork_ErrorCode EndTrace(TraceHandle h);

TuningFork_ErrorCode SetUploadCallback(TuningFork_UploadCallback cbk);

TuningFork_ErrorCode Flush();

TuningFork_ErrorCode Destroy();

// Load default fidelity params from either the saved file or the file in
//  settings.default_fidelity_parameters_filename, then start the download thread.
TuningFork_ErrorCode GetDefaultsFromAPKAndDownloadFPs(
    const Settings& settings);

TuningFork_ErrorCode KillDownloadThreads();

// Load settings from assets/tuningfork/tuningfork_settings.bin.
// Ownership of @p settings is passed to the caller: call
//  TuningFork_Settings_Free to deallocate data stored in the struct.
// Returns TUNINGFORK_ERROR_OK and fills 'settings' if the file could be loaded.
// Returns TUNINGFORK_ERROR_NO_SETTINGS if the file was not found.
TuningFork_ErrorCode FindSettingsInApk(Settings* settings);

// Get the current settings (TF must have been initialized)
const Settings* GetSettings();

TuningFork_ErrorCode SetFidelityParameters(
    const ProtobufSerialization& params);

TuningFork_ErrorCode FindFidelityParamsInApk(const std::string& filename,
                                    ProtobufSerialization& fp);

TuningFork_ErrorCode EnableMemoryRecording(bool enable);

} // namespace tuningfork
