//
// Created by Ganesa Chinnathambi on 2019-10-29.
//
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

/*
 * MediumPVecNormalizationVulkanOperation
 *
 * This test aims to identify devices which fail to correctly normalize
 * 3-component half-precision vectors - per reports from developers that
 * some hardware produced "garbage" when attempting to do so.
 *
 * In short, given a 3-component half precision vector, then incrementing
 * one or more fields to a value > 127, when normalized incorrect results
 * are observed from some hardware.
 *
 * Input:
 *
 * vertex_stage_configuration: configuration params for the test as run on vertex processor
 *  enabled: [bool] is this test enabled?
 *  offset_steps:[int] number of steps in range to test
 *  offset_scale:[float] each channel will be incremented by (i / offset_steps) * offset_scale
 *    resulting in offset values from [0, offset_scale]
 * fragment_stage_configuration: configuration params for running the test
 *  on the fragment processor - same fields as above
 * varying_configuration: configuration params for running the test on
 *  vertex and fragment stages, passing data through the varying pipeline. Same
 *  params as above.
 *
 * Output:
 *
 *  test:[string] the test for which this datum is issued, one of
 *    "VertexStageTest", "FragmentStageTest", "VaryingPassthroughTest"
 *  failure:[bool] if true, this datum represents a failed normalization
 *  expected_failure:[bool] if true, normalization failure is expected due to precision
 *  expected_rgb:[rgb triplet] expected framebuffer rgb value for the read fragment
 *  actual_rgb:[rgb triplet] value read from framebuffer
 *  offset:[float] offset applied to one ore more components of the vec3
 *  squared_magnitude:[float] squared mag of the vector that was normalized,
 *    i.e. (v.x*v.x + v.y*v.y + v.z*v.z)
 */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-non-private-member-variables-in-classes"

#include <cmath>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <ancer/BaseVulkanOperation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/util/Json.hpp>
#include <ancer/System.hpp>
#include <iostream>

#include <list>

#include <ancer/vulkan/render_pass.h>
#include <ancer/vulkan/graphics_pipeline.h>
#include <ancer/vulkan/shader_module.h>
#include <ancer/vulkan/buffer.h>
#include <ancer/vulkan/image.h>
#include <ancer/vulkan/context.h>

using namespace ancer;

//==============================================================================

namespace {
constexpr Log::Tag TAG{"MediumPVecNormalizationVulkanOperation"};

struct rgb_u8 {

  rgb_u8() = default;

  rgb_u8(uint8_t r, uint8_t g, uint8_t b)
      : r(r), g(g), b(b) {}

  rgb_u8(vec3 v)
      : r(static_cast<uint8_t>(ceil(v.r * 255))),
        g(static_cast<uint8_t>(ceil(v.g * 255))),
        b(static_cast<uint8_t>(ceil(v.b * 255))) {}

  rgb_u8(glm::u8vec4 v)
      : r(v.r), g(v.g), b(v.b) {}

  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
};

bool operator==(const rgb_u8& lhs, const rgb_u8& rhs) {
  return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}

bool operator!=(const rgb_u8& lhs, const rgb_u8& rhs) {
  return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b;
}

void WriteDatum(report_writers::Struct w, const rgb_u8& d) {
  ADD_DATUM_MEMBER(w, d, r);
  ADD_DATUM_MEMBER(w, d, g);
  ADD_DATUM_MEMBER(w, d, b);
}

//==============================================================================

struct test_configuration {
  bool enabled = true;

  // number of increments each channel will go through
  int offset_steps = 16;

  // each channel will be incremented by (i / offset_steps) * offset_scale
  // resulting in offset values from [0, offset_scale]
  float offset_scale = 255;
};

JSON_CONVERTER(test_configuration) {
  JSON_OPTVAR(enabled);
  JSON_REQVAR(offset_steps);
  JSON_REQVAR(offset_scale);
}

struct configuration {
  test_configuration vertex_stage_configuration;
  test_configuration fragment_stage_configuration;
  test_configuration varying_configuration;
};

JSON_CONVERTER(configuration) {
  JSON_REQVAR(vertex_stage_configuration);
  JSON_REQVAR(fragment_stage_configuration);
  JSON_REQVAR(varying_configuration);
}

//------------------------------------------------------------------------------

struct result {
  // name of test, one of TestNames[]
  std::string test;

