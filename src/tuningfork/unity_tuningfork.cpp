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

UnitySwappyTracerFn unity_swappy_tracer_fn;

void UnityTracer(const SwappyTracer* tracer) {
    SwappyBackend swappyBackend = unity_swappy_tracer_fn(tracer);
    ALOGI("Swappy backend %d", swappyBackend);
}

bool Unity_TuningFork_swappyIsAvailable() {
    unity_swappy_tracer_fn =
        findFunction<UnitySwappyTracerFn>("libunity.so", "UnitySwappy_injectTracer");

    UnitySwappyVersion version_fn =
        findFunction<UnitySwappyVersion>("libunity.so", "UnitySwappy_version");

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

jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    if(vm == nullptr) return -1;
    everCalled = true;
    if(vm->GetEnv(reinterpret_cast<void**>(&env_), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    vm_ = vm;

    jclass activityThread = env_->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThread = env_->GetStaticMethodID(activityThread,
        "currentActivityThread", "()Landroid/app/ActivityThread;");
    jmethodID getApplication = env_->GetMethodID(activityThread,
        "getApplication", "()Landroid/app/Application;");

    jobject activityThreadObj = env_->CallStaticObjectMethod(activityThread, currentActivityThread);
    jobject context = env_->CallObjectMethod(activityThreadObj, getApplication);

    context_ = env_->NewGlobalRef(context);

    return JNI_VERSION_1_6;
}

TFErrorCode Unity_TuningFork_initFromAssetsWithSwappy(
        VoidCallback frame_callback,
        const char* fp_default_file_name,
        ProtoCallback fidelity_params_callback,
        int initialTimeoutMs, int ultimateTimeoutMs){
    ALOGW("FP default file name %s", fp_default_file_name);

    if(!Unity_TuningFork_swappyIsAvailable()) {
        return TFERROR_NO_SWAPPY;
    }

    return TuningFork_initFromAssetsWithSwappy(env_, context_, swappy_tracer_fn, 0,
        frame_callback, fp_default_file_name, fidelity_params_callback,
        initialTimeoutMs, ultimateTimeoutMs);
}


TFErrorCode Unity_TuningFork_getFidelityParameters(
        const CProtobufSerialization *defaultParams,
        CProtobufSerialization *params,
        uint32_t timeout_ms) {
    return TuningFork_getFidelityParameters(env_, context_,  defaultParams, params, timeout_ms);
}

TFErrorCode Unity_TuningFork_findProtoSettingsInApk(CProtobufSerialization* s) {
    if (GetSettingsSerialization(env_, context_, s)) {
        return TFERROR_OK;
    }
    else {
        return TFERROR_NO_SETTINGS;
    }
}

TFErrorCode Unity_TuningFork_findFidelityParamsInApk(
        const char* filename,
        CProtobufSerialization* fp) {
    return TuningFork_findFidelityParamsInApk(env_, context_, filename, fp);
}

void Unity_TuningFork_startFidelityParamDownloadThread(
        const CProtobufSerialization* defaultParams,
        ProtoCallback fidelity_params_callback,
        int initialTimeoutMs,
        int ultimateTimeoutMs){
    TuningFork_startFidelityParamDownloadThread(env_, context_, defaultParams,
        fidelity_params_callback, initialTimeoutMs, ultimateTimeoutMs);
}

TFErrorCode Unity_TuningFork_saveOrDeleteFidelityParamsFile(CProtobufSerialization* fps) {
    return TuningFork_saveOrDeleteFidelityParamsFile(env_, context_, fps);
}
} // extern "C" {