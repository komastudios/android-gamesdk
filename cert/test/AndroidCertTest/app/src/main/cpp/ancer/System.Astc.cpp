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

#include "System.Astc.hpp"

#include <android/asset_manager_jni.h>
#include <GLES2/gl2ext.h>

#include <ancer/System.Asset.hpp>
#include <ancer/System.Gpu.hpp>
#include <ancer/util/JNIHelpers.hpp>
#include <ancer/util/Log.hpp>

using namespace ancer;

namespace {
Log::Tag TAG{"SystemAstc"};
}

//==================================================================================================

namespace {
/* ASTC header declaration. */
struct AstcHeader {
  unsigned char magic[4];
  unsigned char blockdim_x;
  unsigned char blockdim_y;
  unsigned char blockdim_z;
  unsigned char xsize[3];   /* x-size = xsize[0] + xsize[1] + xsize[2] */
  unsigned char ysize[3];   /* x-size, y-size and z-size are given in texels */
  unsigned char zsize[3];   /* block count is inferred */
};
}

AstcTextureMetadata::AstcTextureMetadata(const std::string &relative_path,
                                         const std::string &filename_stem,
                                         const TexturePostCompressionFormat post_compression_format)
    : CompressedTextureMetadata(relative_path, filename_stem, post_compression_format) {}

void AstcTextureMetadata::_Load() {
  Asset asset{ToString(), AASSET_MODE_BUFFER};

  if (asset) {
    _file_size = asset.GetLength();
    _bitmap_data = std::unique_ptr<u_char>(new u_char[_file_size]);
    _mem_size = _file_size;

    if (asset.Read(_bitmap_data.get(), _file_size) == _file_size) {
      auto *astc_data_ptr = reinterpret_cast<AstcHeader *>(_bitmap_data.get());
      /* Merge x,y-sizes from 3 chars into one integer value. */
      _width =
          astc_data_ptr->xsize[0] + (astc_data_ptr->xsize[1] << 8)
              + (astc_data_ptr->xsize[2] << 16);
      _height =
          astc_data_ptr->ysize[0] + (astc_data_ptr->ysize[1] << 8)
              + (astc_data_ptr->ysize[2] << 16);
    } else {
      Log::E(TAG, "Reading error in asset %s", ToString().c_str());
    }
  }
}

void AstcTextureMetadata::_ApplyBitmap() {
  unsigned int n_bytes_to_read = 0;

  /* Number of blocks in the x, y and z direction. */
  int xblocks = 0;
  int yblocks = 0;
  int zblocks = 0;

  /* Number of bytes for each dimension. */
  int xsize = _width;
  int ysize = _height;
  int zsize = 0;

  auto *astc_data_ptr = reinterpret_cast<AstcHeader *>(_bitmap_data.get());

  /* Merge x,y,z-sizes from 3 chars into one integer value. */
  zsize =
      astc_data_ptr->zsize[0] + (astc_data_ptr->zsize[1] << 8) + (astc_data_ptr->zsize[2] << 16);

  /* Compute number of blocks in each direction. */
  xblocks = (xsize + astc_data_ptr->blockdim_x - 1) / astc_data_ptr->blockdim_x;
  yblocks = (ysize + astc_data_ptr->blockdim_y - 1) / astc_data_ptr->blockdim_y;
  zblocks = (zsize + astc_data_ptr->blockdim_z - 1) / astc_data_ptr->blockdim_z;

  /* Each block is encoded on 16 bytes, so calculate total compressed image data size. */
  n_bytes_to_read = xblocks * yblocks * zblocks << 4;

  glCompressedTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_COMPRESSED_RGBA_ASTC_8x8_KHR,
                         _width,
                         _height,
                         0,
                         n_bytes_to_read,
                         (const GLvoid *) &astc_data_ptr[1]);
  // TODO(dagum): _bitmap_data.get() + sizeof(AstcHeader)

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

const std::string &AstcTextureMetadata::_GetInnerExtension() const {
  static const std::string kAstc("astc");
  return kAstc;
}