  // did the framebuffer read differ from expected value
  bool failure = false;

  // was the value used "big enough" to expect mediump float error
  bool expected_failure = false;

  // expected value
  rgb_u8 expected_rgb8;

  // value read from framebuffer
  rgb_u8 actual_rgb8;

  float offset;

  float squared_magnitude;
};

void WriteDatum(report_writers::Struct w, const result& d) {
  ADD_DATUM_MEMBER(w, d, test);
  ADD_DATUM_MEMBER(w, d, failure);
  ADD_DATUM_MEMBER(w, d, expected_failure);
  ADD_DATUM_MEMBER(w, d, expected_rgb8);
  ADD_DATUM_MEMBER(w, d, actual_rgb8);
  ADD_DATUM_MEMBER(w, d, offset);
  ADD_DATUM_MEMBER(w, d, squared_magnitude);
}

struct datum {
  result mediump_vec_normalization_result;
};

void WriteDatum(report_writers::Struct w, const datum& d) {
  ADD_DATUM_MEMBER(w, d, mediump_vec_normalization_result);
}

//------------------------------------------------------------------------------

struct Vertex {
  enum class Attributes : GLuint {
    Pos = 0,
    BaseColor = 1,
    ColorOffset = 2
  };

  // position
  vec3 pos;

  // base color (offset will be applied to)
  vec3 base_color;

  // offset base (normalized, will be scaled by a uniform in shader)
  vec3 color_offset;
};

static const uint32_t RenderWidth = 256;
static const uint32_t RenderHeight = 4;

struct StageTest {
 private:
  GraphicsPipeline _pipeline;
  UniformBuffer _uniform_buffer;
  VkPipelineLayout _pl;
  Resources _resources;
  Vulkan _vk;

 public:
  StageTest() = default;
  StageTest(const StageTest&) = delete;
  StageTest(const StageTest&&) = delete;

  ~StageTest() {
    _pipeline.Shutdown();
    _uniform_buffer.Shutdown();
  }

  Result Initialize(Vulkan &vk, const char *vertex_shader,
                    const char * fragment_shader,
                    ResourcesLayout &layout,
                    const RenderPass &renderpass) {
    _vk = vk;

    VK_RETURN_FAIL(_vk.GetResourcesStore().Resolve({ layout }, _pl));
    
    _resources.Layout(&layout).Bind(0, _uniform_buffer);

    VertexShaderModule vertex_module;
    VK_RETURN_FAIL(vertex_module.InitializeFromGLSL(_vk, vertex_shader));

    FragmentShaderModule fragment_module;
    VK_RETURN_FAIL(fragment_module.InitializeFromGLSL(_vk, fragment_shader));

    VK_RETURN_FAIL(
    _pipeline.Builder()
      .Shader(vertex_module)
      .Shader(fragment_module)
      .VertexBinding(0, sizeof(Vertex))
      .VertexAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos))
      .VertexAttribute(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, base_color))
      .VertexAttribute(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color_offset))
      .PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
      .StaticViewport(0, 0, RenderWidth, RenderHeight, 0, 1, 0, 0, RenderWidth, RenderHeight)
      .PolygonMode(VK_POLYGON_MODE_FILL)
      .CullMode(0)
      .FrontFace(VK_FRONT_FACE_CLOCKWISE)
      .DepthMode(DepthMode::kNoDepth)
      .BlendMode(BlendMode::kOpaque)
      .Layout(_pl)
      .RenderPass(renderpass, 0)
      .Build(vk)
    );

    return Result::kSuccess;
  }

  Result Bind(GraphicsContext &context, float offset_scale) {
    VK_RETURN_FAIL(context.BindResources(VK_PIPELINE_BIND_POINT_GRAPHICS, _pl,
                                         0, { _resources }));
    _vk->cmdUpdateBuffer(context.CommandBuffer(), _uniform_buffer.Handle(), 0,
                         sizeof(float), &offset_scale);
    _vk->cmdBindPipeline(context.CommandBuffer(),
                         VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.Handle());
    return Result::kSuccess;
  }
};

struct Probe {
  // position in framebuffer to look up value from
  ivec2 pos;

  // if true, the values used are expected to
  // exceed mediump float precision
  bool failure_acceptable;

  // the expected result, pre-normalization (in range 0,BIG)
  vec3 expected_result_non_normalized;

