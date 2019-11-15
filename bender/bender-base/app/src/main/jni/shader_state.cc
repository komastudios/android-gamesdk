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

#include "shader_state.h"

ShaderState::ShaderState(std::string shaderName, const BenderKit::VertexFormat& vertex_format, android_app *app, VkDevice appDevice)
  : vertex_format_(vertex_format) {
  ShaderState::androidAppCtx = app;
  ShaderState::device = appDevice;

  setVertexShader("shaders/" + shaderName + ".vert");
  setFragmentShader("shaders/" + shaderName + ".frag");

  pipelineInputAssembly = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE,
  };
}

void ShaderState::setVertexShader(const std::string &name) {
  VkShaderModule shader;
  loadShaderFromFile((name + ".spv").c_str(), &shader);

  VkPipelineShaderStageCreateInfo shaderStage{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = shader,
      .pSpecializationInfo = nullptr,
      .flags = 0,
      .pName = "main",
  };

  shaderStages[static_cast<int>(Type::Vertex)] = shaderStage;
  shaderModules[static_cast<int>(Type::Vertex)] = shader;
}

void ShaderState::setFragmentShader(const std::string &name) {
  VkShaderModule shader;
  loadShaderFromFile((name + ".spv").c_str(), &shader);

  VkPipelineShaderStageCreateInfo shaderStage{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = shader,
      .pSpecializationInfo = nullptr,
      .flags = 0,
      .pName = "main",
  };

  shaderStages[static_cast<int>(Type::Fragment)] = shaderStage;
  shaderModules[static_cast<int>(Type::Fragment)] = shader;
}

void ShaderState::fillPipelineInfo(VkGraphicsPipelineCreateInfo *pipeline_info) {
  pipeline_info->pStages = shaderStages.data();
  pipeline_info->pVertexInputState = &vertex_format_.getVertexInputState();
  pipeline_info->pInputAssemblyState = &pipelineInputAssembly;
}

void ShaderState::cleanup() {
  for (auto it = shaderModules.begin(); it != shaderModules.end(); it++) {
    vkDestroyShaderModule(device, *it, nullptr);
  }
}

VkResult ShaderState::loadShaderFromFile(const char *filePath, VkShaderModule *shaderOut) {
  assert(androidAppCtx);
  AAsset *file = AAssetManager_open(androidAppCtx->activity->assetManager,
                                    filePath, AASSET_MODE_BUFFER);
  size_t fileLength = AAsset_getLength(file);

  char *fileContent = new char[fileLength];

  AAsset_read(file, fileContent, fileLength);
  AAsset_close(file);

  VkShaderModuleCreateInfo shaderModuleCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pNext = nullptr,
      .codeSize = fileLength,
      .pCode = (const uint32_t *) fileContent,
      .flags = 0,
  };
  VkResult result = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, shaderOut);
  assert(result == VK_SUCCESS);

  delete[] fileContent;

  return result;
}