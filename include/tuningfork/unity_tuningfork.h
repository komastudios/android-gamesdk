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

#include <stdint.h>
#include <jni.h>
#include "tuningfork/tuningfork.h"
#include "tuningfork/tuningfork_extra.h"

#ifdef __cplusplus
extern "C" {
#endif

jint JNI_OnLoad(JavaVM* vm, void* reserved);

//From tuningfork.h
TFErrorCode Unity_TuningFork_init_default();
TFErrorCode Unity_TuningFork_init(const TFSettings *settings);

TFErrorCode Unity_TuningFork_getFidelityParameters(
                             const char* url_base,
                             const char* api_key,
                             const CProtobufSerialization *defaultParams,
                             CProtobufSerialization *params, uint32_t timeout_ms);

TFErrorCode Unity_TuningFork_setCurrentAnnotation(const CProtobufSerialization *annotation);
TFErrorCode Unity_TuningFork_frameTick(TFInstrumentKey id);
TFErrorCode Unity_TuningFork_frameDeltaTimeNanos(TFInstrumentKey id, TFDuration dt);
TFErrorCode Unity_TuningFork_startTrace(TFInstrumentKey key, TFTraceHandle* handle);
TFErrorCode Unity_TuningFork_endTrace(TFTraceHandle h);
TFErrorCode Unity_TuningFork_flush();

//from tuningfork_extra.h
TFErrorCode Unity_TuningFork_findSettingsInApk(TFSettings* settings);

TFErrorCode Unity_TuningFork_findFidelityParamsInApk(
                                               const char* filename,
                                               CProtobufSerialization* fp);

TFErrorCode Unity_TuningFork_initWithSwappy(const TFSettings* settings,
                                      SwappyTracerFn swappy_tracer_fn,
                                      uint32_t swappy_lib_version,
                                      VoidCallback frame_callback);

TFErrorCode Unity_TuningFork_setUploadCallback(ProtoCallback cbk);

void Unity_TuningFork_startFidelityParamDownloadThread(
                                      const char* url_base,
                                      const char* api_key,
                                      const CProtobufSerialization* defaultParams,
                                      ProtoCallback fidelity_params_callback,
                                      int initialTimeoutMs, int ultimateTimeoutMs);

TFErrorCode Unity_TuningFork_initFromAssetsWithSwappy(
                             SwappyTracerFn swappy_tracer_fn,
                             uint32_t swappy_lib_version,
                             VoidCallback frame_callback,
                             const char* url_base,
                             const char* api_key,
                             const char* fp_default_file_name,
                             ProtoCallback fidelity_params_callback,
                             int initialTimeoutMs, int ultimateTimeoutMs);

TFErrorCode Unity_TuningFork_saveOrDeleteFidelityParamsFile(CProtobufSerialization* fps);

#ifdef __cplusplus
}
#endif
