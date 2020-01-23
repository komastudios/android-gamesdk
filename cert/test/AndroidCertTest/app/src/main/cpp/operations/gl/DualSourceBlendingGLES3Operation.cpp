/*
 * Copyright 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <memory>
#include <optional>
#include <vector>

#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/util/GLPixelBuffer.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;

// PURPOSE: To test the availability of dual source blending extensions, as well
// to perform a variety of dual source blending operations to verify that the
// rendered result matches the value expected by mathematical computation of the
// equation given in the OpenGL spec.

// Dual source blending allows a shader to write out an additional color per
// fragment, which can then be used in OpenGL's blend operation:
//
//   COLOR = BLEND_MODE(SRC_FACTOR * SRC_COLOR, DST_FACTOR * DST_COLOR),
//
// where BLEND_MODE is either GL_ADD or GL_SUBTRACT. Essentially, it blends
// the existing color (destination or "DST") with the fragment shader's primary
// output color (source or "SRC"), multiplying both quantities by a configurable
// factor.

//==============================================================================

namespace {

constexpr Log::Tag TAG{"DualSourceBlendingGLES3Operation"};

//------------------------------------------------------------------------------

// "reference_screenshot_mode" is only used when creating a reference
// screenshot for a specific test variant. This should be false during normal
// use (that is, when gathering data from devices).
struct configuration {
  bool reference_screenshot_mode;
};

JSON_CONVERTER(configuration) { JSON_OPTVAR(reference_screenshot_mode); }

//------------------------------------------------------------------------------

struct feature_report_datum {
  bool gl_ext_blend_func_available;
  bool arb_blend_func_available;
  bool shaders_compiled;
};

void WriteDatum(report_writers::Struct w, const feature_report_datum& d) {
  ADD_DATUM_MEMBER(w, d, gl_ext_blend_func_available);
  ADD_DATUM_MEMBER(w, d, arb_blend_func_available);
  ADD_DATUM_MEMBER(w, d, shaders_compiled);
}

struct datum {
  std::string name;
  bool success;
  int epsilon;
  int max_variance;
};

void WriteDatum(report_writers::Struct w, const datum& d) {
  ADD_DATUM_MEMBER(w, d, name);
  ADD_DATUM_MEMBER(w, d, success);
  ADD_DATUM_MEMBER(w, d, epsilon);
  ADD_DATUM_MEMBER(w, d, max_variance);
}

struct sampled_image_datum {
  std::string base64_image;
  std::string blend_mode;
  std::string src_blend_factor;
  std::string dst_blend_factor;
  std::string dst_color;
  std::string src_1_color;
  std::string src_palette_last_color;
};

void WriteDatum(report_writers::Struct w, const sampled_image_datum& d) {
  ADD_DATUM_MEMBER(w, d, base64_image);
  ADD_DATUM_MEMBER(w, d, blend_mode);
  ADD_DATUM_MEMBER(w, d, src_blend_factor);
  ADD_DATUM_MEMBER(w, d, dst_blend_factor);
  ADD_DATUM_MEMBER(w, d, dst_color);
  ADD_DATUM_MEMBER(w, d, src_1_color);
  ADD_DATUM_MEMBER(w, d, src_palette_last_color);
}
}  // end anonymous namespace

//==============================================================================

namespace {

class ShaderProgram {
 public:
  ShaderProgram() {
    _program = CreateProgram(
        "Shaders/DualSourceBlendingGLES3Operation/implicit_quad.vsh",
        "Shaders/DualSourceBlendingGLES3Operation/dual_source.fsh");

    if (!_program) {
      return;
    }

    _valid = true;

    getUniformLocations();
    setUniformDefaults();
  }

  ~ShaderProgram() { glDeleteProgram(_program); }

  feature_report_datum GetDatum() const {
    feature_report_datum datum = {};

    datum.gl_ext_blend_func_available =
        glh::CheckGlExtension("GL_EXT_blend_func_extended");
    datum.arb_blend_func_available =
        glh::CheckGlExtension("ARB_blend_func_extended");
    datum.shaders_compiled = _valid;

    return datum;
  }

  bool Valid() const { return _valid; }

  void Bind() const { glUseProgram(_program); }

  // Be sure to call Bind() before calling any of these set methods.
  void SetModelMatrix(const glm::mat4& m) const {
    glUniformMatrix4fv(_u_model_loc, 1, GL_FALSE, &m[0][0]);
  }

  void SetColor(const glm::vec4& v) const {
    glUniform4fv(_u_color_loc, 1, &v[0]);
  }

  void SetColor1(const glm::vec4& v) const {
    glUniform4fv(_u_color1_loc, 1, &v[0]);
  }

  void DrawQuad() const { glDrawArrays(GL_TRIANGLES, 0, 6); }

 private:
  void getUniformLocations() {
    _u_model_loc = glGetUniformLocation(_program, "uModel");
    _u_color_loc = glGetUniformLocation(_program, "uColor");
    _u_color1_loc = glGetUniformLocation(_program, "uColor1");
  }

  void setUniformDefaults() {
    Bind();
    SetModelMatrix(glm::mat4(1.0F));
    SetColor(glm::vec4(1.0F));
    SetColor1(glm::vec4(1.0F));
  }

 private:
  bool _valid = false;
  GLuint _program = 0;
  GLint _u_model_loc = 0;
  GLint _u_color_loc = 0;
  GLint _u_color1_loc = 0;
};

//------------------------------------------------------------------------------

using RGBA = glm::u8vec4;

const int MAX_RGBA = 255;

glm::vec4 RGBAToVec4(const RGBA& c) {
  const float s = 1.0F / static_cast<float>(MAX_RGBA);
  return glm::vec4{
      s * static_cast<float>(c.r),
      s * static_cast<float>(c.g),
      s * static_cast<float>(c.b),
      s * static_cast<float>(c.a),
  };
}

RGBA Vec4ToRGBA(const glm::vec4& c) {
  const float s = static_cast<float>(MAX_RGBA);
  return RGBA{
      static_cast<int>(s * c.r),
      static_cast<int>(s * c.g),
      static_cast<int>(s * c.b),
      static_cast<int>(s * c.a),
  };
}

std::string RGBAToString(const RGBA& c) {
  std::string str = "(";
  str += std::to_string(c.r) + ",";
  str += std::to_string(c.g) + ",";
  str += std::to_string(c.b) + ",";
  str += std::to_string(c.a) + ")";
  return str;
}

//------------------------------------------------------------------------------

using Palette = std::vector<RGBA>;

Palette white_palette;
Palette black_palette;
Palette red_palette;
Palette green_palette;
Palette blue_palette;

void InitRedPalette(int shades) {
  const int MAX_VALUE = MAX_RGBA + 1;
  const int step = MAX_VALUE / shades;
  for (int a = 0; a < MAX_VALUE; a += step) {
    for (int c = 0; c < MAX_VALUE; c += step) {
      red_palette.push_back(RGBA{c, 0, 0, a});
    }
  }
}

void InitGreenPalette(int shades) {
  const int MAX_VALUE = MAX_RGBA + 1;
  const int step = MAX_VALUE / shades;
  for (int a = 0; a < MAX_VALUE; a += step) {
    for (int c = 0; c < MAX_VALUE; c += step) {
      green_palette.push_back(RGBA{0, c, 0, a});
    }
  }
}

void InitBluePalette(int shades) {
  const int MAX_VALUE = MAX_RGBA + 1;
  const int step = MAX_VALUE / shades;
  for (int a = 0; a < MAX_VALUE; a += step) {
    for (int c = 0; c < MAX_VALUE; c += step) {
      blue_palette.push_back(RGBA{0, 0, c, a});
    }
  }
}

void InitColorPalettes(int shades) {
  white_palette = {RGBA{MAX_RGBA, MAX_RGBA, MAX_RGBA, MAX_RGBA}};
  black_palette = {RGBA{0, 0, 0, MAX_RGBA}};

  InitRedPalette(shades);
  InitGreenPalette(shades);
  InitBluePalette(shades);
}

//------------------------------------------------------------------------------

std::string BlendModeName(GLenum mode) {
  switch (mode) {
    case GL_ADD:
      return "GL_ADD";
    case GL_SUBTRACT:
      return "GL_SUBTRACT";
    default:
      FatalError(TAG, "unhandled blend mode name");
  }
}

std::string BlendFactorName(GLenum factor) {
  switch (factor) {
    case GL_ZERO:
      return "GL_ZERO";
    case GL_ONE:
      return "GL_ONE";
    case GL_SRC_COLOR:
      return "GL_SRC_COLOR";
    case GL_ONE_MINUS_SRC_COLOR:
      return "GL_ONE_MINUS_SRC_COLOR";
    case GL_SRC_ALPHA:
      return "GL_SRC_ALPHA";
    case GL_ONE_MINUS_SRC_ALPHA:
      return "GL_ONE_MINUS_SRC_ALPHA";
    case GL_SRC1_COLOR_EXT:
      return "GL_SRC1_COLOR_EXT";
    case GL_ONE_MINUS_SRC1_COLOR_EXT:
      return "GL_ONE_MINUS_SRC1_COLOR_EXT";
    case GL_SRC1_ALPHA_EXT:
      return "GL_SRC1_ALPHA_EXT";
    case GL_ONE_MINUS_SRC1_ALPHA_EXT:
      return "GL_ONE_MINUS_SRC1_ALPHA_EXT";
    case GL_DST_COLOR:
      return "GL_DST_COLOR";
    case GL_ONE_MINUS_DST_COLOR:
      return "GL_ONE_MINUS_DST_COLOR";
    case GL_DST_ALPHA:
      return "GL_DST_ALPHA";
    case GL_ONE_MINUS_DST_ALPHA:
      return "GL_ONE_MINUS_DST_ALPHA";
    default:
      FatalError(TAG, "unhandled blend name");
  }
};
}  // end anonymous namespace

// =============================================================================

namespace {

// A Test is considered to be one permutation of a blend equation; a Test draws
// many color variants to the screen and the result is sampled. If any sample
// does not match the expected value (within some `epsilon`), the Test fails.
// With each Test we log the greatest magnitude the sample varied from the
// expected value, which is usually 0 or 1 for a device that renders the correct
// result, but if the value exceeds 1, we also produce a screenshot of the
// worst-case run of that Test.
struct Test {
  GLenum blend_mode;
  GLenum blend_s_factor;
  GLenum blend_d_factor;

  std::vector<Palette> src_palettes;
  std::vector<Palette> src_1_palettes;
  std::vector<Palette> dst_palettes;

  int epsilon = 1;

  std::string Description() const {
    return "(" + BlendFactorName(blend_s_factor) + ", " +
           BlendFactorName(blend_d_factor) + ")";
  }
};

//------------------------------------------------------------------------------

// A Test fails when the rendered image does not match the expected image;
// that is, when some pixel's RGBA values differ from the expected value by
// more than the Test's `epsilon`.
//
// In such a case, a base64 image is capture, as well as enough information
// to construct a reference image (since a single Test encompases many
// permutations of `src`, `src_1`, and `dst` colors.)
//
// `src_palette_last_color` is captured as a cheap way to tell which palette
// was being rendered (for example, `(240,0,0,255)` would signal `red_palette`).
// This allows us to avoid logging the entire 16x16 palette.
struct TestFailureCapture {
  RGBA src_palette_last_color;
  RGBA dst;
  RGBA src_1;
  std::string base64_image;

  TestFailureCapture(RGBA src_palette_last_color, RGBA dst, RGBA src_1,
                     std::string base64_image)
      : src_palette_last_color(src_palette_last_color),
        dst(dst),
        src_1(src_1),
        base64_image(base64_image) {}
};

std::optional<TestFailureCapture> InitFailure() {
  return std::make_optional<TestFailureCapture>(RGBA{}, RGBA{}, RGBA{}, "");
}

//------------------------------------------------------------------------------

struct TestResult {
  int greatest_variance;
  std::optional<TestFailureCapture> failure;
};

std::string Vec4ToString(const glm::vec4& v) {
  std::string str = "(";
  str += std::to_string(v.r) + ",";
  str += std::to_string(v.g) + ",";
  str += std::to_string(v.b) + ",";
  str += std::to_string(v.a) + ")";
  return str;
}

//------------------------------------------------------------------------------

// A Tile represents a rectangular region over which `dst`, `src`, and `src1`
// are the same for each pixel. For efficiency, many tiles are drawn during
// a single frame of a Test, though the results are measured individually.
// Thus, each Tile will have an "Expected" color, which is computed.
struct Tile {
  RGBA src;
  RGBA src1;
  RGBA dst;
  GLenum blend_mode;
  GLenum blend_s_factor;
  GLenum blend_d_factor;
  glm::vec2 clipspace_coord;

  RGBA Expected() const {
    const glm::vec4 a = GetFactor(blend_s_factor) * RGBAToVec4(src);
    const glm::vec4 b = GetFactor(blend_d_factor) * RGBAToVec4(dst);

    glm::vec4 blend_result;
    switch (blend_mode) {
      case GL_ADD:
        blend_result = a + b;
        break;
      case GL_SUBTRACT:
        blend_result = a - b;
        break;
      default:
        FatalError(TAG, "unhandled blend mode");
    }
    return Vec4ToRGBA(
        glm::clamp(blend_result, glm::vec4(0.0F), glm::vec4(1.0F)));
  }

 private:
  glm::vec4 GetFactor(GLenum factor) const {
    switch (factor) {
      case GL_ONE:
        return glm::vec4(1.0F);
      case GL_ZERO:
        return glm::vec4(0.0F);
      case GL_SRC_COLOR:
        return RGBAToVec4(src);
      case GL_ONE_MINUS_SRC_COLOR:
        return glm::vec4(1.0F) - RGBAToVec4(src);
      case GL_SRC_ALPHA:
        return glm::vec4(RGBAToVec4(src).a);
      case GL_ONE_MINUS_SRC_ALPHA:
        return glm::vec4(1.0F - RGBAToVec4(src).a);
      case GL_SRC1_COLOR_EXT:
        return RGBAToVec4(src1);
      case GL_ONE_MINUS_SRC1_COLOR_EXT:
        return glm::vec4(1.0F) - RGBAToVec4(src1);
      case GL_SRC1_ALPHA_EXT:
        return glm::vec4(RGBAToVec4(src1).a);
      case GL_ONE_MINUS_SRC1_ALPHA_EXT:
        return glm::vec4(1.0F - RGBAToVec4(src1).a);
      case GL_DST_COLOR:
        return RGBAToVec4(dst);
      case GL_ONE_MINUS_DST_COLOR:
        return glm::vec4(1.0F) - RGBAToVec4(dst);
      case GL_DST_ALPHA:
        return glm::vec4(RGBAToVec4(dst).a);
      case GL_ONE_MINUS_DST_ALPHA:
        return glm::vec4(1.0F - RGBAToVec4(dst).a);
      default:
        FatalError(TAG, "unhandled blend factor case");
    }
  }
};

//------------------------------------------------------------------------------

// Gives clipspace coordinate for the ith element of an NxN grid.
// i = 0 is the lower-left corner of the grid.
glm::vec2 GridClipspaceCoord(int i, int N) {
  const int row = i / N;
  const int col = i % N;

  glm::vec2 c = glm::vec2(0.0F);
  c.x = (1.0F / N) * (1.0F + 2.0F * col) - 1.0F;
  c.y = (1.0F / N) * (1.0F + 2.0F * row) - 1.0F;

  return c;
}

// Coming from clipspace where (0,0) is center, (1,1) is upper-right,
// going to (0,0) is lower-left, (width,height) is upper-right.
glm::ivec2 ClipspaceToPixelCoord(glm::vec2 clipspace_coord, int width,
                                 int height) {
  glm::ivec2 p;

  const auto w = static_cast<float>(width);
  const auto h = static_cast<float>(height);

  p.x = static_cast<int>(w * (clipspace_coord.x + 1.0F) / 2.0F);
  p.y = static_cast<int>(h * (clipspace_coord.y + 1.0F) / 2.0F);

  return p;
}
}  // end anonymous namespace

// =============================================================================

class DualSourceBlendingGLES3Operation : public BaseGLES3Operation {
 public:
  DualSourceBlendingGLES3Operation() = default;

  void OnGlContextReady(const GLContextConfig& ctx_config) override {
    Log::D(TAG, "GlContextReady");
    _configuration = GetConfiguration<configuration>();

    SetHeartbeatPeriod(500ms);

    _egl_context = eglGetCurrentContext();
    if (_egl_context == EGL_NO_CONTEXT) {
      FatalError(TAG, "No EGL context available");
    }

    _program = std::make_unique<ShaderProgram>();
    Report(_program->GetDatum());
    if (!_program->Valid()) {
      Stop();
      return;
    }

    InitColorPalettes(_grid_size);
    InitTests();
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);

    if (_tests.empty()) {
      Stop();
      return;
    }

    if (!_pixel_buffer) {
      _pixel_buffer = std::make_unique<GLPixelBuffer>();
    }

    if (_configuration.reference_screenshot_mode) {
      GenerateReferenceScreenshot();
      Stop();
    } else {
      RunNextTest();
    }
  }

 private:
  void InitTests() {
    struct BlendConfig {
      GLenum blend_mode;
      GLenum blend_s_factor;
      GLenum blend_d_factor;
    };

    const std::vector<BlendConfig> configs = {
        {GL_ADD, GL_ONE, GL_ZERO},

        {GL_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA},
        {GL_ADD, GL_SRC1_ALPHA_EXT, GL_ONE_MINUS_SRC1_ALPHA_EXT},
        {GL_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA_EXT},
        {GL_ADD, GL_SRC1_ALPHA_EXT, GL_ONE_MINUS_SRC_ALPHA},

        {GL_ADD, GL_ONE, GL_SRC1_COLOR_EXT},
        {GL_ADD, GL_ONE, GL_ONE_MINUS_SRC1_COLOR_EXT},

        {GL_ADD, GL_SRC_ALPHA, GL_SRC1_COLOR_EXT},
        {GL_ADD, GL_SRC1_COLOR_EXT, GL_SRC1_COLOR_EXT},
    };

    for (const BlendConfig& config : configs) {
      Test t;
      t.blend_mode = config.blend_mode;
      t.blend_s_factor = config.blend_s_factor;
      t.blend_d_factor = config.blend_d_factor;
      t.src_palettes = {
          red_palette,
          blue_palette,
          green_palette,
      };
      t.src_1_palettes = {
          red_palette,
          green_palette,
          blue_palette,
      };
      t.dst_palettes = {
          black_palette,
          white_palette,
      };
      t.epsilon = 1;
      _tests.push_back(t);
    }
  }

  // Modify to generate a reference image on a device that is known to produce
  // correct results. A reference image may need to be generated when a
  // particular device fails a Test and yields an image of the screen when
  // it failed.
  // TODO (baxtermichael@google.com): Move this to the reporting suite.
  void GenerateReferenceScreenshot() {
    const RGBA dst{0, 0, 0, 255};
    const RGBA src_1{0, 0, 0, 0};
    Palette& src_palette = red_palette;
    const GLenum blend_mode = GL_ADD;
    const GLenum blend_s_factor = GL_ONE;
    const GLenum blend_d_factor = GL_ZERO;
    const RGBA src_palette_last_color = src_palette[src_palette.size() - 1];

    DrawTestFrame(dst, src_palette, src_1, blend_mode, blend_s_factor,
                  blend_d_factor);

    _pixel_buffer->CopyFromFramebuffer();

    const auto png_data = _pixel_buffer->ToPNG();
    const auto base64_image =
        Base64EncodeBytes(png_data.data(), static_cast<int>(png_data.size()));

    sampled_image_datum sample = {};
    sample.src_1_color = RGBAToString(src_1);
    sample.dst_color = RGBAToString(dst);
    sample.src_blend_factor = BlendFactorName(blend_s_factor);
    sample.dst_blend_factor = BlendFactorName(blend_d_factor);
    sample.src_palette_last_color = RGBAToString(src_palette_last_color);
    sample.base64_image = base64_image;
    Report(sample);
  }

  void RunNextTest() {
    Test test = _tests[_tests.size() - 1];
    _tests.pop_back();

    const TestResult result = RunTest(test);

    datum d = {};
    d.name = test.Description();
    d.epsilon = test.epsilon;
    d.max_variance = result.greatest_variance;
    d.success = d.max_variance <= d.epsilon;
    Report(d);

    if (!d.success && result.failure) {
      const auto failure = result.failure.value();
      sampled_image_datum sample = {};
      sample.src_1_color = RGBAToString(result.failure->src_1);
      sample.dst_color = RGBAToString(result.failure->dst);
      sample.blend_mode = BlendModeName(test.blend_mode);
      sample.src_blend_factor = BlendFactorName(test.blend_s_factor);
      sample.dst_blend_factor = BlendFactorName(test.blend_d_factor);
      sample.src_palette_last_color =
          RGBAToString(result.failure->src_palette_last_color);
      sample.base64_image = result.failure->base64_image;
      Report(sample);
    }
  }

  TestResult RunTest(const Test& test) {
    TestResult result = {};

    for (const Palette& dst_palette : test.dst_palettes) {
      for (const RGBA& dst : dst_palette) {
        for (const Palette& src_1_palette : test.src_1_palettes) {
          for (const RGBA& src_1 : src_1_palette) {
            for (const Palette& src_palette : test.src_palettes) {
              const auto new_result = RunTestIteration(
                  result.greatest_variance, dst, src_palette, src_1,
                  test.blend_mode, test.blend_s_factor, test.blend_d_factor);

              if (new_result.greatest_variance > result.greatest_variance) {
                result = new_result;
              }
            }
          }
        }
      }
    }

    return result;
  }

  TestResult RunTestIteration(int epsilon, RGBA dst, const Palette& src_palette,
                              RGBA src_1, GLenum blend_mode,
                              GLenum blend_s_factor, GLenum blend_d_factor) {
    TestResult result = {};

    const std::vector<Tile> tiles = DrawTestFrame(
        dst, src_palette, src_1, blend_mode, blend_s_factor, blend_d_factor);

    _pixel_buffer->CopyFromFramebuffer();

    const int variance = MeasureVariance(_pixel_buffer, tiles);

    if (variance > epsilon) {
      result.greatest_variance = variance;

      if (!result.failure) {
        result.failure = InitFailure();
      }

      result.failure->dst = dst;
      result.failure->src_1 = src_1;

      if (!src_palette.empty()) {
        const auto last = src_palette.back();
        result.failure->src_palette_last_color = last;
      }

      const auto png_data = _pixel_buffer->ToPNG();
      result.failure->base64_image =
          Base64EncodeBytes(png_data.data(), static_cast<int>(png_data.size()));
    }

    return result;
  }

  // Each "Test Frame" will draw a solid background color (dst) and will draw
  // the src_palette as a grid of Tiles. Additionally, the src_1 color is set
  // constant for all pixels.
  //
  // (So, for example, if we wanted our entire screen to appear as the src_1
  // color, we could set blend_s_factor to GL_ZERO (multiply the primary
  // fragment color by zero) and blend_d_factor to GL_SRC1_COLOR_EXT and draw
  // against a white background, with GL_ADD as our blend_mode.
  //   COLOR = 0 + (SRC_1_COLOR_EXT) * (255, 255, 255, 255)
  std::vector<Tile> DrawTestFrame(RGBA dst, const Palette& src_palette,
                                  RGBA src_1, GLenum blend_mode,
                                  GLenum blend_s_factor,
                                  GLenum blend_d_factor) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    _program->Bind();

    // Draw background (dst)
    glDisable(GL_BLEND);
    _program->SetColor(RGBAToVec4(dst));
    _program->SetModelMatrix(glm::mat4(1.0F));
    _program->DrawQuad();

    // Set blend mode
    glEnable(GL_BLEND);
    glBlendEquation(blend_mode);
    glBlendFunc(blend_s_factor, blend_d_factor);

    _program->SetColor1(RGBAToVec4(src_1));

    // Draw src palette as grid and record tile (grid) locations for measuring
    std::vector<Tile> tiles;

    int grid_index = 0;
    for (const RGBA& src : src_palette) {
      Tile tile = {};
      tile.src = src;
      tile.src1 = src_1;
      tile.dst = dst;
      tile.blend_mode = blend_mode;
      tile.blend_s_factor = blend_s_factor;
      tile.blend_d_factor = blend_d_factor;
      tile.clipspace_coord = GridClipspaceCoord(grid_index++, _grid_size);
      tiles.push_back(tile);

      const float scale = 1.0F / static_cast<float>(_grid_size);
      glm::mat4 model = glm::mat4(1.0F);
      model = glm::translate(model, glm::vec3(tile.clipspace_coord.x,
                                              tile.clipspace_coord.y, 0.0F));
      model = glm::scale(model, glm::vec3(scale, scale, 1.0F));
      _program->SetModelMatrix(model);

      _program->SetColor(RGBAToVec4(src));
      _program->DrawQuad();
    }

    return tiles;
  }

  // Measures the largest amount (0-255) by which the pixel buffer varies from
  // the expected colors for a set of tiles, which was presumably just drawn.
  // If the tile was drawn correctly, the return value should be <= `epsilon`,
  // accounting for some small imprecision.
  int MeasureVariance(std::unique_ptr<GLPixelBuffer>& buffer,
                      const std::vector<Tile>& tiles) {
    int max_variance = 0;

    for (const Tile& tile : tiles) {
      glm::ivec2 screen_coord = ClipspaceToPixelCoord(
          tile.clipspace_coord, buffer->Width(), buffer->Height());
      const RGBA result = buffer->ReadPixel(screen_coord.x, screen_coord.y);
      const RGBA expected = tile.Expected();

      const int r = abs(result.r - expected.r);
      const int g = abs(result.g - expected.g);
      const int b = abs(result.b - expected.b);
      const int a = abs(result.a - expected.a);
      const int variance = std::max(r, std::max(g, std::max(b, a)));
      if (variance > max_variance) {
        max_variance = variance;
      }
    }

    return max_variance;
  }

 private:
  configuration _configuration;
  EGLContext _egl_context = EGL_NO_CONTEXT;

  const unsigned int _grid_size = 16;
  std::unique_ptr<ShaderProgram> _program;
  std::vector<Test> _tests;
  std::unique_ptr<GLPixelBuffer> _pixel_buffer;
};

EXPORT_ANCER_OPERATION(DualSourceBlendingGLES3Operation)
