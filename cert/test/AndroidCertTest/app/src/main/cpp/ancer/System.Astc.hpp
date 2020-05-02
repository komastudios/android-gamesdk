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

// TODO(dagum): document that we won't cover ASTC extensions such as ASTC low precision
//              GL_EXT_texture_compression_astc_decode_mode and
//              GL_EXT_texture_compression_astc_decode_mode_rgb9e5
//              Covering, though, could help improve ratios in jpg (especially rgb9e5).

#ifndef _SYSTEM_ASTC_HPP
#define _SYSTEM_ASTC_HPP

#include <string>

#include <android/asset_manager.h>

#include <ancer/System.Gpu.hpp>

namespace ancer {

class AstcLoader {
 public:
 private:
  AAsset *_asset_ptr;
};

//==================================================================================================

class AstcTextureMetadata : public CompressedTextureMetadata {
 public:
  AstcTextureMetadata(const std::string &relative_path,
                      const std::string &filename_stem,
                      const TexturePostCompressionFormat post_compression_format
                      = TexturePostCompressionFormat::NONE);

 private:
  void _Load() override;
  void _ApplyBitmap() override ;
  const std::string &_GetInnerExtension() const override;
};

}

#endif  // _SYSTEM_ASTC_HPP
