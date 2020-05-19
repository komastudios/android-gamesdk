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

#include <ancer/System.Asset.hpp>
#include <ancer/util/Log.hpp>

using namespace ancer;

namespace {
Log::Tag TAG{"TextureMetadata"};
}

//--------------------------------------------------------------------------------------------------

TextureMetadata::TextureMetadata(std::string relative_path, std::string filename_stem)
    :
    _relative_path{std::move(relative_path)},
    _filename_stem{std::move(filename_stem)},
    _internal_format{GL_RGBA},
    _width{0},
    _height{0},
    _has_alpha{false},
    _bitmap_data{nullptr},
    _mem_size{0},
    _file_size{0},
    _load_time_in_millis{} {}

TextureMetadata::~TextureMetadata() {
  _ReleaseBitmapData();
}

const std::string &TextureMetadata::GetRelativePath() const {
  return _relative_path;
}

const std::string &TextureMetadata::GetFilenameStem() const {
  return _filename_stem;
}

GLenum TextureMetadata::GetInternalFormat() const {
  return _internal_format;
}

int32_t TextureMetadata::GetWidth() const {
  return _width;
}

int32_t TextureMetadata::GetHeight() const {
  return _height;
}

size_t TextureMetadata::GetSizeAtRest() const {
  return _file_size;
}

size_t TextureMetadata::GetSizeInMemory() const {
  return _mem_size;
}

bool TextureMetadata::HasAlpha() const {
  return _has_alpha;
}

Milliseconds::rep TextureMetadata::GetLoadTimeInMillis() const {
  return _load_time_in_millis.count();
}

bool TextureMetadata::Load() {
  assert(_bitmap_data == nullptr);

  auto start_time = SteadyClock::now();
  _Load();
  _load_time_in_millis = duration_cast<Milliseconds>(SteadyClock::now() - start_time);

  return _bitmap_data != nullptr;
}

void TextureMetadata::ApplyBitmap(const GLuint texture_id) {
  glBindTexture(GL_TEXTURE_2D, texture_id);
  _ApplyBitmap();
  _ReleaseBitmapData();
}

std::string TextureMetadata::ToString() const {
  std::string full_path(_relative_path);
  if (not _relative_path.empty()) {
    full_path.append("/");
  }
  full_path.append(_filename_stem).append(".").append(GetFilenameExtension());

  return full_path;
}

void TextureMetadata::_ReleaseBitmapData() {
  _bitmap_data = nullptr;
  _mem_size = 0;
}

EncodedTextureMetadata::EncodedTextureMetadata(const std::string &relative_path,
                                               const std::string &filename_stem)
    : TextureMetadata(relative_path, filename_stem) {}

CompressedTextureMetadata::CompressedTextureMetadata(
    const std::string &relative_path,
    const std::string &filename_stem,
    const TexturePostCompressionFormat post_compression_format)
    : TextureMetadata(relative_path, filename_stem),
      _post_compression_format{post_compression_format},
      _image_size{0} {
}

void CompressedTextureMetadata::_Load() {
  Asset asset{ToString(), AASSET_MODE_BUFFER};

  if (asset) {
    _file_size = asset.GetLength();
    _bitmap_data = std::unique_ptr<u_char>(new u_char[_file_size]);
    _mem_size = _file_size;

    if (asset.Read(_bitmap_data.get(), _file_size) == _file_size) {
      _OnBitmapLoaded();
    } else {
      Log::E(TAG, "Reading error in asset %s", ToString().c_str());
    }
  }
}

TexturePostCompressionFormat CompressedTextureMetadata::GetPostCompressionFormat() const {
  return _post_compression_format;
}

const std::string &CompressedTextureMetadata::GetFilenameExtension() const {
  static const std::string extension_by_post_compression_format[] = {"", "gz", "lz4"};

  return _post_compression_format == TexturePostCompressionFormat::NONE
         ?
         _GetInnerExtension()
         : extension_by_post_compression_format[static_cast<std::size_t>(_post_compression_format)];
}
