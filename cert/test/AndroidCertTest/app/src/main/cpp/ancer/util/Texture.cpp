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

#include "Texture.hpp"

#include <sstream>

#include <ancer/System.Asset.hpp>
#include <ancer/util/Gzip.hpp>
#include <ancer/util/Log.hpp>

using namespace ancer;

namespace {
Log::Tag TAG{"Texture"};
}

//--------------------------------------------------------------------------------------------------

std::string ancer::ToString(const TextureChannels channels) {
  return channels == TextureChannels::RGB ? "RGB" : "RGBA";
}

//--------------------------------------------------------------------------------------------------

Texture::Texture(std::string relative_path,
                 std::string filename_stem,
                 const TextureChannels channels)
    :
    _relative_path{std::move(relative_path)},
    _filename_stem{std::move(filename_stem)},
    _channels{channels},
    _internal_gl_format{GL_RGBA},
    _width{0},
    _height{0},
    _has_alpha{false},
    _bitmap_data{nullptr},
    _mem_size{0},
    _file_size{0},
    _load_time_in_millis{} {}

const std::string &Texture::GetRelativePath() const {
  return _relative_path;
}

const std::string &Texture::GetFilenameStem() const {
  return _filename_stem;
}

TextureChannels Texture::GetChannels() const {
  return _channels;
}

GLenum Texture::GetInternalFormat() const {
  return _internal_gl_format;
}

int32_t Texture::GetWidth() const {
  return _width;
}

int32_t Texture::GetHeight() const {
  return _height;
}

size_t Texture::GetSizeAtRest() const {
  return _file_size;
}

size_t Texture::GetSizeInMemory() const {
  return _mem_size;
}

bool Texture::HasAlpha() const {
  return _has_alpha;
}

Milliseconds::rep Texture::GetLoadTimeInMillis() const {
  return _load_time_in_millis.count();
}

Milliseconds::rep Texture::GetGpuTransferTimeInMillis() const {
  return _gpu_trx_time_in_millis.count();
}

bool Texture::Load() {
  assert(_bitmap_data == nullptr);

  auto start_time = SteadyClock::now();
  _Load();
  _load_time_in_millis = duration_cast<Milliseconds>(SteadyClock::now() - start_time);

  return _bitmap_data != nullptr;
}

void Texture::ApplyBitmap(const GLuint texture_id) {
  glBindTexture(GL_TEXTURE_2D, texture_id);

  auto start_time = SteadyClock::now();
  _ApplyBitmap();
  _gpu_trx_time_in_millis = duration_cast<Milliseconds>(SteadyClock::now() - start_time);

  _bitmap_data = nullptr;
}

std::string Texture::Str() const {
  std::string full_path(GetRelativePath());
  if (not full_path.empty()) {
    full_path.append("/");
  }
  full_path.append(GetFilenameStem()).append(".").append(GetFilenameExtension());

  return full_path;
}

//--------------------------------------------------------------------------------------------------

EncodedTexture::EncodedTexture(const std::string &relative_path,
                               const std::string &filename_stem, const TextureChannels channels)
    : Texture(relative_path, filename_stem, channels) {}

//--------------------------------------------------------------------------------------------------

CompressedTexture::CompressedTexture(
    const std::string &relative_path,
    const std::string &filename_stem, const TextureChannels channels,
    const TexturePostCompressionFormat post_compression_format)
    : Texture(relative_path, filename_stem, channels),
      _post_compression_format{post_compression_format},
      _image_size{0} {
}

TexturePostCompressionFormat CompressedTexture::GetPostCompressionFormat() const {
  return _post_compression_format;
}

const std::string &CompressedTexture::GetFilenameExtension() const {
  static const std::string extension_by_post_compression_format[] = {"", "gzip", "lz4"};

  return _post_compression_format == TexturePostCompressionFormat::NONE
         ?
         _GetInnerExtension()
         : extension_by_post_compression_format[static_cast<std::size_t>(_post_compression_format)];
}

std::string CompressedTexture::GetFormat() const {
  static const std::string format_by_post_compression[] = {"", "Deflated", "LZ4"};

  if (_post_compression_format == TexturePostCompressionFormat::NONE) {
    return _GetInnerFormat();
  } else {
    std::stringstream string_stream;
    string_stream << format_by_post_compression[static_cast<std::size_t>(_post_compression_format)]
                  << " (" << _GetInnerFormat() << ')';

    return string_stream.str();
  }
}

void CompressedTexture::_Load() {
  switch (_post_compression_format) {
    case TexturePostCompressionFormat::NONE: {
      if (Asset asset{Str(), AASSET_MODE_STREAMING}) {
        _file_size = static_cast<size_t>(asset.GetLength());
        u_char *bitmap_data = new u_char[_file_size];
        _bitmap_data = std::unique_ptr<u_char>(bitmap_data);
        _mem_size = _file_size;

        if (asset.Read(bitmap_data, _file_size) == _file_size) {
          _OnBitmapLoaded();
        } else {
          Log::E(TAG, "%s reading error", Str().c_str());
        }
      }
    }
      break;

    case TexturePostCompressionFormat::GZIP: {
      if ((_bitmap_data = gzip::InflateAsset(Str(), &_file_size, &_mem_size))) {
        _OnBitmapLoaded();
      } else {
        Log::E(TAG, "%s inflate error", Str().c_str());
      }
    }
      break;

    case TexturePostCompressionFormat::LZ4:
      // TODO(dagum): implement lz4 decompression
      break;
  }
}
