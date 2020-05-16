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

namespace {
/**
 * A PKM header is a 16-byte holder of information about the image
 */
class PkmHeader {
 public:
  /**
   * Width of the original texture.
   */
  unsigned short GetWidth(void) {
    return (_width_msb << 8) | _width_lsb;
  }

  /**
   * Height of the original texture.
   */
  unsigned short GetHeight(void) {
    return (_height_msb << 8) | _height_lsb;
  }

  /**
   * Width of the compressed texture with padding added.
   */
  unsigned short GetPaddedWidth(void) {
    return (_padded_width_msb << 8) | _padded_width_lsb;
  }

  /**
   * Height of the compressed texture with padding added.
   */
  unsigned short GetPaddedHeight(void) {
    return (_padded_height_msb << 8) | _padded_height_lsb;
  }

  /**
   * Size of the compressed texture with padding added.
   */
  GLsizei getSize(GLenum internalFormat) {
    if (internalFormat != GL_COMPRESSED_RG11_EAC
        && internalFormat != GL_COMPRESSED_SIGNED_RG11_EAC
        && internalFormat != GL_COMPRESSED_RGBA8_ETC2_EAC
        && internalFormat != GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC) {
      return (GetPaddedWidth() * GetPaddedHeight()) >> 1;
    } else {
      return (GetPaddedWidth() * GetPaddedHeight());
    }
  }

 private:
  unsigned char _format_name[8];
  unsigned char _padded_width_msb;
  unsigned char _padded_width_lsb;
  unsigned char _padded_height_msb;
  unsigned char _padded_height_lsb;
  unsigned char _width_msb;
  unsigned char _width_lsb;
  unsigned char _height_msb;
  unsigned char _height_lsb;
};
}

//--------------------------------------------------------------------------------------------------

Etc2Texture::Etc2Texture(const std::string &relative_path,
                         const std::string &filename_stem,
                         const TextureChannels channels,
                         const TexturePostCompressionFormat post_compression_format)
    : CompressedTexture(relative_path,
                        filename_stem,
                        channels,
                        post_compression_format) {
  _internal_format = GetChannels() == TextureChannels::RGB ? GL_COMPRESSED_RGB8_ETC2
                                                           : GL_COMPRESSED_RGBA8_ETC2_EAC;
}

Etc2Texture Etc2Texture::MirrorPostCompressed(
    const TexturePostCompressionFormat post_compression_format) const {
  std::string filename_stem{_filename_stem};
  filename_stem.append(".").append(GetFilenameExtension());
  return Etc2Texture(_relative_path, filename_stem, _channels, post_compression_format);
}

void Etc2Texture::_OnBitmapLoaded() {
  PkmHeader header{*(reinterpret_cast<PkmHeader *>(_bitmap_data.get()))};

  _width = header.GetWidth();
  _height = header.GetHeight();
  _image_size = header.getSize(_internal_format);
}

void Etc2Texture::_ApplyBitmap() {
  glCompressedTexImage2D(GL_TEXTURE_2D, 0, _internal_format,
                         _width, _height, 0, _image_size,
                         _bitmap_data.get() + sizeof(PkmHeader));

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

const std::string &Etc2Texture::_GetInnerExtension() const {
  static const std::string kPkm("pkm");
  return kPkm;
}
