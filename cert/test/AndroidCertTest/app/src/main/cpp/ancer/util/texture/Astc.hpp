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

#ifndef _SYSTEM_ASTC_HPP
#define _SYSTEM_ASTC_HPP

#include <string>

#include <ancer/util/Texture.hpp>

namespace ancer {

/**
 * ASTC compressed texture.
 */
class AstcTextureMetadata : public CompressedTextureMetadata {
 public:
  AstcTextureMetadata(const std::string &relative_path,
                      const std::string &filename_stem,
                      const uint bits_per_pixel,
                      const TexturePostCompressionFormat post_compression_format
                      = TexturePostCompressionFormat::NONE);

  AstcTextureMetadata &&MirrorPostCompressed(
      const TexturePostCompressionFormat post_compression_format) const;

 protected:
  void _OnBitmapLoaded() override;
  void _ApplyBitmap() override;
  const std::string &_GetInnerExtension() const override;

  uint _bits_per_pixel;
};

}

#endif  // _SYSTEM_ASTC_HPP
