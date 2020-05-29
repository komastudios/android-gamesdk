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

#ifndef _SYSTEM_ETC2_HPP
#define _SYSTEM_ETC2_HPP

#include <string>

#include <ancer/util/Texture.hpp>

namespace ancer {

/**
 * ETC2 compressed texture.
 */
class Etc2Texture : public CompressedTexture {
 public:
  Etc2Texture(const std::string &relative_path,
              const std::string &filename_stem,
              const TextureChannels channels,
              const TexturePostCompressionFormat post_compression_format
              = TexturePostCompressionFormat::NONE);

  /**
   * Clones this instance on a new one, but with a specific post compression format. Namely, if this
   * is a plain ETC2 texture, invoking this function with GZIP would return a similar texture but
   * post-compressed via gzip.
   */
  Etc2Texture MirrorPostCompressed(
      const TexturePostCompressionFormat post_compression_format) const;

 protected:
  void _OnBitmapLoaded() override;
  void _ApplyBitmap() override;
  const std::string &_GetInnerExtension() const override;
  std::string _GetInnerFormat() const override;
};

}

#endif  // _SYSTEM_ETC2_HPP
