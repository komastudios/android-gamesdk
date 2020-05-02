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

#include <utility>

#include <ancer/util/Log.hpp>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

using namespace ancer;

namespace {
Log::Tag TAG{"TextureAstc"};
}

//==================================================================================================

namespace {
/* ASTC header declaration. */
struct AstcHeader {
  unsigned char magic[4];
  unsigned char blockdim_x;
  unsigned char blockdim_y;
  unsigned char blockdim_z;
  unsigned char x_size[3];   /* x-size = x_size[0] + x_size[1] + x_size[2] */
  unsigned char y_size[3];   /* x-size, y-size and z-size are given in texels */
  unsigned char z_size[3];   /* block count is inferred */
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

AstcTextureMetadata::AstcTextureMetadata(const std::string &relative_path,
                                         const std::string &filename_stem,
                                         const uint bits_per_pixel,
                                         const TexturePostCompressionFormat post_compression_format)
    : CompressedTextureMetadata(relative_path,
                                filename_stem,
                                post_compression_format),
      _bits_per_pixel{bits_per_pixel} {
  _internal_format = ::map_bpp_to_internal_format(bits_per_pixel, _relative_path, _filename_stem);
}

AstcTextureMetadata &&AstcTextureMetadata::MirrorPostCompressed(
    const TexturePostCompressionFormat post_compression_format) const {
  return std::move(AstcTextureMetadata(_relative_path,
                                       _filename_stem,
                                       _bits_per_pixel,
                                       post_compression_format));
}

void AstcTextureMetadata::_OnBitmapLoaded() {
  auto *astc_data_ptr = reinterpret_cast<AstcHeader *>(_bitmap_data.get());
  /* Merge x,y-sizes from 3 chars into one integer value. */
  _width =
      astc_data_ptr->x_size[0] + (astc_data_ptr->x_size[1] << 8) + (astc_data_ptr->x_size[2] << 16);
  _height =
      astc_data_ptr->y_size[0] + (astc_data_ptr->y_size[1] << 8) + (astc_data_ptr->y_size[2] << 16);

  /* Number of bytes for each dimension. */
  int x_size{_width};
  int y_size{_height};
  /* Merge x,y,z-sizes from 3 chars into one integer value. */
  int z_size{
      astc_data_ptr->z_size[0] + (astc_data_ptr->z_size[1] << 8) + (astc_data_ptr->z_size[2] << 16)};

  /* Compute number of blocks in each direction. */
  /* Number of blocks in the x, y and z direction. */
  int x_blocks{(x_size + astc_data_ptr->blockdim_x - 1) / astc_data_ptr->blockdim_x};
  int y_blocks{(y_size + astc_data_ptr->blockdim_y - 1) / astc_data_ptr->blockdim_y};
  int z_blocks{(z_size + astc_data_ptr->blockdim_z - 1) / astc_data_ptr->blockdim_z};

  /* Each block is encoded on 16 bytes, so calculate total compressed image data size. */
  _image_size = x_blocks * y_blocks * z_blocks << 4;
}

void AstcTextureMetadata::_ApplyBitmap() {
  glCompressedTexImage2D(GL_TEXTURE_2D, 0, _internal_format,
                         _width, _height, 0, _image_size,
                         _bitmap_data.get() + sizeof(AstcHeader));

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

const std::string &AstcTextureMetadata::_GetInnerExtension() const {
  static const std::string kAstc("astc");
  return kAstc;
}