  // the expected result, normalized (in range 0,1)
  vec3 expected_result_normalized;

  float offset;

  float squared_magnitude;
};

enum class Test : int {
  VertexStageTest = 0, FragmentStageTest, VaryingPassthroughTest
};

static const char* TestNames[] = {
  "VertexStageTest", "FragmentStageTest", "VaryingPassthroughTest"
};

static constexpr size_t NumTests = 3;

static const std::array<vec3, 7> ColorMasks = {
  vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1),
  vec3(1, 1, 0), vec3(1, 0, 1), vec3(0, 1, 1),
  vec3(1, 1, 1)
};

static const char * VertexStageVertexShader =
  "#version 450\n"
  "precision mediump float;\n"
  "layout(location = 0) in vec3 inPos;\n"
  "layout(location = 1) in vec3 inBaseColor;\n"
  "layout(location = 2) in vec3 inColorOffset;\n"
  "layout(set=0, binding=0) uniform UBO {\n"
  "  float offsetScale;\n"
  "} ubo;\n"
  "layout(location = 0) out vec3 vNormalizedColor;\n"
  "void main() {\n"
  "  vec3 v = inBaseColor;\n"
  "  v += inColorOffset * ubo.offsetScale;\n"
  "  v = normalize(v);\n"
  "  vNormalizedColor = v;\n"
  "  gl_Position = vec4(inPos.x, inPos.y, inPos.z, 1.0);\n"
  "}\n";

static const char * VertexStageFragmentShader =
  "#version 450\n"
  "precision mediump float;\n"
  "layout(location=0) in vec3 vNormalizedColor;\n"
  "layout(location=0) out vec4 outColor;\n"
  "void main() {\n"
  "  outColor = vec4(vNormalizedColor, 1.0);\n"
  "}\n";

static const char * VaryingVertexShader =
  "#version 450\n"
  "precision mediump float;\n"
  "layout(location = 0) in vec3 inPos;\n"
  "layout(location = 1) in vec3 inBaseColor;\n"
  "layout(location = 2) in vec3 inColorOffset;\n"
  "layout(set=0, binding=0) uniform UBO {\n"
  "  float offsetScale;\n"
  "} ubo;\n"
  "layout(location = 0) out vec3 vNormalizedColor;\n"
  "void main() {\n"
  "  vec3 v = inBaseColor;\n"
  "  v += inColorOffset * ubo.offsetScale;\n"
  "  v = normalize(v);\n"
  "  vNormalizedColor = v;\n"
  "  gl_Position = vec4(inPos.x, inPos.y, inPos.z, 1.0);\n"
  "}\n";

static const char * VaryingFragmentShader =
  "#version 450\n"
  "precision mediump float;\n"
  "layout(location=0) in vec3 vNormalizedColor;\n"
  "layout(location=0) out vec4 outColor;\n"
  "void main() {\n"
  "  outColor = vec4(vNormalizedColor, 1.0);\n"
  "}\n";

static const char * FragmentStageVertexShader =
  "#version 450\n"
  "precision mediump float;\n"
  "layout(location = 0) in vec3 inPos;\n"
  "layout(location = 1) in vec3 inBaseColor;\n"
  "layout(location = 2) in vec3 inColorOffset;\n"
  "layout(set=0, binding=0) uniform UBO {\n"
  "  float offsetScale;\n"
  "} ubo;\n"
  "layout(location = 0) out vec3 vNormalizedColor;\n"
  "void main() {\n"
  "  vec3 v = inBaseColor;\n"
  "  v += inColorOffset * ubo.offsetScale;\n"
  "  v = normalize(v);\n"
  "  vNormalizedColor = v;\n"
  "  gl_Position = vec4(inPos.x, inPos.y, inPos.z, 1.0);\n"
  "}\n";

static const char * FragmentStageFragmentShader =
  "#version 450\n"
  "precision mediump float;\n"
  "layout(location=0) in vec3 vNormalizedColor;\n"
  "layout(location=0) out vec4 outColor;\n"
  "void main() {\n"
  "  outColor = vec4(vNormalizedColor, 1.0);\n"
  "}\n";

} // anonymous namespace

//==============================================================================

class MediumPVecNormalizationVulkanOperation : public BaseVulkanOperation {
 public:

  MediumPVecNormalizationVulkanOperation() : BaseVulkanOperation(TAG) { }

