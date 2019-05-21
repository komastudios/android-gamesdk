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
#include <dlfcn.h>
#include "tuningfork_utils.h"
#include <android/asset_manager_jni.h>



#include <cstdlib>
#include <cstring>
#include "Log.h"
#define LOG_TAG "UnityTuningfork"

using namespace tuningfork;

namespace {
    static JavaVM* vm_;
    static JNIEnv* env_;
    static jobject context_;
    bool everCalled = false;

    typedef SwappyBackend (*UnitySwappyTracerFn)(const SwappyTracer*);
    typedef uint32_t (*UnitySwappyVersion)();

    SwappyTracerFn swappy_tracer_fn = nullptr;

    template <class T> T findFunction (const char* lib_name, const char* function_name) {
        void* lib = dlopen(lib_name, RTLD_NOW);
        if(lib == nullptr) return nullptr;

        ALOGI("%s is found", lib_name);
        T fn = reinterpret_cast<T>(dlsym(lib, function_name));
        if(fn != nullptr) {
            ALOGI("%s is found", function_name);
        } else {
            ALOGW("%s is not found", function_name);
        }
        return fn;
    }

    // Gets the serialized settings from the APK.
    // Returns false if there was an error.
    bool GetSettingsSerialization(JNIEnv* env, jobject context,
                                            CProtobufSerialization* settings_ser) {
        auto asset = apk_utils::GetAsset(env, context, "tuningfork/tuningfork_settings.bin");
        if (asset == nullptr )
            return false;
        ALOGI("Got settings from tuningfork/tuningfork_settings.bin");
        // Get serialized settings from assets
        uint64_t size = AAsset_getLength64(asset);
        settings_ser->bytes = (uint8_t*)::malloc(size);
        memcpy(settings_ser->bytes, AAsset_getBuffer(asset), size);
        settings_ser->size = size;
        settings_ser->dealloc = CProtobufSerialization_Dealloc;
        AAsset_close(asset);
        return true;
    }

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
        env_, context_,  defaultParams, params, timeout_ms);
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

TFErrorCode Unity_TuningFork_findProtoSettingsInApk(CProtobufSerialization* s) {
    //CProtobufSerialization settings;

    if (GetSettingsSerialization(env_, context_, s)) {
        //s = &settings;
        return TFERROR_OK;
    }
    else {
        return TFERROR_NO_SETTINGS;
    }

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


TFErrorCode Unity_TuningFork_setUploadCallback(UploadCallback cbk){
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
    TuningFork_startFidelityParamDownloadThread(env_, context_, defaultParams,
        fidelity_params_callback, initialTimeoutMs, ultimateTimeoutMs);
}
UnitySwappyTracerFn unity_swappy_tracer_fn;

void UnityTracer(const SwappyTracer* tracer) {
    SwappyBackend swappyBackend = unity_swappy_tracer_fn(tracer);
    ALOGI("Swappy backend %d", swappyBackend);
}

bool Unity_TuningFork_swappyIsAvailable() {
    unity_swappy_tracer_fn =
        findFunction<UnitySwappyTracerFn>("libunity.so", "UnitySwappy_injectTracer");

    UnitySwappyVersion version_fn = findFunction<UnitySwappyVersion>("libunity.so", "UnitySwappy_version");

    if(version_fn != nullptr) {
        auto version = version_fn();
        ALOGI("Swappy version: [%d]", version);
    }

    if(unity_swappy_tracer_fn != nullptr) {
        swappy_tracer_fn = UnityTracer;
    }
    if(swappy_tracer_fn == nullptr) {
        swappy_tracer_fn = findFunction<SwappyTracerFn>("libswappy.so", "Swappy_injectTracer");
    }
    if(swappy_tracer_fn == nullptr) {
        swappy_tracer_fn = findFunction<SwappyTracerFn>("libswappy.so", "SwappyGL_injectTracer");
    }
    return swappy_tracer_fn != nullptr;
}

TFErrorCode Unity_TuningFork_initFromAssetsWithSwappy(
                             VoidCallback frame_callback,
                             const char* url_base,
                             const char* api_key,
                             const char* fp_default_file_name,
                             ProtoCallback fidelity_params_callback,
                             int initialTimeoutMs, int ultimateTimeoutMs){
    //ALOGW("Swappy_version %d", swappy_lib_version);
    ALOGW("Url base %s", url_base);
    ALOGW("Api key %s", api_key);
    ALOGW("FP default file name %s", fp_default_file_name);
    ALOGW("Initial timeout %d", initialTimeoutMs);
    ALOGW("Ultimate timeout %d", ultimateTimeoutMs);

    Unity_TuningFork_swappyIsAvailable();

    return TuningFork_initFromAssetsWithSwappy(env_, context_, swappy_tracer_fn, 0,
        frame_callback, fp_default_file_name, fidelity_params_callback,
        initialTimeoutMs, ultimateTimeoutMs);
}

TFErrorCode Unity_TuningFork_saveOrDeleteFidelityParamsFile(CProtobufSerialization* fps) {
    return TuningFork_saveOrDeleteFidelityParamsFile(env_, context_, fps);
}
} // extern "C" {
