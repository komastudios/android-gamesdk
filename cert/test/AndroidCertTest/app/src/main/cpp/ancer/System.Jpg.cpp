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

#include "System.Jpg.hpp"
#include "System.Asset.hpp"

#include <memory>
#include <string>

#include <basis_universal/jpgd.h>

#include <ancer/util/JNIHelpers.hpp>
#include <ancer/util/Log.hpp>

using namespace ancer;

namespace {
Log::Tag TAG{"SystemJpg"};
}

//==================================================================================================


class JpegDecoderAssetStream : public jpgd::jpeg_decoder_stream {
 public:
  explicit JpegDecoderAssetStream(const std::string &asset_path) :
  _asset{asset_path, AASSET_MODE_STREAMING}, _is_eof{false}, _is_error{false} {}

  explicit operator bool() const {
    return (bool)_asset;
  }

  [[nodiscard]] size_t size() const {
    return _asset.GetLength();
  }

  int read(jpgd::uint8 *pBuf, int max_bytes_to_read, bool *pEOF_flag) override {
    if (not _asset || _is_error) {
      return -1;
    }

    if (_is_eof) {
      *pEOF_flag = true;
      return 0;
    }

    int bytes_read = _asset.Read(static_cast<void *>(pBuf), max_bytes_to_read);
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

 private:
  Asset _asset;
  bool _is_eof, _is_error;
};

//==================================================================================================

JpgTextureMetadata::JpgTextureMetadata(const std::string &relative_path,
                                       const std::string &filename_stem)
    : UncompressedTextureMetadata(relative_path, filename_stem) {}

const std::string &JpgTextureMetadata::GetFilenameExtension() const {
  static const std::string kJpg("jpg");
  return kJpg;
}

const std::string &JpgTextureMetadata::GetChannels() const {
  static const std::string kRgb("rgb");
  return kRgb;
}

void JpgTextureMetadata::_Load() {
  static const int kRequiredComponents{4};

  JpegDecoderAssetStream asset_stream{ToString()};

  if (asset_stream) {
    _file_size = asset_stream.size();
    int width = 0, height = 0, actual_comps = 0;
    uint8_t *bitmap_data = jpgd::decompress_jpeg_image_from_stream(
        &asset_stream,
        &width,
        &height,
        &actual_comps,
        kRequiredComponents,
        jpgd::jpeg_decoder::cFlagLinearChromaFiltering);
    if (bitmap_data) {
      _width = width;
      _height = height;
      _has_alpha = false;
      _bitmap_data = std::unique_ptr<u_char>(bitmap_data);
      _mem_size = width * kRequiredComponents * height;
    }
  }
}

void JpgTextureMetadata::_ApplyBitmap() {
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               _width, _height, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, _bitmap_data.get());
}

