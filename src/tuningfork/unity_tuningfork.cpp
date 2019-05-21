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
    enum SwappyBackend {
       Swappy_None=0, Swappy_OpenGL=1, Swappy_Vulkan=2
    };

    typedef SwappyBackend (*UnitySwappyTracerFn)(const SwappyTracer*);
    typedef uint32_t (*UnitySwappyVersion)();
    typedef bool (*SwappyIsEnabled)();

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

    static SwappyTracerFn s_swappy_tracer_fn = nullptr;
    static UnitySwappyTracerFn s_unity_swappy_tracer_fn = nullptr;
    static bool s_swappy_enabled = false;

    void UnityTracer(const SwappyTracer* tracer) {
        SwappyBackend swappyBackend = s_unity_swappy_tracer_fn(tracer);
        ALOGI("Swappy backend: %d", swappyBackend);
    }

    // Return swappy tracer for opengl.
    // There is no tracer for Vulkan in that version of swappy.
    SwappyTracerFn findTracerSwappy() {
        auto tracer = findFunction<SwappyTracerFn>("libswappy.so", "Swappy_injectTracer");
        if(tracer == nullptr) return nullptr;

        auto swappyIsEnabledFunc =
            findFunction<SwappyIsEnabled>("libswappy.so", "Swappy_isEnabled");
        if(swappyIsEnabledFunc == nullptr) return nullptr;

        bool swappyIsEnabled = swappyIsEnabledFunc();
        ALOGI("Swappy version 0_1 isEnabled: [%d]", swappyIsEnabled);
        if(swappyIsEnabled) return tracer;
        return nullptr;
    }

    UnitySwappyTracerFn findUnityTracer() {
        auto tracer = findFunction<UnitySwappyTracerFn>("libunity.so", "UnitySwappy_injectTracer");
        auto version_fn = findFunction<UnitySwappyVersion>("libunity.so", "UnitySwappy_version");
        if(version_fn != nullptr) {
            auto version = version_fn();
            ALOGI("Unity Swappy version: [%d]", version);
        }
        return tracer;
    }

    bool findSwappy() {
        s_unity_swappy_tracer_fn = findUnityTracer();

        if(s_unity_swappy_tracer_fn != nullptr) {
            s_swappy_tracer_fn = UnityTracer;
        }
        if(s_swappy_tracer_fn == nullptr) {
            s_swappy_tracer_fn = findTracerSwappy();
        }
        return s_swappy_tracer_fn != nullptr;
    }

} // anonymous namespace


extern "C" {

static JniCtx* s_jni;

jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    if(vm == nullptr) return -1;
    JNIEnv* env;
    if(vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    jclass activityThread = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThread,
        "currentActivityThread", "()Landroid/app/ActivityThread;");
    jmethodID getApplication = env->GetMethodID(activityThread,
        "getApplication", "()Landroid/app/Application;");

    jobject activityThreadObj = env->CallStaticObjectMethod(activityThread, currentActivityThread);
    jobject context = env->CallObjectMethod(activityThreadObj, getApplication);

    s_jni = new JniCtx(env, context);
    return JNI_VERSION_1_6;
}


TFErrorCode Unity_TuningFork_init(ProtoCallback fidelity_params_callback){

    s_swappy_enabled = findSwappy();
    TFSettings settings {};
    if (s_swappy_enabled) {
        settings.swappy_tracer_fn = s_swappy_tracer_fn;
    }
    settings.fidelity_params_callback = fidelity_params_callback;
    return TuningFork_init(&settings, s_jni->Env(), s_jni->Ctx());
}

bool Unity_TuningFork_swappyIsEnabled(){
    return s_swappy_enabled;
}

TFErrorCode Unity_TuningFork_findFidelityParamsInApk(
        const char* filename, CProtobufSerialization* fp) {
    return TuningFork_findFidelityParamsInApk(s_jni->Env(), s_jni->Ctx(), filename, fp);
}

TFErrorCode Unity_TuningFork_saveOrDeleteFidelityParamsFile(CProtobufSerialization* fps) {
    return TuningFork_saveOrDeleteFidelityParamsFile(s_jni->Env(), s_jni->Ctx(), fps);
}
} // extern "C" {