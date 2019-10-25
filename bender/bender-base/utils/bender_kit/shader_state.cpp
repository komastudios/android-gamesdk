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

#include "shader_state.hpp"

// Constructor that sets up the current android app pointer and vulkan device
// Also setup default pipeline input assembly with Triangle List topology
ShaderState::ShaderState(android_app *app, VkDevice appDevice) {
    ShaderState::androidAppCtx = app;
    ShaderState::device = appDevice;

    // Describes how the vertex input data is organized
    pipelineInputAssembly = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,
    };
}

// Add a shader stage to the current shader program list
// Assumes shader entry function is always "main"
// Assumes shader will be located in the shader/* folder
void ShaderState::addShader(std::string shaderFile, VkShaderStageFlagBits type) {
    VkShaderModule shader;
    loadShaderFromFile((shaderFile + ".spv").c_str(), &shader);

    VkPipelineShaderStageCreateInfo shaderStage{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .stage = type,
            .module = shader,
            .pSpecializationInfo = nullptr,
            .flags = 0,
            .pName = "main",
    };

    shaderStages.push_back(shaderStage);
    shaderModules.push_back(shader);
}

void ShaderState::setVertexShader(std::string shaderFile) {
    addShader(shaderFile, VK_SHADER_STAGE_VERTEX_BIT);
}

void ShaderState::setFragmentShader(std::string shaderFile) {
    addShader(shaderFile, VK_SHADER_STAGE_FRAGMENT_BIT);
}

// Adds a Vertex Attribute description to this shader program
void ShaderState::addVertexAttributeDescription(u_int32_t binding, u_int32_t location, VkFormat format, u_int32_t offset) {
    VkVertexInputAttributeDescription attribute{
            .binding = binding,
            .location = location,
            .format = format,
            .offset = offset,
    };

    vertex_input_attributes.push_back(attribute);
}

// Creates a Vertex Binding
void ShaderState::addVertexInputBinding(u_int32_t binding, u_int32_t stride) {
    VkVertexInputBindingDescription inputBinding{
            .binding = binding,
            .stride = stride,
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    vertex_input_bindings.push_back(inputBinding);
}

// Returns the default PipelineInputAssembly created in the constructor
VkPipelineInputAssemblyStateCreateInfo* ShaderState::getPipelineInputAssembly() {
    return &pipelineInputAssembly;
}

// Creates and returns a VertexInputState based on the current state of the bindings and attributes
VkPipelineVertexInputStateCreateInfo* ShaderState::getVertexInputState(){
    // Describes the way that vertex attributes/bindings are laid out in memory
    vertexInputState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = static_cast<u_int32_t>(vertex_input_bindings.size()),
            .pVertexBindingDescriptions = vertex_input_bindings.data(),
            .vertexAttributeDescriptionCount = static_cast<u_int32_t>(vertex_input_attributes.size()),
            .pVertexAttributeDescriptions = vertex_input_attributes.data()
    };

    return &vertexInputState;
}

// Returns the pointer to the current state of the shader stages
VkPipelineShaderStageCreateInfo* ShaderState::getShaderStages() {
    return shaderStages.data();
}

// Destroy the unneeded shader modules
void ShaderState::cleanup() {
    std::vector<VkShaderModule>::iterator it;
    for (it = shaderModules.begin(); it != shaderModules.end(); it++){
        vkDestroyShaderModule(device, *it, nullptr);
    }
}


//Helper function for loading shader from Android APK
VkResult ShaderState::loadShaderFromFile(const char *filePath, VkShaderModule *shaderOut) {
    // Read the file
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