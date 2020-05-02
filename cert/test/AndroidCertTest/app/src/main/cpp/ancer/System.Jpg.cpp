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

#include <android/asset_manager_jni.h>

#include <ancer/util/JNIHelpers.hpp>
#include <ancer/util/Log.hpp>

using namespace ancer;

namespace {
Log::Tag TAG{"SystemJpg"};
}

JpegDecoderAssetStream::JpegDecoderAssetStream() :
    _asset_ptr{nullptr}, _is_eof{false}, _is_error{false} {}

JpegDecoderAssetStream::~JpegDecoderAssetStream() {
  close();
}

bool JpegDecoderAssetStream::open(const std::string &asset_path) {
  close();

  _is_eof = false;
  _is_error = false;

  jni::SafeJNICall([this, &asset_path](jni::LocalJNIEnv *env) {
    jobject activity = env->NewLocalRef(jni::GetActivityWeakGlobalRef());
    jclass activity_class = env->GetObjectClass(activity);

    jmethodID get_assets_method_id = env->GetMethodID(
        activity_class,
        "getAssets",
        "()Landroid/content/res/AssetManager;"
    );
    jobject java_asset_manager = env->CallObjectMethod(activity, get_assets_method_id);
    AAssetManager *native_asset_manager = AAssetManager_fromJava(env->GetOriginalJNIEnv(),
                                                                 java_asset_manager);
    _asset_ptr = AAssetManager_open(native_asset_manager,
                                    asset_path.c_str(),
                                    AASSET_MODE_STREAMING);
  });

  return _asset_ptr != nullptr;
}

void JpegDecoderAssetStream::close() {
  if (_asset_ptr) {
    AAsset_close(_asset_ptr);
  }
}

int JpegDecoderAssetStream::read(jpgd::uint8 *pBuf, int max_bytes_to_read, bool *pEOF_flag) {
  if (!_asset_ptr)
    return -1;

  if (_is_eof) {
    *pEOF_flag = true;
    return 0;
  }

  if (_is_error)
    return -1;

  int bytes_read = AAsset_read(_asset_ptr, static_cast<void *>(pBuf), max_bytes_to_read);
  if (bytes_read < max_bytes_to_read) {
    if (bytes_read < 0) {
      _is_error = true;
      return -1;
    }

    _is_eof = true;
    *pEOF_flag = true;
  }

  return bytes_read;
}