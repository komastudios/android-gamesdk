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

#include "System.Asset.hpp"

#include <android/asset_manager_jni.h>

#include <ancer/util/JNIHelpers.hpp>
#include <ancer/util/Log.hpp>

using namespace ancer;

namespace {
  Log::Tag TAG{"SystemAsset"};
}

//==================================================================================================

Asset::Asset(const std::string &asset_path, const int opening_mode) {
  AAssetManager *native_asset_manager = nullptr;
      jni::SafeJNICall([&native_asset_manager](jni::LocalJNIEnv *env) {
    jobject activity = env->NewLocalRef(jni::GetActivityWeakGlobalRef());
    jclass activity_class = env->GetObjectClass(activity);

    jmethodID get_assets_method_id = env->GetMethodID(
        activity_class, "getAssets", "()Landroid/content/res/AssetManager;"
    );
    jobject java_asset_manager = env->CallObjectMethod(activity, get_assets_method_id);
    native_asset_manager = AAssetManager_fromJava(env->GetOriginalJNIEnv(), java_asset_manager);
  });

  _asset_ptr = AAssetManager_open(native_asset_manager, asset_path.c_str(), opening_mode);
  if (not _asset_ptr) {
    Log::E(TAG, "Error opening asset at %s", asset_path.c_str());
  }
}

Asset::~Asset() {
  if (_asset_ptr) {
    AAsset_close(_asset_ptr);
  }
}

Asset::operator bool() const {
  return _asset_ptr != nullptr;
}

const void *Asset::GetBuffer() const {
  return _asset_ptr ? AAsset_getBuffer(_asset_ptr) : nullptr;
}

int64_t Asset::GetLength() const {
  return _asset_ptr ? AAsset_getLength64(_asset_ptr) : 0;
}

int64_t Asset::GetRemainingLength() const {
  return _asset_ptr ? AAsset_getRemainingLength64(_asset_ptr) : 0;
}

bool Asset::IsAllocated() const {
  return _asset_ptr ? AAsset_isAllocated(_asset_ptr) : false;
}

int Asset::OpenFileDescriptor(int64_t *outStart, int64_t *outLength) const {
  return _asset_ptr ? AAsset_openFileDescriptor64(_asset_ptr, outStart, outLength) : -1;
}

int Asset::Read(void *buf, const size_t count) const {
  return _asset_ptr ? AAsset_read(_asset_ptr, buf, count) : -1;
}

int64_t Asset::Seek(const int64_t offset, const int whence) const {
  return _asset_ptr ? AAsset_seek64(_asset_ptr, offset, whence) : -1;
}

