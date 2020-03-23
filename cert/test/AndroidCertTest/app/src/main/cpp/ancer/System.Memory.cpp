/*
 * Copyright 2020 The Android Open Source Project
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

#include "util/JNIHelpers.hpp"

ancer::MemoryInfo ancer::GetMemoryInfo() {
  auto info = MemoryInfo{};
  jni::SafeJNICall(
      [&info](jni::LocalJNIEnv *env) {
        jobject activity = env->NewLocalRef(jni::GetActivityWeakGlobalRef());
        jclass activity_class = env->GetObjectClass(activity);

        // get the memory info object from our activity

        jmethodID get_system_helpers_mid = env->GetMethodID(
            activity_class,
            "getSystemHelpers",
            "()Lcom/google/gamesdk/gamecert/operationrunner/util/SystemHelpers;"
        );
        jobject system_helpers_instance = env->CallObjectMethod(activity, get_system_helpers_mid);
        jclass system_helpers_class = env->GetObjectClass(system_helpers_instance);

        jmethodID get_memory_info_mid = env->GetMethodID(
            system_helpers_class, "getMemoryInfo",
            "()Lcom/google/gamesdk/gamecert/operationrunner/util/"
            "SystemHelpers$MemoryInformation;");

        jobject j_info_obj = env->CallObjectMethod(system_helpers_instance, get_memory_info_mid);

        // get the field ids for the object

        jclass java_cls = LoadClass(
            env, activity,
            "com/google/gamesdk/gamecert/operationrunner/util/"
            "SystemHelpers$MemoryInformation");
        jfieldID fid_oom_score = env->GetFieldID(java_cls, "oomScore", "I");
        jfieldID fid_native_heap_allocation_size = env->GetFieldID(
            java_cls, "nativeHeapAllocationSize", "J");
        jfieldID fid_commit_limit = env->GetFieldID(java_cls, "commitLimit", "J");
        jfieldID fid_total_memory = env->GetFieldID(java_cls, "totalMemory", "J");
        jfieldID fid_available_memory = env->GetFieldID(java_cls, "availableMemory", "J");
        jfieldID fid_threshold = env->GetFieldID(java_cls, "threshold", "J");
        jfieldID fid_low_memory = env->GetFieldID(java_cls, "lowMemory", "Z");

        // finally fill out the struct

        info._oom_score = env->GetIntField(j_info_obj, fid_oom_score);
        info.native_heap_allocation_size = static_cast<long>(env->GetLongField(
            j_info_obj, fid_native_heap_allocation_size));
        info.commit_limit = static_cast<long>(env->GetLongField(j_info_obj, fid_commit_limit));
        info.total_memory = static_cast<long>(env->GetLongField(j_info_obj, fid_total_memory));
        info.available_memory = static_cast<long>(env->GetLongField(
            j_info_obj, fid_available_memory));
        info.low_memory_threshold = static_cast<long>(env->GetLongField(j_info_obj, fid_threshold));
        info.low_memory = env->GetBooleanField(j_info_obj, fid_low_memory);
      });
  return info;
}

//==============================================================================

void ancer::RunSystemGc() {
  jni::SafeJNICall(
      [](jni::LocalJNIEnv *env) {
        jobject activity = env->NewLocalRef(jni::GetActivityWeakGlobalRef());
        jclass activity_class = env->GetObjectClass(activity);

        // get the memory info object from our activity

        jmethodID get_system_helpers_mid = env->GetMethodID(
            activity_class, "getSystemHelpers",
            "()Lcom/google/gamesdk/gamecert/operationrunner/util/SystemHelpers;"
        );
        jobject system_helpers_instance = env->CallObjectMethod(activity, get_system_helpers_mid);
        jclass system_helpers_class = env->GetObjectClass(system_helpers_instance);

        jmethodID run_gc_mid = env->GetMethodID(system_helpers_class, "runGc", "()V");

        env->CallVoidMethod(system_helpers_instance, run_gc_mid);
      });
}
