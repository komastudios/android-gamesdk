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
     * \brief Class to extract information from the ETC headers of compressed textures.
     */
class ETCHeader {
 public:
  /**
   * \brief Extract the ETC header information from a loaded ETC compressed texture.
   */
  ETCHeader(unsigned char *data) {
    /*
     * Load from a ETC compressed pkm image file.
     * First 6 bytes are the name of the file format and version/packing type.
     * Bytes 7 and 8 are blank.
     */
    /* Beware endianess issues with most/least significant bits of the height and width. */
    paddedWidthMSB = data[8];
    paddedWidthLSB = data[9];
    paddedHeightMSB = data[10];
    paddedHeightLSB = data[11];
    widthMSB = data[12];
    widthLSB = data[13];
    heightMSB = data[14];
    heightLSB = data[15];
  }

  ETCHeader(const ETCHeader &) = default;

  /**
   * \brief The width of the original texture.
   *
   * The width of a compressed texture is padded to 4x4 blocks by the compression method.
   * The resulting width of the compressed texture may therefore be larger if it's original width was not a multiple of 4.
   * By using the unpadded width, the original texture can be drawn.
   * \return The width of the original texture without padding.
   */
  unsigned short getWidth(void) {
    return (widthMSB << 8) | widthLSB;
  }

  /**
   * \brief The height of the original texture.
   *
   * The height of a compressed texture is padded to 4x4 blocks by the compression method.
   * The resulting height of the compressed texture may therefore be larger if it's original height was not a multiple of 4.
   * By using the unpadded height, the original texture can be drawn.
   * \return The height of the original texture without padding.
   */
  unsigned short getHeight(void) {
    return (heightMSB << 8) | heightLSB;
  }

  /**
   * \brief The width of the compressed texture with the padding added.
   *
   * The width of a compressed texture is padded to 4x4 blocks by the compression method.
   * The resulting width of the compressed texture may therefore be larger if it's original width was not a multiple of 4.
   * \return The width of the compressed texture with padding included.
   */
  unsigned short getPaddedWidth(void) {
    return (paddedWidthMSB << 8) | paddedWidthLSB;
  }

  /**
   * \brief The height of the compressed texture with the padding added.
   *
   * The height of a compressed texture is padded to 4x4 blocks by the compression method.
   * The resulting height of the compressed texture may therefore be larger if it's original height was not a multiple of 4.
   * \return The height of the compressed texture with padding included.
   */
  unsigned short getPaddedHeight(void) {
    return (paddedHeightMSB << 8) | paddedHeightLSB;
  }

  /**
   * \brief The size of the compressed texture with the padding added.
   *
   * The size is computed as padded width multiplied by padded height.
   * \param[in] internalFormat The internal format of the compressed texture.
   * \return The size of the compressed texture with padding included.
   */
  GLsizei getSize(GLenum internalFormat) {
    if (internalFormat != GL_COMPRESSED_RG11_EAC && internalFormat != GL_COMPRESSED_SIGNED_RG11_EAC
        &&
            internalFormat != GL_COMPRESSED_RGBA8_ETC2_EAC
        && internalFormat != GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC) {
      return (getPaddedWidth() * getPaddedHeight()) >> 1;
    } else {
      return (getPaddedWidth() * getPaddedHeight());
    }
  }

 private:
  unsigned char buffer[8];
  unsigned char paddedWidthMSB;
  unsigned char paddedWidthLSB;
  unsigned char paddedHeightMSB;
  unsigned char paddedHeightLSB;
  unsigned char widthMSB;
  unsigned char widthLSB;
  unsigned char heightMSB;
  unsigned char heightLSB;
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
/* PKM file consists of a header with information about image (stored in 16 first bits) and image data. */
  ETCHeader header{*(reinterpret_cast<ETCHeader *>(_bitmap_data.get()))};

  _width = header.getWidth();
  _height = header.getHeight();
  _image_size = header.getSize(_internal_format);
}

void Etc2Texture::_ApplyBitmap() {
  glCompressedTexImage2D(GL_TEXTURE_2D, 0, _internal_format,
                         _width, _height, 0, _image_size,
                         _bitmap_data.get() + sizeof(ETCHeader));

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

const std::string &Etc2Texture::_GetInnerExtension() const {
  static const std::string kPkm("pkm");
  return kPkm;
}
