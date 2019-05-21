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
#include <jni.h>

#include <cstdlib>
#include <cstring>
#include "Log.h"
#define LOG_TAG "UnityTuningfork"

namespace {
    static JavaVM* vm_;
    static JNIEnv* env_;
    static jobject context_;
    bool everCalled = false;

} // anonymous namespace

extern "C" {

jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    if(vm == nullptr) return -1;
    everCalled = true;
    if(vm->GetEnv(reinterpret_cast<void**>(&env_), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    vm_ = vm;

    jclass activityThread = env_->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThread = env_->GetStaticMethodID(activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jmethodID getApplication = env_->GetMethodID(activityThread, "getApplication", "()Landroid/app/Application;");

    jobject activityThreadObj = env_->CallStaticObjectMethod(activityThread, currentActivityThread);
    jobject context = env_->CallObjectMethod(activityThreadObj, getApplication);

    context_ = env_->NewGlobalRef(context);

    return JNI_VERSION_1_6;
}

TFErrorCode Unity_TuningFork_init_default() {

    if(!everCalled) return TFERROR_JNI_BAD_JVM;

    if(vm_ == nullptr) return TFERROR_JNI_BAD_ENV;
    if(context_ == nullptr) return TFERROR_JNI_EXCEPTION;
    return TuningFork_init(nullptr, env_, context_);
}

TFErrorCode Unity_TuningFork_init(const TFSettings *settings) {
    return TuningFork_init(settings, env_, context_);
}

TFErrorCode Unity_TuningFork_getFidelityParameters(
                                      const char* url_base,
                                      const char* api_key,
                                      const CProtobufSerialization *defaultParams,
                                      CProtobufSerialization *params, uint32_t timeout_ms) {
    ALOGW("Unity_TuningFork_getFidelityParameters, url_base %s, api_key %s", url_base, api_key);
    return TuningFork_getFidelityParameters(
        env_, context_, url_base, api_key,  defaultParams, params, timeout_ms);
}

// Protobuf serialization of the current annotation
TFErrorCode Unity_TuningFork_setCurrentAnnotation(const CProtobufSerialization *annotation) {
   return TuningFork_setCurrentAnnotation(annotation);
}

TFErrorCode Unity_TuningFork_frameTick(TFInstrumentKey id) {
    return TuningFork_frameTick(id);
}

// Record a frame tick using an external time, rather than system time
TFErrorCode Unity_TuningFork_frameDeltaTimeNanos(TFInstrumentKey id, TFDuration dt) {
    return TuningFork_frameDeltaTimeNanos(id, dt);
}

// Start a trace segment
TFErrorCode  Unity_TuningFork_startTrace(TFInstrumentKey key, TFTraceHandle* handle) {
    return TuningFork_startTrace(key, handle);
}

// Record a trace with the key and annotation set using startTrace
TFErrorCode Unity_TuningFork_endTrace(TFTraceHandle h) {
    return TuningFork_endTrace(h);
}

TFErrorCode Unity_TuningFork_flush() {
    return TuningFork_flush();
}


TFErrorCode Unity_TuningFork_findSettingsInApk(TFSettings* settings) {
    return TuningFork_findSettingsInApk(env_, context_, settings);
}

TFErrorCode Unity_TuningFork_findFidelityParamsInApk(const char* filename,
                                               CProtobufSerialization* fp) {
    return TuningFork_findFidelityParamsInApk(env_, context_, filename, fp);
}

TFErrorCode Unity_TuningFork_initWithSwappy(const TFSettings* settings,
                                      JNIEnv* env,
                                      SwappyTracerFn swappy_tracer_fn,
                                      uint32_t swappy_lib_version,
                                      VoidCallback frame_callback){
    return TuningFork_initWithSwappy(settings, env_, context_, swappy_tracer_fn, swappy_lib_version, frame_callback);
}


TFErrorCode Unity_TuningFork_setUploadCallback(ProtoCallback cbk){
    return TuningFork_setUploadCallback(cbk);
}

void Unity_TuningFork_startFidelityParamDownloadThread(
                                      const char* url_base,
                                      const char* api_key,
                                      const CProtobufSerialization* defaultParams,
                                      ProtoCallback fidelity_params_callback,
                                      int initialTimeoutMs, int ultimateTimeoutMs){
    ALOGW("Url base %s", url_base);
    ALOGW("Api key %s", api_key);
    ALOGW("Initial timeout %d", initialTimeoutMs);
    ALOGW("Ultimate timeout %d", ultimateTimeoutMs);
    TuningFork_startFidelityParamDownloadThread(env_, context_, url_base, api_key, defaultParams,
        fidelity_params_callback, initialTimeoutMs, ultimateTimeoutMs);
}

TFErrorCode Unity_TuningFork_initFromAssetsWithSwappy(
                             SwappyTracerFn swappy_tracer_fn,
                             uint32_t swappy_lib_version,
                             VoidCallback frame_callback,
                             const char* url_base,
                             const char* api_key,
                             const char* fp_default_file_name,
                             ProtoCallback fidelity_params_callback,
                             int initialTimeoutMs, int ultimateTimeoutMs){
    ALOGW("Swappy_version %d", swappy_lib_version);
    ALOGW("Url base %s", url_base);
    ALOGW("Api key %s", api_key);
    ALOGW("FP default file name %s", fp_default_file_name);
    ALOGW("Initial timeout %d", initialTimeoutMs);
    ALOGW("Ultimate timeout %d", ultimateTimeoutMs);

    return TuningFork_initFromAssetsWithSwappy(env_, context_, swappy_tracer_fn, swappy_lib_version,
        frame_callback, url_base, api_key, fp_default_file_name, fidelity_params_callback,
        initialTimeoutMs, ultimateTimeoutMs);
}

TFErrorCode Unity_TuningFork_saveOrDeleteFidelityParamsFile(CProtobufSerialization* fps) {
    return TuningFork_saveOrDeleteFidelityParamsFile(env_, context_, fps);
}

} // extern "C" {
