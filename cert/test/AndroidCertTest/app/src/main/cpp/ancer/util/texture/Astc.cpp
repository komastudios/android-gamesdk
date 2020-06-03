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

#include "Astc.hpp"

#include <sstream>

#include <ancer/util/Log.hpp>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

using namespace ancer;

namespace {
Log::Tag TAG{"TextureAstc"};
}

//==================================================================================================

namespace {

#define COMPUTE_SIZE(axis)  (_##axis##_size[0] + (_##axis##_size[1] << 8) + \
                            (_##axis##_size[2] << 16))

#define COMPUTE_BLOCKS(axis, size)  ((size + _##axis##_block_dim - 1) / _##axis##_block_dim)

/**
 * Header of an ASTC texture with informational functions about its bitmap payload.
 */
class AstcHeader {
 public:
  int32_t GetWidth() const {
    return COMPUTE_SIZE(x);
  }

  int32_t GetHeight() const {
    return COMPUTE_SIZE(y);
  }

  int32_t GetDepth() const {
    return COMPUTE_SIZE(z);
  }

  GLsizei GetSize() const {
    // Each block is encoded on 16 bytes, so calculate total compressed image data size
    return COMPUTE_BLOCKS(x, GetWidth()) * COMPUTE_BLOCKS(y, GetHeight())
        * COMPUTE_BLOCKS(z, GetDepth()) << 4;
  }

 private:
  unsigned char _magic[4];
  unsigned char _x_block_dim;
  unsigned char _y_block_dim;
  unsigned char _z_block_dim;
  unsigned char _x_size[3];
  unsigned char _y_size[3];
  unsigned char _z_size[3];
};

GLenum map_bpp_to_internal_format(const uint bits_per_pixel,
                                  const std::string &relative_path,
                                  const std::string &stem) {
  GLenum result{GL_RGBA};
  switch (bits_per_pixel) {
    case 2: {
      result = GL_COMPRESSED_RGBA_ASTC_8x8_KHR;
    }
      break;
    case 4: {
      result = GL_COMPRESSED_RGBA_ASTC_6x5_KHR;
    }
      break;
    case 8: {
      result = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
    }
      break;
    default: {
      Log::E(TAG,
             "Invalid %u bpp for ASTC texture %s/%s.astc will affect rendering.",
             bits_per_pixel,
             relative_path.c_str(),
             stem.c_str());
    }
  }

  return result;
}
}

//--------------------------------------------------------------------------------------------------

AstcTexture::AstcTexture(const std::string &relative_path,
                         const std::string &filename_stem,
                         const TextureChannels channels,
                         const uint bits_per_pixel,
                         const TexturePostCompressionFormat post_compression_format)
    : CompressedTexture(relative_path,
                        filename_stem,
                        channels,
                        post_compression_format),
      _bits_per_pixel{bits_per_pixel} {
  _internal_gl_format =
      ::map_bpp_to_internal_format(bits_per_pixel, _relative_path, _filename_stem);
}

AstcTexture AstcTexture::MirrorPostCompressed(
    const TexturePostCompressionFormat post_compression_format) const {
  std::string filename_stem{_filename_stem};
  filename_stem.append(".").append(GetFilenameExtension());
  return AstcTexture(_relative_path,
                     filename_stem,
                     _channels,
                     _bits_per_pixel,
                     post_compression_format);
}

void AstcTexture::_OnBitmapLoaded() {
  auto *astc_data_ptr = reinterpret_cast<AstcHeader *>(_bitmap_data.get());
  _width = astc_data_ptr->GetWidth();
  _height = astc_data_ptr->GetHeight();
  _image_size = astc_data_ptr->GetSize();
}

void AstcTexture::_ApplyBitmap() {
  glCompressedTexImage2D(GL_TEXTURE_2D, 0, _internal_gl_format,
                         _width, _height, 0, _image_size,
                         _bitmap_data.get() + sizeof(AstcHeader));

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

const std::string &AstcTexture::_GetInnerExtension() const {
  static const std::string kAstc("astc");
  return kAstc;
}

std::string AstcTexture::_GetInnerFormat() const {
  std::stringstream type;
  type << "ASTC " << _bits_per_pixel << "BPP";

  return type.str();
}
