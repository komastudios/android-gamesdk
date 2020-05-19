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

#include "Etc2.hpp"

#include <utility>

#include <ancer/util/Log.hpp>

using namespace ancer;

namespace {
Log::Tag TAG{"TextureEtc2"};
}

//==================================================================================================

//--------------------------------------------------------------------------------------------------

Etc2TextureMetadata::Etc2TextureMetadata(const std::string &relative_path,
                                         const std::string &filename_stem,
                                         const TexturePostCompressionFormat post_compression_format)
    : CompressedTextureMetadata(relative_path,
                                filename_stem,
                                post_compression_format) {}

Etc2TextureMetadata &&Etc2TextureMetadata::MirrorPostCompressed(
    const TexturePostCompressionFormat post_compression_format) const {
  return std::move(Etc2TextureMetadata(_relative_path,
                                       _filename_stem,
                                       post_compression_format));
}

void Etc2TextureMetadata::_OnBitmapLoaded() {
/* PKM file consists of a header with information about image (stored in 16 first bits) and image data. */
  const int sizeOfETCHeader = 16;
}

void Etc2TextureMetadata::_ApplyBitmap() {
}

const std::string &Etc2TextureMetadata::_GetInnerExtension() const {
  static const std::string kPkm("pkm");
  return kPkm;
}
