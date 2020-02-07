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

/*
 * Test phones for their use of components, many in a single block or many
 * individual single components. These limits should be the same for both.
 *
 * A component is a 32bit lane between shader stages and 4 per location. We
 * actually testing location counts.
 */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-non-private-member-variables-in-classes"

#include <ancer/BaseVulkanOperation.hpp>
#include <ancer/util/Json.hpp>

#include <ancer/vulkan/image.h>
#include <ancer/vulkan/render_pass.h>
#include <ancer/vulkan/context.h>
#include <ancer/vulkan/shader_module.h>
#include <ancer/vulkan/graphics_pipeline.h>
#include <ancer/vulkan/buffer.h>

#include <cassert>
#include <sys/mman.h>
#include <sys/user.h>
#include <iostream>
#include <sstream>

using namespace ancer;
using namespace ancer::vulkan;

namespace {
struct datum {
  bool initialization_success;
  uint32_t max_block_components;
  uint32_t max_single_components;
  uint32_t reported_max_components;
};

JSON_WRITER(datum) {
  JSON_REQVAR(initialization_success);
  JSON_REQVAR(max_block_components);
  JSON_REQVAR(max_single_components);
  JSON_REQVAR(reported_max_components);
}

constexpr Log::Tag TAG{"VulkanVaryingsTestOperation"};

void GenerateBlock(std::string &s, uint32_t num_components, bool out) {
  s += "struct VertexI {\n";
  for(uint32_t i = 0; i < num_components / 4; ++i) {
    s += "  vec4 v";
    s += std::to_string(i + 1);
    s += ";\n";
  }
  s += "};\n";
  s += "layout(location=0) ";
  s += out ? "out" : "in";
  s += " VertexI v_interface;\n";
}
void GenerateBlockVertexShader(std::string &s, uint32_t num_components) {
  s.clear();
  s += "#version 430\n";
  GenerateBlock(s, num_components, true);
  s += "layout(location=0) in vec3 a_position;\n";
  s += "void main() {\n"
       "  v_interface.v1 = vec4(a_position, 0);\n"
       "  gl_Position = vec4(a_position, 0);\n"
       "}\n";
}
void GenerateBlockFragmentShader(std::string &s, uint32_t num_components) {
  s.clear();
  s += "#version 430\n";
  GenerateBlock(s, num_components, false);
  s += "layout(location=0) out vec4 f_out;\n";
  s += "void main() {\n"
       "  f_out = vec4(v_interface.v1.rgb, 1);\n"
       "}\n";
}

void GenerateSingles(std::string &s, uint32_t num_components, bool out) {
  const char * io = out ? ") out vec4 v" : ") in vec4 v";
  for(uint32_t i = 0; i < num_components / 4; ++i) {
    s += "layout(location=";
    s += std::to_string(i);
    s += io;
    s += std::to_string(i + 1);
    s += ";\n";
  }
}
void GenerateSingleVertexShader(std::string &s, uint32_t num_components) {
  s.clear();
  s += "#version 430\n";
  GenerateSingles(s, num_components, true);
  s += "layout(location=0) in vec3 a_position;\n";
  s += "void main() {\n"
       "  v1 = vec4(a_position, 0);\n"
       "  gl_Position = vec4(a_position, 0);\n"
       "}\n";
}
void GenerateSingleFragmentShader(std::string &s, uint32_t num_components) {
  s.clear();
  s += "#version 430\n";
  GenerateSingles(s, num_components, false);
  s += "layout(location=0) out vec4 f_out;\n";
  s += "void main() {\n"
       "  f_out = vec4(v1.rgb, 1);\n"
       "}\n";
}

struct Data {
  float position[3];
} kData[3] = {
  { 0, 0, 0 },
  { 1, 0, 0 },
  { 0, 1, 0 }
};

}

#define VK_(x) do { \
  Result _r = (x); \
  if(!_r.is_success()) { \
    Log::D(TAG, "Vulkan call failed " #x); \
    return _r; \
  } \
} while(false)

class VulkanVaryingsTestOperation : public BaseVulkanOperation {
 public:
  VulkanVaryingsTestOperation() : BaseVulkanOperation(::TAG) {
  }

  ~VulkanVaryingsTestOperation() override {
  }

  Result Initialize() {
    VK_(
      _render_pass.Builder()
          .Color(0, VK_FORMAT_R8G8B8A8_UNORM).Clear().Store().End()
          .Subpass(0).Color(0).End()
          .Build(_vk)
    );
    return Result::kSuccess;
  }

  Result Initialize(const char *vertex_shader, const char *fragment_shader) {
    VK_(_vertex_shader.InitializeFromGLSL(_vk, vertex_shader));
    VK_(_fragment_shader.InitializeFromGLSL(_vk, fragment_shader));
    VK_(
      _pipeline.Builder()
          .Shader(_vertex_shader)
          .Shader(_fragment_shader)
          .VertexBinding(0, sizeof(kData[0]))
          .VertexAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT,
                           offsetof(Data, position))
          .PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
          .DepthMode(DepthMode::kNoDepth)
          .BlendMode(BlendMode::kOpaque)
          .RenderPass(_render_pass, 0)
          .Build(_vk)
    );
    return Result::kSuccess;
  }

  void Start() override {
    auto initialization_result = Initialize();

    uint32_t max =
              _vk->physical_device_properties.limits.maxVertexOutputComponents;

    uint32_t block_i;
    uint32_t single_i;

    std::string vertex;
    std::string fragment;

    for(block_i = 4; block_i < max; block_i += 4) {
      GenerateBlockVertexShader(vertex, block_i);
      GenerateBlockFragmentShader(fragment, block_i);
      if(!Initialize(vertex.c_str(), fragment.c_str()).is_success()) {
        block_i -= 4;
        break;
      }
    }

    for(single_i = 4; single_i < max; single_i += 4) {
      GenerateSingleVertexShader(vertex, single_i);
      GenerateSingleFragmentShader(fragment, single_i);
      if(!Initialize(vertex.c_str(), fragment.c_str()).is_success()) {
        single_i -= 4;
        break;
      }
    }

    Report(datum {
      initialization_result.is_success(),
      block_i,
      single_i,
      _vk->physical_device_properties.limits.maxVertexOutputComponents
    });
  }

 private:
  RenderPass _render_pass;
  VertexShaderModule _vertex_shader;
  FragmentShaderModule _fragment_shader;
  GraphicsPipeline _pipeline;
};

EXPORT_ANCER_OPERATION(VulkanVaryingsTestOperation)

#pragma clang diagnostic pop