  ~MediumPVecNormalizationVulkanOperation() {
    Cleanup();
  }

  void FillVulkanRequirements(VulkanRequirements &requirements) override {
    requirements.Debug();
  }

  void Start() override {
    BaseVulkanOperation::Start();

    _configuration = GetConfiguration<configuration>();

    _currentTest = 0;
    _test_configurations = {
        _configuration.vertex_stage_configuration,
        _configuration.fragment_stage_configuration,
        _configuration.varying_configuration,
    };

    _color_buffer
        .Initialize(_vk, EResourceUse::GPU, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                    RenderWidth, RenderHeight, 1, 1, 1, VK_FORMAT_R8G8B8_UNORM,
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, nullptr)
        .ok();

    _color_buffer_cpu
      .Builder()
      .ResourceUse(EResourceUse::GPUToCPU)
      .Usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
      .Extents(RenderWidth, RenderHeight, 1)
      .Levels(1)
      .Layers(1)
      .Linear()
      .Layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
      .Build(_vk).ok();

    _dsl.UniformBuffer(0, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

    size_t stages[3] = {
      static_cast<size_t>(Test::VertexStageTest),
      static_cast<size_t>(Test::FragmentStageTest),
      static_cast<size_t>(Test::VaryingPassthroughTest)
    };
    const char * v_shaders[3] = {
      VertexStageVertexShader,
      FragmentStageVertexShader,
      VaryingVertexShader,
    };
    const char * f_shaders[3] = {
      VertexStageFragmentShader,
      FragmentStageFragmentShader,
      VaryingFragmentShader,
    };
    for(size_t i = 0; i < 3; ++i) {
      _program_states[stages[i]].Initialize(_vk, v_shaders[i], f_shaders[i], _dsl, _renderpass).ok();
    }

    _renderpass.Builder()
      .Color(0, VK_FORMAT_R8G8B8_UNORM).Clear().End()
      .Subpass(0).Color(0).End()
      .Build(_vk).ok();

    SetHeartbeatPeriod(1s);
    UpdateTestPattern();
  }

  void Draw(double delta_seconds) override {
    BaseVulkanOperation::Draw(delta_seconds);

    if (_num_indices <= 0) {
      return;
    }

    _context.Begin().ok();

    if (_frames_drawn == ReadPixelsOnFrame) {
      ReadCurrentTestPattern();
    }

    _context.BeginRenderPass(_renderpass, RenderWidth, RenderHeight, 1,
                             { _color_buffer.View() },
                             { ClearColor(0, 0, 0, 1) }).ok();

    auto& program = _program_states[static_cast<size_t>(_currentTest)];
    const auto& config = _test_configurations[_currentTest];
    program.Bind(_context, config.offset_scale).ok();

    VkCommandBuffer cbuf = _context.CommandBuffer();
    _vk->cmdBindIndexBuffer(cbuf, _index_buffer.Handle(), 0, VK_INDEX_TYPE_UINT32);

    VkBuffer vbuf = _vertex_buffer.Handle();
    VkDeviceSize voffset;
    _vk->cmdBindVertexBuffers(cbuf, 0, 1, &vbuf, &voffset);

    _vk->cmdDrawIndexed(cbuf, _num_indices, 1, 0, 0, 0);

    _vk->cmdEndRenderPass(cbuf);

    if (_frames_drawn++ == ReadPixelsOnFrame) {
      _context.ChangeImageLayout(_color_buffer.Resource().Layout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL),
                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      _context.CopyImage(_color_buffer.Resource(), _color_buffer_cpu.Resource());
      _context.ChangeImageLayout(_color_buffer.Resource().Layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),
                                 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    }
  }

  void OnHeartbeat(Duration elapsed) override {
    // select next test, or stop if we've exhausted the tests
    if (_currentColorMask < ColorMasks.size() - 1) {
      _currentColorMask++;
    } else {
      _currentTest++;
      _currentColorMask = 0;
      if (_currentTest >= NumTests) {
        Log::D(TAG, "Exhausted test permutations, finishing...");
        Stop();
        return;
      }
    }

    if (!IsStopped()) {
      UpdateTestPattern();
    }
  }

 private:

  void UpdateTestPattern() {
    _frames_drawn = 0;
    CreateTestPattern(_test_configurations[_currentTest],
                      ColorMasks[_currentColorMask]);
  }

  void ReadCurrentTestPattern() {
    ReadTestPattern(_test_configurations[_currentTest],
                    ColorMasks[_currentColorMask]);
  }

  void CreateTestPattern(test_configuration conf, vec3 base_color) {
    // clean up previous test pattern
    Cleanup();

    Log::D(TAG,
           "CreatePattern[%s] - _currentTest: %d _currentColorMask: %d",
           TestNames[_currentTest],
           _currentTest,
           _currentColorMask);

    const int width = GetGlContextSize().x;
    const int height = GetGlContextSize().y;
    const int slices = conf.offset_steps;
    const float col_width = 1.0F / slices;
    const float offset_step = 1.0F / (slices - 1);

    std::vector<Vertex> vertices;
    std::vector<GLushort> indices;

    for (int i = 0; i < slices; i++) {
      // NOTE: We're generating vertices in clip space (-1:1)
      const float z = 0;
      const float left = -1 + 2 * i * col_width;
      const float right = left + 2 * col_width;
      const float top = 1;
      const float bottom = -1;

      const auto v_a = vec3(left, bottom, z);
      const auto v_b = vec3(left, top, z);
      const auto v_c = vec3(right, top, z);
      const auto v_d = vec3(right, bottom, z);

      const auto offset_color = vec3(base_color.r * i * offset_step,
                                     base_color.g * i * offset_step,
                                     base_color.b * i * offset_step);

      auto offset = vertices.size();
      vertices.push_back(Vertex{v_a, base_color, offset_color});
      vertices.push_back(Vertex{v_b, base_color, offset_color});
      vertices.push_back(Vertex{v_c, base_color, offset_color});
      vertices.push_back(Vertex{v_d, base_color, offset_color});

      indices.push_back(static_cast<unsigned short>(offset + 0));
      indices.push_back(static_cast<unsigned short>(offset + 1));
      indices.push_back(static_cast<unsigned short>(offset + 2));

      indices.push_back(static_cast<unsigned short>(offset + 0));
      indices.push_back(static_cast<unsigned short>(offset + 2));
      indices.push_back(static_cast<unsigned short>(offset + 3));

      // create a probe using the centroid of the quad
      // and truth values for testing
      const float middle_x = (left + right) * 0.5F;
      const float across_x = (middle_x + 1) * 0.5F; // remap from (-1,1)->(0,1)
      const int middle_x_px = static_cast<int>(floor(across_x * width));

      const float middle_y = (top + bottom) * 0.5F;
      const float across_y = (middle_y + 1) * 0.5F; // remap from (-1,1)->(0,1)
      const int middle_y_px = static_cast<int>(floor(across_y * height));

      const auto summed = base_color + (conf.offset_scale * offset_color);
      const auto expected = normalize(summed);

      /*
       * Normalizing a vec3 requires (in principle) something like:
       * float t = (x*x) + (y*y) + (z*z)
       * float normalized = sqrt(t)
       * Using a mediump precision simulator, I found that when t > 65536
       * (an auspicious number) the error of the sqrt() passes from < 1 to > 1
       * https://oletus.github.io/float16-simulator.js/
       */
      const auto squared_magnitude =
          (summed.r * summed.r) + (summed.g * summed.g) + (summed.b * summed.b);
      const auto failure_acceptable = squared_magnitude >= 65536;

      Log::D(TAG, "generating pattern for %d slices; base_color(%.3f,%.3f,%.3f)"
                  " offset_color(%.3f,%.3f,%.3f) summed(%.3f,%.3f,%.3f)"
                  " expected(%.3f,%.3f,%.3f) failure_acceptable: %s",
             slices, base_color.r, base_color.g, base_color.b,
             offset_color.r, offset_color.g, offset_color.b,
             summed.r, summed.g, summed.b,
             expected.r, expected.g, expected.b,
             failure_acceptable ? "true" : "false");

      // generate a probe to read in ReadTestPattern()
      _probes.push_back(Probe{
          ivec2(middle_x_px, middle_y_px),
          failure_acceptable,
          summed,
          expected,
          (i * offset_step) * conf.offset_scale,
          squared_magnitude
      });
    }

    // generate storage for new test pattern
    _num_indices = static_cast<uint32_t>(indices.size());

    _index_buffer.Initialize(_vk, EResourceUse::GPU, VK_INDEX_TYPE_UINT16,
                             sizeof(uint16_t) * _num_indices,
                             indices.data()).ok();

    _vertex_buffer.Initialize(_vk, EResourceUse::GPU,
                              sizeof(Vertex) * vertices.size(),
                              vertices.data()).ok();
  }

  void ReadTestPattern(test_configuration conf, vec3 base_color) {
    //glFinish();

    // capture our pixels
    //_pixel_capture->CopyFromFramebuffer();

    // allow a delta of 1 in r, g and b (this is arbitrary but allows for
    // some wiggle room)
    const auto error_margin = 1;
    bool errors = false;
    for (auto const& probe : _probes) {
      // read value_rgb at probe, and compute delta r,g,b
      auto value_rgb = rgb_u8{vec3(1, 1, 1)};
      //  _pixel_capture->ReadPixel(probe.pos.x, probe.pos.y)};

      const auto expected_value_rgb = rgb_u8{
        probe.expected_result_normalized};

      const auto dr =
          abs(static_cast<int>(value_rgb.r)
                  - static_cast<int>(expected_value_rgb.r));

      const auto dg =
          abs(static_cast<int>(value_rgb.g)
                  - static_cast<int>(expected_value_rgb.g));

      const auto db =
          abs(static_cast<int>(value_rgb.b)
                  - static_cast<int>(expected_value_rgb.b));

      const auto failure =
          dr > error_margin || dg > error_margin || db > error_margin;

      // report our read
      const auto r = result{
          TestNames[_currentTest],
          failure,
          probe.failure_acceptable,
          expected_value_rgb,
          value_rgb,
          probe.offset,
          probe.squared_magnitude
      };

      Report(datum{r});

      if (!probe.failure_acceptable) {
        if (failure) {
          // looks like we got an unexpected value_rgb
          Log::E(TAG,
                 "ReadPattern[%s] MISMATCH (_currentTest: %d _currentColorMask: %d)"
                 " base_color: (%f,%f,%f) offset: %f squared_magnitude: %f"
                 " expected_color_rgb: (%d,%d,%d) got (%d,%d,%d)",
                 TestNames[_currentTest],
                 _currentTest,
                 _currentColorMask,
                 base_color.r,
                 base_color.g,
                 base_color.b,
                 probe.offset,
                 probe.squared_magnitude,
                 expected_value_rgb.r,
                 expected_value_rgb.g,
                 expected_value_rgb.b,
                 value_rgb.r,
                 value_rgb.g,
                 value_rgb.b
          );
        } else {
          Log::D(TAG,
                 "ReadPattern[%s] CORRECT (_currentTest: %d _currentColorMask: %d)"
                 " base_color: (%f,%f,%f) offset: %f squared_magnitude: %f"
                 " expected_color_rgb: (%d,%d,%d) got (%d,%d,%d)",
                 TestNames[_currentTest],
                 _currentTest,
                 _currentColorMask,
                 base_color.r,
                 base_color.g,
                 base_color.b,
                 probe.offset,
                 probe.squared_magnitude,
                 expected_value_rgb.r,
                 expected_value_rgb.g,
                 expected_value_rgb.b,
                 value_rgb.r,
                 value_rgb.g,
                 value_rgb.b
          );
        }
      }
    }
  }

  void Cleanup() {
    _vertex_buffer.Shutdown();
    _index_buffer.Shutdown();
    _num_indices = 0;
    _probes.clear();
  }

 private:
  configuration _configuration;

  size_t _currentTest;
  size_t _currentColorMask = 0;
  std::array<StageTest, NumTests> _program_states;
  std::array<test_configuration, NumTests> _test_configurations;

  std::vector<Probe> _probes;

  GraphicsContext _context;
  Image _color_buffer;
  Image _color_buffer_cpu;
  ResourcesLayout _dsl;
  RenderPass _renderpass;
  VertexBuffer _vertex_buffer;
  IndexBuffer _index_buffer;
  uint32_t _num_indices = 0;

  size_t _frames_drawn = 0;
  static constexpr size_t ReadPixelsOnFrame = 10;
  //std::unique_ptr<GLPixelBuffer> _pixel_capture;
};

EXPORT_ANCER_OPERATION(MediumPVecNormalizationVulkanOperation)

#pragma clang diagnostic pop

