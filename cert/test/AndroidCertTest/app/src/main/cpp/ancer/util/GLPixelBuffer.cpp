/*
 * Copyright 2019 The Android Open Source Project
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

#include "GLPixelBuffer.hpp"

#include <GLES/gl.h>

#include <ancer/System.hpp>
#include <ancer/util/Error.hpp>
#include <ancer/util/Log.hpp>

namespace ancer {
namespace {
constexpr Log::Tag TAG{"GLPixelBuffer"};
}

GLPixelBuffer::GLPixelBuffer() {
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  _width = static_cast<unsigned int>(viewport[2]);
  _height = static_cast<unsigned int>(viewport[3]);

  if (_width == 0 || _height == 0) {
    FatalError(TAG,
               "cannot create GLPixelBuffer with width or height of 0."
               " (Hint: the viewport might not be set, consider "
               "initializing just before the first draw call.)");
  }

  GLint type;
  glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &type);
  _type = static_cast<GLenum>(type);

  GLint format;
  glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &format);
  _format = static_cast<GLenum>(format);

  switch (_format) {
    case GL_RGB:
      _components = 3;
      break;
    case GL_RGBA:
      _components = 4;
      break;
    default:
      FatalError(TAG, "unsupported pixel format");
  }

  if (_type != GL_UNSIGNED_BYTE) {
    FatalError(TAG, "unsupported pixel type");
  }

  const auto size = static_cast<size_t>(_width * _height * _components);
  _pixels = (GLubyte*)malloc(size);
  if (!_pixels) {
    FatalError(TAG, "Failed to allocate pixel buffer");
  }
}

GLPixelBuffer::~GLPixelBuffer() { free(_pixels); }

unsigned int GLPixelBuffer::Width() const { return _width; }

unsigned int GLPixelBuffer::Height() const { return _height; }

unsigned int GLPixelBuffer::Components() const { return _components; }

glm::u8vec4 GLPixelBuffer::ReadPixel(int x, int y) const {
  const int idx = (y * _width + x) * _components;

  glm::u8vec4 color;
  color.r = _pixels[idx];
  color.g = _pixels[idx + 1];
  color.b = _pixels[idx + 2];
  color.a = 255;

  if (_components == 4) {
    color.a = _pixels[idx + 3];
  }

  return color;
}

void GLPixelBuffer::CopyFromFramebuffer() {
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  const int width = static_cast<int>(viewport[2]);
  const int height = static_cast<int>(viewport[3]);

  if (width != _width || height != _height) {
    FatalError(TAG,
               "current viewport dimensions to not match pixel buffer "
               "dimensions");
  }

  glReadPixels(0, 0, _width, _height, _format, _type, _pixels);
}

std::vector<unsigned char> GLPixelBuffer::RGBABytes(bool flipped) const {
  std::vector<unsigned char> bytes(4 * _width * _height);

  const auto row_length = static_cast<size_t>(4 * _width);

  size_t offset = flipped ? static_cast<size_t>(_height - 1) * row_length : 0;

  for (int y = 0; y < _height; ++y) {
    for (int x = 0; x < _width; ++x) {
      const auto color = ReadPixel(x, y);
      bytes[offset] = static_cast<unsigned char>(color.r);
      bytes[offset + 1] = static_cast<unsigned char>(color.g);
      bytes[offset + 2] = static_cast<unsigned char>(color.b);
      bytes[offset + 3] = static_cast<unsigned char>(color.a);
      offset += 4;
    }
    if (flipped) {
      offset -= 2 * row_length;
    }
  }

  return bytes;
}

std::vector<unsigned char> GLPixelBuffer::ToPNG() const {
  return PNGEncodeRGBABytes(_width, _height, RGBABytes(true));
}

}  // namespace ancer
