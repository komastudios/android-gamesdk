// Copyright 2019 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef BENDER_BASE_SHADER_STATE_HPP
#define BENDER_BASE_SHADER_STATE_HPP

#include "vulkan_wrapper.h"

#include <android_native_app_glue.h>
#include <string>
#include <vector>
#include <array>

class ShaderState {
 public:
  enum class Type { Vertex, Fragment };
  static constexpr int shaderTypesCount = static_cast<int>(Type::Fragment) + 1;

  ShaderState(std::string shaderName, android_app *app, VkDevice appDevice);

  void addVertexAttributeDescription(u_int32_t binding,
                                     u_int32_t location,
                                     VkFormat format,
                                     u_int32_t offset);
  void addVertexInputBinding(u_int32_t binding, u_int32_t stride);
  void completeVertexInputState();

  void updatePipelineInfo(VkGraphicsPipelineCreateInfo &pipelineInfo);

  void cleanup();

 private:
  android_app *androidAppCtx;
  VkDevice device;

  VkPipelineVertexInputStateCreateInfo vertexInputState;
  VkPipelineInputAssemblyStateCreateInfo pipelineInputAssembly;

  std::array<VkShaderModule, shaderTypesCount> shaderModules;
  std::array<VkPipelineShaderStageCreateInfo, shaderTypesCount> shaderStages;

  std::vector<VkVertexInputBindingDescription> vertex_input_bindings;
  std::vector<VkVertexInputAttributeDescription> vertex_input_attributes;

  void setVertexShader(const std::string &shaderFile);
  void setFragmentShader(const std::string &shaderFile);

  VkResult loadShaderFromFile(const char *filePath, VkShaderModule *shaderOut);
};

#endif //BENDER_BASE_SHADER_STATE_HPP
