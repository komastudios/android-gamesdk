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

ShaderState::ShaderState(std::string shader_name, const benderkit::VertexFormat& vertex_format,
                         android_app &app, VkDevice app_device)
  : android_app_ctx_(app), vertex_format_(vertex_format) {
  device_ = app_device;
  file_name_ = shader_name;

  SetVertexShader("shaders/" + shader_name + ".vert");
  SetFragmentShader("shaders/" + shader_name + ".frag");

  pipeline_input_assembly_ = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE,
  };
}

void ShaderState::OnResume(VkDevice app_device){
  device_ = app_device;
  SetVertexShader("shaders/" + file_name_ + ".vert");
  SetFragmentShader("shaders/" + file_name_ + ".frag");
}

void ShaderState::SetVertexShader(const std::string &name) {
  VkShaderModule shader;
  LoadShaderFromFile((name + ".spv").c_str(), &shader);

  VkPipelineShaderStageCreateInfo shader_stage {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = shader,
      .pSpecializationInfo = nullptr,
      .flags = 0,
      .pName = "main",
  };

  shader_stages[static_cast<int>(Type::Vertex)] = shader_stage;
  shader_modules[static_cast<int>(Type::Vertex)] = shader;
}

void ShaderState::SetFragmentShader(const std::string &name) {
  VkShaderModule shader;
  LoadShaderFromFile((name + ".spv").c_str(), &shader);

  VkPipelineShaderStageCreateInfo shader_stage {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = shader,
      .pSpecializationInfo = nullptr,
      .flags = 0,
      .pName = "main",
  };

  shader_stages[static_cast<int>(Type::Fragment)] = shader_stage;
  shader_modules[static_cast<int>(Type::Fragment)] = shader;
}

void ShaderState::FillPipelineInfo(VkGraphicsPipelineCreateInfo *pipeline_info) {
  pipeline_info->pStages = shader_stages.data();
  pipeline_info->pVertexInputState = &vertex_format_.GetVertexInputState();
  pipeline_info->pInputAssemblyState = &pipeline_input_assembly_;
}

ShaderState::~ShaderState() {
  Cleanup();
}

void ShaderState::Cleanup() {
  vkDeviceWaitIdle(device_);
  for (auto it = shader_modules.begin(); it != shader_modules.end(); it++) {
    vkDestroyShaderModule(device_, *it, nullptr);
  }
}

VkResult ShaderState::LoadShaderFromFile(const char *file_path, VkShaderModule *shader_out) {
  AAsset *file = AAssetManager_open(android_app_ctx_.activity->assetManager,
                                    file_path, AASSET_MODE_BUFFER);
  size_t file_length = AAsset_getLength(file);

  char *file_content = new char[file_length];

  AAsset_read(file, file_content, file_length);
  AAsset_close(file);

  VkShaderModuleCreateInfo shader_module_create_info {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pNext = nullptr,
      .codeSize = file_length,
      .pCode = (const uint32_t *) file_content,
      .flags = 0,
  };
  VkResult result = vkCreateShaderModule(device_, &shader_module_create_info, nullptr, shader_out);
  assert(result == VK_SUCCESS);

  delete[] file_content;

  return result;
}