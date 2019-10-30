// Copyright 2016 Google Inc. All Rights Reserved.
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

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include "vulkan_main.h"
#include <android_native_app_glue.h>
#include <cassert>
#include <vector>
#include <array>
#include <cstring>
#include <debug_marker.h>
#include <chrono>

#include "vulkan_wrapper.h"
#include "bender_kit.h"
#include "bender_helpers.h"
#include "renderer.h"
#include "shader_state.h"
#include "geometry.h"
#include "bender_textures.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace BenderHelpers;

/// Global Variables ...

std::vector<VkImageView> displayViews_;
std::vector<VkFramebuffer> framebuffers_;

std::vector<VkBuffer> uniformBuffers_;
std::vector<VkDeviceMemory> uniformBuffersMemory_;
VkDescriptorSetLayout descriptorSetLayout_;
VkDescriptorPool descriptorPool_;
std::vector<VkDescriptorSet> descriptorSets_;

VkRenderPass render_pass;

struct UniformBufferObject {
    alignas(16) glm::mat4 mvp;
};

struct VulkanGfxPipelineInfo {
  VkPipelineLayout layout_;
  VkPipelineCache cache_;
  VkPipeline pipeline_;
};
VulkanGfxPipelineInfo gfxPipeline;

struct AttachmentBuffer {
  VkFormat format;
  VkImage image;
  VkDeviceMemory device_memory;
  VkImageView image_view;
};
AttachmentBuffer depthBuffer;

// Android Native App pointer...
android_app *androidAppCtx = nullptr;
BenderKit::Device *device;
Geometry *geometry;
Renderer *renderer;

const char* texFiles[TEXTURE_COUNT] = {
        "textures/sample_texture.png",
};
struct texture_object textures[TEXTURE_COUNT];

void createSampler(texture_object *texture) {
  const VkSamplerCreateInfo sampler = {
          .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
          .pNext = nullptr,
          .magFilter = VK_FILTER_NEAREST,
          .minFilter = VK_FILTER_NEAREST,
          .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
          .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
          .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
          .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
          .mipLodBias = 0.0f,
          .maxAnisotropy = 1,
          .compareOp = VK_COMPARE_OP_NEVER,
          .minLod = 0.0f,
          .maxLod = 0.0f,
          .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
          .unnormalizedCoordinates = VK_FALSE,
  };
  CALL_VK(vkCreateSampler(device->getDevice(), &sampler, nullptr,
                          &texture->sampler));
}

void createTextures() {
  assert(androidAppCtx != nullptr);
  assert(device != nullptr);
  for (uint32_t i = 0; i < TEXTURE_COUNT; i++) {
    LoadTextureFromFile(texFiles[i], androidAppCtx, &textures[i], device, VK_IMAGE_USAGE_SAMPLED_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    VkImageViewCreateInfo view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .image = textures[i].image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = kTexFmt,
            .components =
                    {
                            .r = VK_COMPONENT_SWIZZLE_R,
                            .g = VK_COMPONENT_SWIZZLE_G,
                            .b = VK_COMPONENT_SWIZZLE_B,
                            .a = VK_COMPONENT_SWIZZLE_A,
                    },
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
            .flags = 0,
    };
    CALL_VK(
            vkCreateImageView(device->getDevice(), &view, nullptr, &textures[i].view));
  }
}

void createFrameBuffers(VkRenderPass &renderPass,
                        VkImageView depthView = VK_NULL_HANDLE) {
  // create image view for each swapchain image
  displayViews_.resize(device->getDisplayImagesSize());
  for (uint32_t i = 0; i < device->getDisplayImagesSize(); i++) {
    VkImageViewCreateInfo viewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .image = device->getDisplayImage(i),
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = device->getDisplayFormat(),
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_R,
                .g = VK_COMPONENT_SWIZZLE_G,
                .b = VK_COMPONENT_SWIZZLE_B,
                .a = VK_COMPONENT_SWIZZLE_A,
            },
        .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        .flags = 0,
    };
    CALL_VK(vkCreateImageView(device->getDevice(), &viewCreateInfo, nullptr,
                              &displayViews_[i]));
  }

  framebuffers_.resize(device->getSwapchainLength());
  for (uint32_t i = 0; i < device->getSwapchainLength(); i++) {
    VkImageView attachments[2] = {
        displayViews_[i], depthView,
    };
    VkFramebufferCreateInfo fbCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .renderPass = renderPass,
        .layers = 1,
        .attachmentCount = 2,  // 2 if using depth
        .pAttachments = attachments,
        .width = static_cast<uint32_t>(device->getDisplaySize().width),
        .height = static_cast<uint32_t>(device->getDisplaySize().height),
    };

    CALL_VK(vkCreateFramebuffer(device->getDevice(), &fbCreateInfo, nullptr,
                                &framebuffers_[i]));
  }
}

