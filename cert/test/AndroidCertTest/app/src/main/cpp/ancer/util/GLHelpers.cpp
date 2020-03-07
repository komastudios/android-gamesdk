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

#include "GLHelpers.hpp"

#include <ancer/System.hpp>
#include <ancer/util/Error.hpp>
#include <ancer/util/Log.hpp>
#include <ancer/util/Raii.hpp>

#include <EGL/egl.h>

namespace ancer::glh {
namespace {
constexpr Log::Tag TAG{"GLHelpers"};
}

namespace color {

hsv rgb2hsv(rgb in) {
  float min = in.r < in.g ? in.r : in.g;
  min = min < in.b ? min : in.b;

  float max = in.r > in.g ? in.r : in.g;
  max = max > in.b ? max : in.b;

  hsv out;
  out.v = max;                                // v
  const float delta = max - min;
  if (delta < 0.00001F) {
    out.s = 0;
    out.h = 0; // undefined, maybe nan?
    return out;
  }
  if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
    out.s = (delta / max);                  // s
  } else {
    // if max is 0, then r = g = b = 0
    // s = 0, h is undefined
    out.s = 0.0;
    out.h = NAN;                            // its now undefined
    return out;
  }
  if (in.r >= max)                           // > is bogus, just keeps compilor happy
    out.h = (in.g - in.b) / delta;        // between yellow & magenta
  else if (in.g >= max)
    out.h = 2.0F + (in.b - in.r) / delta;  // between cyan & yellow
  else
    out.h = 4.0F + (in.r - in.g) / delta;  // between magenta & cyan

  out.h *= 60.0F;                              // degrees

  if (out.h < 0.0)
    out.h += 360.0F;

  return out;
}

rgb hsv2rgb(hsv in) {
  rgb out;

  if (in.s <= 0.0f) { // < is bogus, just shuts up warnings
    return {in.v, in.v, in.v};
  }
  const float hh = (in.h < 360.0f ? in.h : 0.0f) / 60.0f;
  const auto i = static_cast<long>(hh);
  const float ff = hh - i;
  const float p = in.v * (1.0F - in.s);
  const float q = in.v * (1.0F - (in.s * ff));
  const float t = in.v * (1.0F - (in.s * (1.0F - ff)));

  switch (i) {
    case 0: return {in.v, t, p};
    case 1: return {q, in.v, p};
    case 2: return {p, in.v, t};
    case 3: return {p, q, in.v};
    case 4: return {t, p, in.v};
    case 5: return {in.v, p, q};
    default: return {};
  }
}
} // namespace color

bool CheckGlError(const char *func_name) {
#ifndef NDEBUG
  if (GLint err = glGetError(); err != GL_NO_ERROR) {
    // TODO(tmillican@google.com): Make this fatal and re-introduce checks in
    //  the few (if any?) places where it makes sense to continue.
    Log::E(TAG, "GL error after %s(): 0x%08x\n", func_name, err);
    return true;
  }
#endif
  return false;
}

bool CheckGlExtension(const char *extension_name) {
  GLint extension_count = 0;
  glGetIntegerv(GL_NUM_EXTENSIONS, &extension_count);
  for (int i = 0; i < extension_count; ++i) {
    const char *ext = reinterpret_cast<const char *>(glGetStringi(GL_EXTENSIONS, i));
    if (strcmp(ext, extension_name) == 0) {
      return true;
    }
  }
  return false;
}

namespace {
GLuint _CreateProgram(const char *vtx_src,
                      const char *vtx_file,
                      const char *frag_src,
                      const char *frag_file) {
  const GLuint vtx_shader = CreateShader(GL_VERTEX_SHADER, vtx_src, vtx_file);
  ANCER_AT_SCOPE([&] { glDeleteShader(vtx_shader); });
  if (!vtx_shader)
    return 0;

  const GLuint frag_shader = CreateShader(GL_FRAGMENT_SHADER, frag_src, frag_file);
  ANCER_AT_SCOPE([&] { glDeleteShader(frag_shader); });
  if (!frag_shader)
    return 0;

  const GLuint program = glCreateProgram();
  if (!program) {
    CheckGlError("glCreateProgram");
    return program;
  }
  glAttachShader(program, vtx_shader);
  glAttachShader(program, frag_shader);

  glLinkProgram(program);
  GLint linked = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if (!linked) {
    GLint info_log_len = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len);
    if (info_log_len) {
      auto *info_log = (GLchar *) malloc(static_cast<size_t>(info_log_len));
      if (info_log) {
        glGetProgramInfoLog(program, info_log_len, nullptr, info_log);
        FatalError(TAG, "Could not link program:\n%s\n", info_log);
        free(info_log);
      }
    }
    glDeleteProgram(program);
    FatalError(TAG, "Could not link program");
    return 0;
  }

  return program;
}
}

GLuint CreateProgramFromFiles(const char *vtx_file, const char *frag_file) {
  auto vtx_src = LoadText(vtx_file);
  auto frag_src = LoadText(frag_file);
  return _CreateProgram(vtx_src.c_str(), vtx_file, frag_src.c_str(), frag_file);
}

GLuint CreateProgramSrc(const char *vtx_src, const char *frag_src) {
  return _CreateProgram(vtx_src, nullptr, frag_src, nullptr);
}

