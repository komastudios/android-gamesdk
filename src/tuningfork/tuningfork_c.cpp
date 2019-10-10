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

#include "tuningfork/tuningfork.h"
#include "tuningfork/tuningfork_extra.h"
#include "tuningfork/protobuf_util.h"
#include "tuningfork_internal.h"
#include "tuningfork_utils.h"
#include <jni.h>

#include <cstdlib>
#include <cstring>

extern "C" {

namespace tf = tuningfork;

TFErrorCode TuningFork_init_internal(const TFSettings *c_settings_in, JNIEnv* env, jobject context) {
    tf::Settings settings {};
    if (c_settings_in != nullptr) {
        settings.c_settings = *c_settings_in;
    }
    tf::JniCtx jni {env, context};
    TFErrorCode err = FindSettingsInApk(&settings, jni);
    if (err!=TFERROR_OK)
        return err;
    std::string default_save_dir = tf::file_utils::GetAppCacheDir(jni) + "/tuningfork";
    CheckSettings(settings, default_save_dir);
    err = tf::Init(settings, jni);
    if (err!=TFERROR_OK)
        return err;
    if ( !(settings.default_fp_filename.empty()
           || settings.c_settings.fidelity_params_callback==nullptr) ) {
        err = GetDefaultsFromAPKAndDownloadFPs(settings, jni);
    }
    return err;
}

// Blocking call to get fidelity parameters from the server.
// Note that once fidelity parameters are downloaded, any timing information is recorded
//  as being associated with those parameters.
TFErrorCode TuningFork_getFidelityParameters(const CProtobufSerialization *defaultParams,
                                      CProtobufSerialization *params, uint32_t timeout_ms) {
    tf::ProtobufSerialization defaults;
    if(defaultParams)
        defaults = tf::ToProtobufSerialization(*defaultParams);
    tf::ProtobufSerialization s;
    TFErrorCode result = tf::GetFidelityParameters(defaults, s, timeout_ms);
    if (result==TFERROR_OK && params)
        tf::ToCProtobufSerialization(s, *params);
    return result;
}

// Protobuf serialization of the current annotation
TFErrorCode TuningFork_setCurrentAnnotation(const CProtobufSerialization *annotation) {
    if(annotation)
        return tf::SetCurrentAnnotation(tf::ToProtobufSerialization(*annotation));
    else
        return TFERROR_INVALID_ANNOTATION;
}

// Record a frame tick that will be associated with the instrumentation key and the current
//   annotation
TFErrorCode TuningFork_frameTick(TFInstrumentKey id) {
    return tf::FrameTick(id);
}

// Record a frame tick using an external time, rather than system time
TFErrorCode TuningFork_frameDeltaTimeNanos(TFInstrumentKey id, TFDuration dt) {
    return tf::FrameDeltaTimeNanos(id, std::chrono::nanoseconds(dt));
}

// Start a trace segment
TFErrorCode  TuningFork_startTrace(TFInstrumentKey key, TFTraceHandle* handle) {
    if (handle==nullptr) return TFERROR_INVALID_TRACE_HANDLE;
    return tf::StartTrace(key, *handle);
}

// Record a trace with the key and annotation set using startTrace
TFErrorCode TuningFork_endTrace(TFTraceHandle h) {
    return tf::EndTrace(h);
}

TFErrorCode TuningFork_flush() {
    return tf::Flush();
}

void TUNINGFORK_VERSION_SYMBOL() {
    // Intentionally empty: this function is used to ensure that the proper
    // version of the library is linked against the proper headers.
    // In case of mismatch, a linker error will be triggered because of an
    // undefined symbol, as the name of the function depends on the version.
}

} // extern "C" {