void createUniformBuffers() {
  VkDeviceSize bufferSize = sizeof(UniformBufferObject);

  uniformBuffers_.resize(device->getSwapchainLength());
  uniformBuffersMemory_.resize(device->getSwapchainLength());

  for (size_t i = 0; i < device->getSwapchainLength(); i++) {
    device->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            uniformBuffers_[i], uniformBuffersMemory_[i]);
  }
}

void updateUniformBuffer(uint32_t frameIndex) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration<float>(currentTime - startTime).count();

    glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.5f, 1.0f, 0.5f));
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::perspective(glm::radians(100.0f),
            device->getDisplaySize().width / (float) device->getDisplaySize().height, 0.1f, 10.0f);
    proj[1][1] *= -1;
    glm::mat4 mvp = proj * view * model;

    UniformBufferObject ubo = {
        .mvp = mvp,
    };

    void* data;
    vkMapMemory(device->getDevice(), uniformBuffersMemory_[frameIndex], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device->getDevice(), uniformBuffersMemory_[frameIndex]);
}

void createDescriptorPool() {
  std::array<VkDescriptorPoolSize, 2> poolSizes = {};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = static_cast<uint32_t>(device->getDisplayImagesSize());
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = static_cast<uint32_t>(device->getDisplayImagesSize());

  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>(device->getDisplayImagesSize());

  CALL_VK(vkCreateDescriptorPool(device->getDevice(), &poolInfo, nullptr, &descriptorPool_));
}

void createDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding uboLayoutBinding = {};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.pImmutableSamplers = nullptr;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  CALL_VK(vkCreateDescriptorSetLayout(device->getDevice(), &layoutInfo, nullptr, &descriptorSetLayout_));
}

void createDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(device->getDisplayImagesSize(), descriptorSetLayout_);
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool_;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(device->getDisplayImagesSize());
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets_.resize(device->getDisplayImagesSize());
  CALL_VK(vkAllocateDescriptorSets(device->getDevice(), &allocInfo, descriptorSets_.data()));

  for (size_t i = 0; i < device->getDisplayImagesSize(); i++) {
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = uniformBuffers_[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textures[0].view;
    imageInfo.sampler = textures[0].sampler;

    std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSets_[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSets_[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device->getDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
  }
}

void createGraphicsPipeline() {
  ShaderState shaderState("triangle", androidAppCtx, device->getDevice());
  shaderState.addVertexInputBinding(0, 8 * sizeof(float));
  shaderState.addVertexAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
  shaderState.addVertexAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, 3 * sizeof(float));
  shaderState.addVertexAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, 5 * sizeof(float));

  VkViewport viewport{
      .x = 0.0f,
      .y = 0.0f,
      .width = static_cast<float>(device->getDisplaySize().width),
      .height = static_cast<float>(device->getDisplaySize().height),
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };

  VkRect2D scissor{
      .offset = {0, 0},
      .extent = device->getDisplaySize(),
  };

  VkPipelineViewportStateCreateInfo pipelineViewportState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .pViewports = &viewport,
      .scissorCount = 1,
      .pScissors = &scissor,
  };

  VkPipelineDepthStencilStateCreateInfo depthStencilState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
      .depthTestEnable = VK_TRUE,
      .depthWriteEnable = VK_TRUE,
      .depthCompareOp = VK_COMPARE_OP_LESS,
      .depthBoundsTestEnable = VK_FALSE,
      .minDepthBounds = 0.0f,
      .maxDepthBounds = 1.0f,
      .stencilTestEnable = VK_FALSE,
  };

  VkPipelineRasterizationStateCreateInfo pipelineRasterizationState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .lineWidth = 1.0f,
      .cullMode = VK_CULL_MODE_BACK_BIT,
      .frontFace = VK_FRONT_FACE_CLOCKWISE,
      .depthBiasEnable = VK_FALSE,
      .depthBiasConstantFactor = 0.0f,
      .depthBiasClamp = 0.0f,
      .depthBiasSlopeFactor = 0.0f,
  };

  // Multisample anti-aliasing setup
  VkPipelineMultisampleStateCreateInfo pipelineMultisampleState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .pNext = nullptr,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 0,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = VK_FALSE,
      .alphaToOneEnable = VK_FALSE,
  };

  // Describes how to blend pixels from past framebuffers to current framebuffers
  // Could be used for transparency or cool screen-space effects
  VkPipelineColorBlendAttachmentState attachmentStates{
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
      .blendEnable = VK_FALSE,
  };

  VkPipelineColorBlendStateCreateInfo colorBlendInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .pNext = nullptr,
      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,
      .attachmentCount = 1,
      .pAttachments = &attachmentStates,
      .flags = 0,
  };

  // Describes the layout of things such as uniforms
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .setLayoutCount = 1,
      .pSetLayouts = &descriptorSetLayout_,
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = nullptr,
  };
  CALL_VK(vkCreatePipelineLayout(device->getDevice(), &pipelineLayoutInfo, nullptr,
                                 &gfxPipeline.layout_))

  VkPipelineCacheCreateInfo pipelineCacheInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
      .pNext = nullptr,
      .initialDataSize = 0,
      .pInitialData = nullptr,
      .flags = 0,  // reserved, must be 0
  };

  CALL_VK(vkCreatePipelineCache(device->getDevice(), &pipelineCacheInfo, nullptr,
                                &gfxPipeline.cache_));

  VkGraphicsPipelineCreateInfo pipelineInfo{
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stageCount = 2,
      .pTessellationState = nullptr,
      .pViewportState = &pipelineViewportState,
      .pRasterizationState = &pipelineRasterizationState,
      .pMultisampleState = &pipelineMultisampleState,
      .pDepthStencilState = &depthStencilState,
      .pColorBlendState = &colorBlendInfo,
      .pDynamicState = nullptr,
      .layout = gfxPipeline.layout_,
      .renderPass = render_pass,
      .subpass = 0,
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = 0,
  };

  shaderState.updatePipelineInfo(pipelineInfo);

  CALL_VK(vkCreateGraphicsPipelines(device->getDevice(), gfxPipeline.cache_, 1, &pipelineInfo,
                                    nullptr, &gfxPipeline.pipeline_));
  LOGI("Setup Graphics Pipeline");
  shaderState.cleanup();
}