GLuint CreateShader(GLenum shader_type, const char *src, const char *file) {
  GLuint shader = glCreateShader(shader_type);
  if (!shader) {
    CheckGlError("glCreateShader");
    return 0;
  }
  glShaderSource(shader, 1, &src, nullptr);

  GLint compiled = GL_FALSE;
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLint info_log_len = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_len);
    if (info_log_len > 0) {
      auto *info_log = (GLchar *) malloc(static_cast<size_t>(info_log_len));
      if (info_log) {
        glGetShaderInfoLog(shader, info_log_len, nullptr, info_log);
        // TODO: Need to review if this should be fatal or not.
        if (file) {
          Log::E(
              TAG, "Could not compile %s shader (file: %s):\n%s\n",
              shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment",
              file,
              info_log);
        } else {
          Log::E(
              TAG, "Could not compile %s shader:\n%s\n",
              shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment",
              info_log);
        }
        free(info_log);
      }
    }
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

glm::mat4 Ortho2d(float left, float top, float right, float bottom) {
  // this was cribbed from elsewhere because glm's ortho function proved hard to use
  const float z_near = -1.0F;
  const float z_far = 1.0F;
  const float inv_z = 1.0F / (z_far - z_near);
  const float inv_y = 1.0F / (-top + bottom);
  const float inv_x = 1.0F / (right - left);

  glm::mat4 result;
  result[0] = glm::vec4(2 * inv_x, 0, 0, 0);
  result[1] = glm::vec4(0, 2 * inv_y, 0, 0);
  result[2] = glm::vec4(0, 0, -2 * inv_z, 0);
  result[3] = glm::vec4(
      -(right + left) * inv_x,
      (top + bottom) * inv_y,
      -(z_far + z_near) * inv_z,
      1
  );

  return result * glm::scale(glm::mat4(1), glm::vec3(1, -1, 1));
}

std::vector<std::string> GetGlExtensions() {
  std::vector<std::string> extensions;

  GLint count = 0;
  glGetIntegerv(GL_NUM_EXTENSIONS, &count);
  for (GLint i = 0; i < count; ++i) {
    const GLubyte *s = glGetStringi(GL_EXTENSIONS, i);
    extensions.push_back(reinterpret_cast<const char *>(s));
  }

  return extensions;
}

std::vector<std::string> GetEglExtensions() {
  std::vector<std::string> extensions;

  const EGLDisplay display = eglGetCurrentDisplay();
  const char *s = eglQueryString(display, EGL_EXTENSIONS);
  const size_t s_len = strlen(s);

  size_t begin = 0;
  for (size_t i = 0; i < s_len + 1; ++i) {
    if (isspace(s[i]) || s[i] == '\0') {
      const std::string str(s + begin, i - begin);
      begin = i + 1;
      if (!str.empty()) {
        extensions.push_back(str);
      }
    }
  }

  return extensions;
}

bool CheckEglExtension(const std::string &name) {
  const auto exts = GetEglExtensions();
  return std::find(exts.begin(), exts.end(), name) != exts.end();
}

bool IsExtensionSupported(const char *gl_extension) {
  auto supported{false};
  auto index{-1};

  int number_of_extensions;
  glGetIntegerv(GL_NUM_EXTENSIONS, &number_of_extensions);

  while (not supported && ++index < number_of_extensions) {
    const auto *ccc = glGetStringi(GL_EXTENSIONS, index);
    if (strcmp((const char *) ccc, gl_extension) == 0) {
      supported = true; // The extension is supported by our hardware and driver
    }
  }

  return supported;
}

TextureHandleRef LoadTexture2D(const std::string &filename) {
  int texWidth = 0, texHeight = 0;
  bool hasAlpha = false;
  GLuint textureId =
      ancer::LoadTexture(filename.c_str(), &texWidth, &texHeight, &hasAlpha, GL_TEXTURE_2D);
  return std::make_shared<TextureHandle>(textureId, GL_TEXTURE_2D, texWidth, texHeight);
}

TextureHandleRef LoadTextureCube(const std::array<std::string, 6> &faces) {
  unsigned int textureId;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

  int width, height;
  for (unsigned int i = 0; i < faces.size(); i++) {
    int texWidth = 0, texHeight = 0;
    bool hasAlpha = false;
    GLuint faceTextureId = ancer::LoadTexture(faces[i].c_str(),
                                              &texWidth, &texHeight, &hasAlpha,
                                              GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
    if (faceTextureId == 0) {
      ancer::Log::E(TAG,
                    "[LoadTextureCube] - unable to load cubemap face (%d) path: %s",
                    i,
                    faces[i].c_str());
      glDeleteTextures(1, &textureId);
      return nullptr;
    }
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // note, sampling mipmap lods requires glEnable(GL_TEXTURE_CUBEMAP_SEAMLESS)
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  return std::make_shared<TextureHandle>(textureId, GL_TEXTURE_CUBE_MAP, width, height);
}

TextureHandleRef LoadTextureCube(const std::string &folder, const std::string &ext) {
  return LoadTextureCube({
                             folder + "/right" + ext,
                             folder + "/left" + ext,
                             folder + "/top" + ext,
                             folder + "/bottom" + ext,
                             folder + "/front" + ext,
                             folder + "/back" + ext,
                         });
}

}  // namespace ancer::glh