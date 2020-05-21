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

#include "Png.hpp"

#include <memory>
#include <string>

#include <basis_universal/lodepng.h>

#include <ancer/System.Asset.hpp>
#include <ancer/util/Log.hpp>

using namespace ancer;

namespace {
Log::Tag TAG{"TexturePng"};
}

//==================================================================================================

namespace {
/**
 * Temporary decoder until this project is based on upcoming Android "R". Android "R" NDK brings an
 * ImageDecoder that makes this type irrelevant. For now, we needed to avoid decoding the bitmap via
 * JNI (which is way slower and doesn't work for a fair loading time comparison between texture
 * formats).
 * This class decodes a PNG stored in the app assets.
 */
class LodePngAssetDecoder {
 public:
  explicit LodePngAssetDecoder(const std::string &asset_path) :
      _asset{asset_path, AASSET_MODE_STREAMING},
      _asset_path{asset_path} {
    lodepng_state_init(&_state);
    _state.info_raw.colortype = LCT_RGBA;
    _state.info_raw.bitdepth = 8;
  }

  virtual ~LodePngAssetDecoder() {
    lodepng_state_cleanup(&_state);
  }

  /**
   * Decodes the PNG asset.
   *
   * @param p_width pointer to the place where the PNG width is to be left.
   * @param p_height pointer to the place where the PNG height is to be left.
   * @param p_encoded_size pointer to the place where the PNG size at rest is to be left.
   * @param p_decoded_size pointer to the place where the PNG size in memory is to be left.
   * @return a pointer to the decoded bitmap. Its deallocation is this method invoker's
   *         responsibility.
   */
  u_char *Decode(int32_t *p_width,
                 int32_t *p_height,
                 size_t *p_encoded_size,
                 size_t *p_decoded_size) {
    u_char *p_bitmap{nullptr};

    *p_encoded_size = static_cast<size_t>(_asset.GetLength());
    auto encoded_data = std::unique_ptr<u_char>(new u_char[*p_encoded_size]);

    if (_asset.Read(encoded_data.get(), *p_encoded_size) == *p_encoded_size) {
      u_int w, h;
      if (auto error =
          lodepng_decode(&p_bitmap, &w, &h, &_state, encoded_data.get(), *p_encoded_size)) {
        Log::E(TAG, "Error decoding asset %s: %s", _asset_path.c_str(), lodepng_error_text(error));
      } else {
        *p_width = w;
        *p_height = h;
        *p_decoded_size = lodepng_get_raw_size(w, h, &_state.info_raw);
      }
    } else {
      Log::E(TAG, "Reading error in asset %s", _asset_path.c_str());
    }

    return p_bitmap;
  }

  explicit operator bool() const {
    return (bool) _asset;
  }

 private:
  Asset _asset;
  LodePNGState _state;
  const std::string &_asset_path;
};
}

//==================================================================================================

PngTexture::PngTexture(const std::string &relative_path, const std::string &filename_stem)
    : EncodedTexture(relative_path, filename_stem, TextureChannels::RGBA) {}

const std::string &PngTexture::GetFilenameExtension() const {
  static const std::string kPng("png");
  return kPng;
}

std::string PngTexture::GetFormat() const {
  static const std::string kPng("PNG");
  return kPng;
}

void PngTexture::_Load() {
  if (auto decoder = LodePngAssetDecoder{ToString(*this)}) {
    _bitmap_data = std::unique_ptr<u_char>(
        decoder.Decode(&_width, &_height, &_file_size, &_mem_size));
    _has_alpha = true;
  }
}

void PngTexture::_ApplyBitmap() {
  glTexImage2D(GL_TEXTURE_2D, 0, _internal_gl_format,
               _width, _height, 0,
               _internal_gl_format, GL_UNSIGNED_BYTE, _bitmap_data.get());

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Generate mipmap
  glGenerateMipmap(GL_TEXTURE_2D);
}