void createDepthBuffer() {
  depthBuffer.format = BenderHelpers::findDepthFormat(device);
  VkImageCreateInfo imageInfo = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .extent.width = device->getDisplaySize().width,
      .extent.height = device->getDisplaySize().height,
      .extent.depth = 1,
      .mipLevels = 1,
      .arrayLayers = 1,
      .format = depthBuffer.format,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  CALL_VK(vkCreateImage(device->getDevice(), &imageInfo, nullptr, &depthBuffer.image));

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device->getDevice(), depthBuffer.image, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = memRequirements.size,
      .memoryTypeIndex = BenderHelpers::findMemoryType(memRequirements.memoryTypeBits,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                       device->getPhysicalDevice()),
  };

  CALL_VK(vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &depthBuffer.device_memory))

  vkBindImageMemory(device->getDevice(), depthBuffer.image, depthBuffer.device_memory, 0);

  VkImageViewCreateInfo viewInfo = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .pNext = nullptr,
      .image = depthBuffer.image,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = depthBuffer.format,
      .subresourceRange =
          {
              .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
      .flags = 0,
  };

  CALL_VK(vkCreateImageView(device->getDevice(), &viewInfo, nullptr, &depthBuffer.image_view));

}

bool InitVulkan(android_app *app) {
  androidAppCtx = app;

  device = new BenderKit::Device(app->window);
  assert(device->isInitialized());
  DebugMarker::setObjectName(device->getDevice(), (uint64_t)device->getDevice(),
      VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT, "TEST NAME: VULKAN DEVICE");

  createUniformBuffers();

  createDescriptorSetLayout();

  renderer = new Renderer(device);

  VkAttachmentDescription color_description{
      .format = device->getDisplayFormat(),
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };

  VkAttachmentDescription depth_description{
      .format = BenderHelpers::findDepthFormat(device),
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
  };

  VkAttachmentReference color_attachment_reference = {
      .attachment = 0,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
  };

  VkAttachmentReference depth_attachment_reference = {
      .attachment = 1,
      .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
  };

  VkSubpassDescription subpass_description{
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .flags = 0,
      .inputAttachmentCount = 0,
      .pInputAttachments = nullptr,
      .colorAttachmentCount = 1,
      .pColorAttachments = &color_attachment_reference,
      .pResolveAttachments = nullptr,
      .pDepthStencilAttachment = &depth_attachment_reference,
      .preserveAttachmentCount = 0,
      .pPreserveAttachments = nullptr,
  };

  std::array<VkAttachmentDescription, 2> attachment_descriptions =
      {color_description, depth_description};

  VkRenderPassCreateInfo render_pass_createInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .pNext = nullptr,
      .attachmentCount = static_cast<uint32_t>(attachment_descriptions.size()),
      .pAttachments = attachment_descriptions.data(),
      .subpassCount = 1,
      .pSubpasses = &subpass_description,
      .dependencyCount = 0,
      .pDependencies = nullptr,
  };
  CALL_VK(vkCreateRenderPass(device->getDevice(), &render_pass_createInfo, nullptr,
                             &render_pass));

  // ---------------------------------------------
  // Create the triangle vertex buffer with indices
  const std::vector<float> vertexData = {
      -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f
  };

  const std::vector<u_int16_t> indexData = {
      1, 2, 4, 2, 1, 0, 0, 3, 2, 2, 3, 4, 3, 0, 4, 0, 1, 4
  };

  geometry = new Geometry(device, vertexData, indexData);

  createDepthBuffer();

  createFrameBuffers(render_pass, depthBuffer.image_view);

  createTextures();

  createGraphicsPipeline();

  createDescriptorPool();

  createDescriptorSets();

  return true;
}

