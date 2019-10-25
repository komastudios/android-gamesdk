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

class ShaderState{
public:
    ShaderState(android_app *app, VkDevice appDevice);
    VkPipelineVertexInputStateCreateInfo* getVertexInputState();
    VkPipelineInputAssemblyStateCreateInfo* getPipelineInputAssembly();
    VkPipelineShaderStageCreateInfo* getShaderStages();
    void setVertexShader(std::string shaderFile);
    void setFragmentShader(std::string shaderFile);
    void addVertexInputBinding(u_int32_t binding, u_int32_t stride);
    void addVertexAttributeDescription(u_int32_t binding, u_int32_t location, VkFormat format, u_int32_t offset);
    void cleanup();

private:
    android_app* androidAppCtx;
    VkDevice device;
    VkPipelineVertexInputStateCreateInfo vertexInputState;
    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssembly;
    std::vector<VkShaderModule> shaderModules;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    std::vector<VkVertexInputBindingDescription> vertex_input_bindings;
    std::vector<VkVertexInputAttributeDescription> vertex_input_attributes;

    VkResult loadShaderFromFile(const char *filePath, VkShaderModule *shaderOut);
};

#endif //BENDER_BASE_SHADER_STATE_HPP
