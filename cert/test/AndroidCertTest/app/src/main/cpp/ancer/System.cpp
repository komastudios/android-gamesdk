/*
 * Copyright 2019 The Android Open Source Project
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

#include "System.hpp"

#include "util/Error.hpp"
#include "util/JNIHelpers.hpp"
#include "util/Log.hpp"

using namespace ancer;


//==============================================================================

namespace {
Log::Tag TAG{"ancer::system"};

std::filesystem::path _internal_data_path;
std::filesystem::path _raw_data_path;
std::filesystem::path _obb_path;
}

//==============================================================================

void ancer::internal::InitSystem(jobject activity, jstring internal_data_path,
                                 jstring raw_data_path, jstring obb_path) {
  jni::SafeJNICall([&](jni::LocalJNIEnv *env) {
    jni::Init(env, activity);

    _internal_data_path = std::filesystem::path(
        env->GetStringUTFChars(internal_data_path, nullptr));
    _raw_data_path = std::filesystem::path(env->GetStringUTFChars(raw_data_path, nullptr));
    _obb_path = std::filesystem::path(env->GetStringUTFChars(obb_path, nullptr));
  });
}

void ancer::internal::DeinitSystem() {
  jni::SafeJNICall([](jni::LocalJNIEnv *env) {
    jni::Deinit(env);
  });
}

//==============================================================================

std::string ancer::InternalDataPath() {
  return _internal_data_path.string();
}

std::string ancer::RawResourcePath() {
  return _raw_data_path.string();
}

std::string ancer::ObbPath() {
  return _obb_path.string();
}