// IsVulkanReady():
//    native app poll to see if we are ready to draw...
bool IsVulkanReady(void) { return device != nullptr && device->isInitialized(); }

void DeleteVulkan(void) {
  delete renderer;

  delete geometry;

  for (int i = 0; i < device->getSwapchainLength(); ++i) {
    vkDestroyImageView(device->getDevice(), displayViews_[i], nullptr);
    vkDestroyFramebuffer(device->getDevice(), framebuffers_[i], nullptr);

    vkDestroyBuffer(device->getDevice(), uniformBuffers_[i], nullptr);
    vkFreeMemory(device->getDevice(), uniformBuffersMemory_[i], nullptr);
  }

  vkDestroyDescriptorPool(device->getDevice(), descriptorPool_, nullptr);

  vkDestroyImageView(device->getDevice(), depthBuffer.image_view, nullptr);
  vkDestroyImage(device->getDevice(), depthBuffer.image, nullptr);
  vkFreeMemory(device->getDevice(), depthBuffer.device_memory, nullptr);

  vkDestroyPipeline(device->getDevice(), gfxPipeline.pipeline_, nullptr);
  vkDestroyPipelineCache(device->getDevice(), gfxPipeline.cache_, nullptr);
  vkDestroyPipelineLayout(device->getDevice(), gfxPipeline.layout_, nullptr);
  vkDestroyRenderPass(device->getDevice(), render_pass, nullptr);

  delete device;
  device = nullptr;
}

bool VulkanDrawFrame(void) {
  TRACE_BEGIN_SECTION("Draw Frame");

  renderer->beginFrame();
  renderer->beginPrimaryCommandBufferRecording();

  // Now we start a renderpass. Any draw command has to be recorded in a
  // renderpass
  std::array<VkClearValue, 2> clear_values = {};
  clear_values[0].color = {{0.0f, 0.34f, 0.90f, 1.0}};
  clear_values[1].depthStencil = {1.0f, 0};

  VkRenderPassBeginInfo render_pass_beginInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = render_pass,
      .framebuffer = framebuffers_[renderer->getCurrentFrame()],
      .renderArea = {.offset =
          {
              .x = 0, .y = 0,
          },
          .extent = device->getDisplaySize()},
      .clearValueCount = static_cast<uint32_t>(clear_values.size()),
      .pClearValues = clear_values.data(),
  };

  vkCmdBeginRenderPass(renderer->getCurrentCommandBuffer(), &render_pass_beginInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  float color[4] = {1.0f, 0.0f, 1.0f, 0.0f};
  DebugMarker::insert(renderer->getCurrentCommandBuffer(), "TEST MARKER: PIPELINE BINDING", color);

  vkCmdBindPipeline(renderer->getCurrentCommandBuffer(),
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    gfxPipeline.pipeline_);

  geometry->bind(renderer->getCurrentCommandBuffer());
  updateUniformBuffer(renderer->getCurrentFrame());

  vkCmdBindDescriptorSets(renderer->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
          gfxPipeline.layout_, 0, 1, &descriptorSets_[renderer->getCurrentFrame()], 0, nullptr);
  vkCmdDrawIndexed(renderer->getCurrentCommandBuffer(),
                   static_cast<u_int32_t>(geometry->getIndexCount()),
                   1, 0, 0, 0);

  vkCmdEndRenderPass(renderer->getCurrentCommandBuffer());

  renderer->endPrimaryCommandBufferRecording();
  renderer->endFrame();

  TRACE_END_SECTION();

  return true;
}

