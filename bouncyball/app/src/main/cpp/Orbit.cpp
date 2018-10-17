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

#define LOG_TAG "Orbit"

#include <string>

#include <jni.h>

#include <android/native_window_jni.h>

#include "swappy-utils/Log.h"
#include "swappy-utils/Settings.h"

#include "swappy/Swappy.h"

#include "Renderer.h"

using std::chrono::nanoseconds;

namespace {

template<typename T>
T to(jstring jstr, JNIEnv *env);

template<>
std::string to<std::string>(jstring jstr, JNIEnv *env) {
    const char *utf = env->GetStringUTFChars(jstr, nullptr);
    std::string str(utf);
    env->ReleaseStringUTFChars(jstr, utf);
    return str;
}
template <>
uint32_t to<uint32_t>(jstring jstr, JNIEnv *env) {
    auto utf = to<std::string>(jstr, env);
    uint32_t i = std::stoi(utf);
    return i;
}
template <>
uint64_t to<uint64_t>(jstring jstr, JNIEnv *env) {
    auto utf = to<std::string>(jstr, env);
    uint64_t i = std::stoll(utf);
    return i;
}
template <>
bool to<bool>(jstring jstr, JNIEnv *env) {
    auto utf = to<std::string>(jstr, env);
    bool value = false;
    if("true"==utf) value = true;
    return value;
}
} // anonymous namespace

extern "C" {

JNIEXPORT void JNICALL
Java_com_prefabulated_bouncyball_OrbitActivity_nInit(JNIEnv *env, jobject activity) {
    // Get the Renderer instance to create it
    Renderer::getInstance();

    Swappy::init(env, activity);
}

JNIEXPORT void JNICALL
Java_com_prefabulated_bouncyball_OrbitActivity_nSetSurface(JNIEnv *env, jobject /* this */,
                                                    jobject surface, jint width, jint height) {
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    Renderer::getInstance()->setWindow(window,
                                     static_cast<int32_t>(width),
                                     static_cast<int32_t>(height));
}

JNIEXPORT void JNICALL
Java_com_prefabulated_bouncyball_OrbitActivity_nClearSurface(JNIEnv * /* env */, jobject /* this */) {
    Renderer::getInstance()->setWindow(nullptr, 0, 0);
}

JNIEXPORT void JNICALL
Java_com_prefabulated_bouncyball_OrbitActivity_nStart(JNIEnv * /* env */, jobject /* this */) {
    ALOGI("start");
    Renderer::getInstance()->start();
}

JNIEXPORT void JNICALL
Java_com_prefabulated_bouncyball_OrbitActivity_nStop(JNIEnv * /* env */, jobject /* this */) {
    ALOGI("stop");
    Renderer::getInstance()->stop();
}

JNIEXPORT void JNICALL
Java_com_prefabulated_bouncyball_OrbitActivity_nOnChoreographer(JNIEnv * /* env */, jobject /* this */,
                                                         jlong frameTimeNanos) {
    Swappy::onChoreographer(frameTimeNanos);
}

JNIEXPORT void JNICALL
Java_com_prefabulated_bouncyball_OrbitActivity_nSetPreference(JNIEnv *env, jobject /* this */,
                                                         jstring key_in, jstring value) {
    auto key = to<std::string>(key_in, env);
    if(key=="refresh_period") {
        Settings::getInstance()->setRefreshPeriod(std::chrono::nanoseconds(to<uint64_t>(value, env)));
    }
    else if(key =="swap_interval") {
        Settings::getInstance()->setSwapInterval(to<uint32_t>(value, env));
    }
    else if(key == "use_affinity") {
        Settings::getInstance()->setUseAffinity(to<bool>(value, env));
    }
    else
        ALOGE("Unknown preference: %s", key.c_str());
}

} // extern "C"