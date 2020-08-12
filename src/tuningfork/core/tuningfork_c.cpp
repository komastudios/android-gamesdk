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

#include <cstdlib>
#include <cstring>

#include "jni/jni_helper.h"
#include "proto/protobuf_util.h"
#include "settings.h"
#include "tuningfork/tuningfork.h"
#include "tuningfork/tuningfork_extra.h"
#include "tuningfork_internal.h"
#include "tuningfork_utils.h"

extern "C" {

namespace tf = tuningfork;
namespace jni = tuningfork::jni;

TuningFork_ErrorCode TuningFork_init_internal(
    const TuningFork_Settings *c_settings_in, JNIEnv *env, jobject context) {
    tf::Settings settings{};
    if (c_settings_in != nullptr) {
        settings.c_settings = *c_settings_in;
    }
    jni::Init(env, context);
    bool first_run = tf::CheckIfFirstRun();
    TuningFork_ErrorCode err = tf::Settings::FindInApk(&settings);
    if (err != TUNINGFORK_ERROR_OK) return err;
    settings.Check();
    err = tf::Init(settings, nullptr, nullptr, nullptr, nullptr, first_run);
    if (err != TUNINGFORK_ERROR_OK) return err;
    if (!(settings.default_fidelity_parameters_filename.empty() &&
          settings.c_settings.training_fidelity_params == nullptr)) {
        err = GetDefaultsFromAPKAndDownloadFPs(settings);
    }
    return err;
}

// Blocking call to get fidelity parameters from the server.
// Note that once fidelity parameters are downloaded, any timing information is
// recorded
//  as being associated with those parameters.
TuningFork_ErrorCode TuningFork_getFidelityParameters(
    const TuningFork_CProtobufSerialization *default_params,
    TuningFork_CProtobufSerialization *params, uint32_t timeout_ms) {
    tf::ProtobufSerialization defaults;
    if (default_params) defaults = tf::ToProtobufSerialization(*default_params);
    tf::ProtobufSerialization s;
    TuningFork_ErrorCode result =
        tf::GetFidelityParameters(defaults, s, timeout_ms);
    if (result == TUNINGFORK_ERROR_OK && params)
        tf::ToCProtobufSerialization(s, *params);
    return result;
}

// Protobuf serialization of the current annotation
TuningFork_ErrorCode TuningFork_setCurrentAnnotation(
    const TuningFork_CProtobufSerialization *annotation) {
    if (annotation != nullptr)
        return tf::SetCurrentAnnotation(
            tf::ToProtobufSerialization(*annotation));
    else
        return TUNINGFORK_ERROR_INVALID_ANNOTATION;
}

// Record a frame tick that will be associated with the instrumentation key and
// the current
//   annotation
TuningFork_ErrorCode TuningFork_frameTick(TuningFork_InstrumentKey id) {
    return tf::FrameTick(id);
}

// Record a frame tick using an external time, rather than system time
TuningFork_ErrorCode TuningFork_frameDeltaTimeNanos(TuningFork_InstrumentKey id,
                                                    TuningFork_Duration dt) {
    return tf::FrameDeltaTimeNanos(id, std::chrono::nanoseconds(dt));
}

// Start a trace segment
TuningFork_ErrorCode TuningFork_startTrace(TuningFork_InstrumentKey key,
                                           TuningFork_TraceHandle *handle) {
    if (handle == nullptr) return TUNINGFORK_ERROR_INVALID_TRACE_HANDLE;
    return tf::StartTrace(key, *handle);
}

// Record a trace with the key and annotation set using startTrace
TuningFork_ErrorCode TuningFork_endTrace(TuningFork_TraceHandle h) {
    return tf::EndTrace(h);
}

TuningFork_ErrorCode TuningFork_flush() { return tf::Flush(); }

TuningFork_ErrorCode TuningFork_destroy() {
    tf::KillDownloadThreads();
    return tf::Destroy();
}

TuningFork_ErrorCode TuningFork_setFidelityParameters(
    const TuningFork_CProtobufSerialization *params) {
    if (params != nullptr)
        return tf::SetFidelityParameters(tf::ToProtobufSerialization(*params));
    else
        return TUNINGFORK_ERROR_BAD_PARAMETER;
}

TuningFork_ErrorCode TuningFork_enableMemoryRecording(bool enable) {
    return tf::EnableMemoryRecording(enable);
}

TuningFork_ErrorCode TuningFork_recordLoadingTime(
    uint64_t time_ns, const TuningFork_LoadingTimeMetadata *eventMetadata,
    uint32_t eventMetadataSize) {
    // Handle LoadingTimeMetadata version changes here (none yet).
    if (eventMetadata == nullptr ||
        eventMetadataSize != sizeof(TuningFork_LoadingTimeMetadata))
        return TUNINGFORK_ERROR_BAD_PARAMETER;
    return tf::RecordLoadingTime(std::chrono::nanoseconds(time_ns),
                                 *eventMetadata);
}

TuningFork_ErrorCode TuningFork_reportLifecycleEvent(
    TuningFork_LifecycleState state) {
    return tf::ReportLifecycleEvent(state);
}

void TUNINGFORK_VERSION_SYMBOL() {
    // Intentionally empty: this function is used to ensure that the proper
    // version of the library is linked against the proper headers.
    // In case of mismatch, a linker error will be triggered because of an
    // undefined symbol, as the name of the function depends on the version.
}

}  // extern "C" {
