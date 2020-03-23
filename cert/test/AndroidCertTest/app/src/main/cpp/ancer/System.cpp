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

//==============================================================================

std::string ancer::LoadText(const char *file_name) {
  std::string text;
  jni::SafeJNICall(
      [file_name, &text](jni::LocalJNIEnv *env) {
        jstring name = env->NewStringUTF(file_name);
        jobject activity = env->NewLocalRef(jni::GetActivityWeakGlobalRef());
        jclass activity_class = env->GetObjectClass(activity);

        jmethodID get_asset_helpers_mid = env->GetMethodID(
            activity_class, "getSystemHelpers",
            "()Lcom/google/gamesdk/gamecert/operationrunner/util/SystemHelpers;"
        );
        jobject asset_helpers_instance =
            env->CallObjectMethod(activity, get_asset_helpers_mid);
        jclass asset_helpers_class =
            env->GetObjectClass(asset_helpers_instance);

        jmethodID load_text_mid = env->GetMethodID(
            asset_helpers_class, "loadText",
            "(Ljava/lang/String;)Ljava/lang/String;");
        auto result_j_string = (jstring) env->CallObjectMethod(
            asset_helpers_instance, load_text_mid,
            name);

        if (result_j_string) {
          const char *result_utf_ptr =
              env->GetStringUTFChars(result_j_string, nullptr);
          text = std::string(result_utf_ptr);
          env->ReleaseStringUTFChars(result_j_string, result_utf_ptr);
        }
      });
  return text;
}
