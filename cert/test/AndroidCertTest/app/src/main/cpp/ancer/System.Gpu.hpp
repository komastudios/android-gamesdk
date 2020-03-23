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

#ifndef _SYSTEM_GPU_HPP
#define _SYSTEM_GPU_HPP

#include <GLES3/gl32.h>

namespace ancer {
class FpsCalculator;
}

namespace ancer {

/**
 * Create gl shader program given the contents of the two shader files in assets/
 * returns handle to program, or 0 if program failed to load or link
 */
GLuint CreateProgram(const char *vtx_file_name, const char *frg_file_name);

/**
 * Loads a texture from application's assets/ folder. If texture loads,
 * writes width into out_width, height into out_height, and if the texture
 * has an alpha channel writes true into has_alpha. Returns the GL
 * texture id.
 */
GLuint LoadTexture(const char *file_name, int32_t *out_width = nullptr,
                   int32_t *out_height = nullptr, bool *has_alpha = nullptr);

/**
 * Configuration params for opengl contexts
 */
struct GLContextConfig {
  int red_bits = 8;
  int green_bits = 8;
  int blue_bits = 8;
  int alpha_bits = 0;
  int depth_bits = 24;
  int stencil_bits = 0;

  static GLContextConfig Default() {
    return GLContextConfig{
        8, 8, 8, 8,
        24, 0
    };
  }
};

inline bool operator==(const GLContextConfig &lhs, const GLContextConfig &rhs) {
  return lhs.red_bits==rhs.red_bits &&
      lhs.green_bits==rhs.green_bits &&
      lhs.blue_bits==rhs.blue_bits &&
      lhs.alpha_bits==rhs.alpha_bits &&
      lhs.depth_bits==rhs.depth_bits &&
      lhs.stencil_bits==rhs.stencil_bits;
}

inline std::ostream &operator<<(std::ostream &os, GLContextConfig c) {
  os << "<GLContextConfig r" << c.red_bits << "g" << c.green_bits << "b"
     << c.blue_bits << "a" << c.alpha_bits << "d" << c.depth_bits << "s" << c.stencil_bits << ">";
  return os;
}

inline std::string to_string(const GLContextConfig &c) {
  std::stringstream ss;
  ss << c;
  return ss.str();
}

/**
 * Write the fields from config into the java-hosted destination
 */
void BridgeGLContextConfiguration(GLContextConfig src_config, jobject dst_config);

/**
 * Create native GLContextConfig from java-based GLContextConfiguration
 * instance
*/
GLContextConfig BridgeGLContextConfiguration(jobject src_config);

FpsCalculator &GetFpsCalculator();

/**
 * Encodes raw color data into a compressed PNG.
 * Each pixel is grouped in four consecutive bytes, ordered RGBA.
 * `bytes` should have (4 * width * height) elements, where the first
 * (4 * width) elements are the top row of pixels in the output image.
 */
std::vector<unsigned char> PNGEncodeRGBABytes(unsigned int width, unsigned int height,
                                              const std::vector<unsigned char> &bytes);

/**
 * Convert byte array into Base64-encoded string.
 * `length` is the number of bytes (given as an int for compatibility
 * with the Java libraries that are used).
 */
std::string Base64EncodeBytes(const unsigned char *bytes, int length);

}

#endif  // _SYSTEM_GPU_HPP
