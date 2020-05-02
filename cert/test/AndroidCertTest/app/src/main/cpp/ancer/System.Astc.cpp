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
Log::Tag TAG{"SystemAstc"};
}

//==================================================================================================

AstcTextureMetadata::AstcTextureMetadata(const std::string &relative_path,
                                         const std::string &filename_stem,
                                         const TexturePostCompressionFormat post_compression_format)
    : CompressedTextureMetadata(relative_path, filename_stem, post_compression_format) {}

void AstcTextureMetadata::_Load() {
  // TODO(dagum): implement
}

void AstcTextureMetadata::_ApplyBitmap() {
  // TODO(dagum): implement
}

const std::string &AstcTextureMetadata::_GetInnerExtension() const {
  static const std::string kAstc("astc");
  return kAstc;
}
