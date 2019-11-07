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
#include "polyhedron.h"
#include "mesh.h"
#include "texture.h"
#include "uniform_buffer.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace BenderHelpers;

/// Global Variables ...

std::vector<VkImageView> displayViews_;
std::vector<VkFramebuffer> framebuffers_;

struct ModelViewProjection {
  alignas(16) glm::mat4 mvp;
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 invTranspose;
};

struct PointLight{
  alignas(16) float intensity;
  alignas(16) glm::vec3 position;
  alignas(16) glm::vec3 color;
};

struct AmbientLight{
  alignas(16) float intensity;
  alignas(16) glm::vec3 color;
};

struct LightBlock{
  PointLight pointLight;
  AmbientLight ambientLight;
  alignas(16) glm::vec3 cameraPos;
};

struct Camera {
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
} camera;

UniformBufferObject<ModelViewProjection> *uniformBuffer;
UniformBufferObject<LightBlock> *lightBuffer;

VkDescriptorPool descriptorPool_;
std::vector<VkDescriptorSet> descriptorSets_;

VkRenderPass render_pass;

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
Renderer *renderer;

std::shared_ptr<ShaderState> shaders;

Mesh *mesh;

auto lastTime = std::chrono::high_resolution_clock::now();
auto currentTime = lastTime;
float frameTime;
float totalTime;

std::vector<Texture*> textures;
std::vector<const char*> texFiles;
VkSampler sampler;

void createSampler() {
  const VkSamplerCreateInfo sampler_create_info = {
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
  CALL_VK(vkCreateSampler(device->getDevice(), &sampler_create_info, nullptr,
                          &sampler));
}

void createTextures() {
  assert(androidAppCtx != nullptr);
  assert(device != nullptr);

  for (uint32_t i = 0; i < texFiles.size(); ++i) {
    textures.push_back(new Texture(*device, androidAppCtx, texFiles[i], VK_FORMAT_R8G8B8A8_SRGB));
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
  uniformBuffer = new UniformBufferObject<ModelViewProjection>(*device);
  lightBuffer = new UniformBufferObject<LightBlock>(*device);
}

void updateCamera(Input::Data *inputData) {
  camera.rotation =
      glm::quat(glm::vec3(0.0f, inputData->deltaX / device->getDisplaySize().width, 0.0f))
          * camera.rotation;
  camera.rotation *=
      glm::quat(glm::vec3(inputData->deltaY / device->getDisplaySize().height, 0.0f, 0.0f));
  camera.rotation = glm::normalize(camera.rotation);

  glm::vec3 up = glm::normalize(camera.rotation * glm::vec3(0.0f, 1.0f, 0.0f));
  glm::vec3 right = glm::normalize(camera.rotation * glm::vec3(1.0f, 0.0f, 0.0f));
  camera.position += right * (inputData->deltaXPan / device->getDisplaySize().width) +
                     up * -(inputData->deltaYPan / device->getDisplaySize().height);

  if (inputData->doubleTapHoldUpper) {
    glm::vec3 forward = glm::normalize(camera.rotation * glm::vec3(0.0f, 0.0f, -1.0f));
    camera.position += forward * 2.0f * frameTime;
  }
  else if (inputData->doubleTapHoldLower) {
    glm::vec3 forward = glm::normalize(camera.rotation * glm::vec3(0.0f, 0.0f, -1.0f));
    camera.position -= forward * 2.0f * frameTime;
  }
}

void updateUniformBuffer(uint32_t frameIndex, Input::Data *inputData) {
    mesh->rotate(glm::vec3(0.0f, 1.0f, 1.0f), 90 * frameTime);
    mesh->translate(.02f * glm::vec3(std::sin(2 * totalTime),
                                     std::sin(3 * totalTime),
                                     std::cos(2 * totalTime)));

    updateCamera(inputData);
    lightBuffer->update(frameIndex, [](auto& lightBuffer) {
      lightBuffer.pointLight.position = {0.0f, 0.0f, 2.0f};
      lightBuffer.pointLight.color = {1.0f, 1.0f, 1.0f};
      lightBuffer.pointLight.intensity = 1.0f;
      lightBuffer.ambientLight.color = {1.0f, 1.0f, 1.0f};
      lightBuffer.ambientLight.intensity = 0.1f;
      lightBuffer.cameraPos = camera.position;
    });

    glm::mat4 model = mesh->getTransform();
    glm::mat4 view = glm::inverse(glm::translate(glm::mat4(1.0f), camera.position) * glm::mat4(camera.rotation));
    glm::mat4 proj = glm::perspective(glm::radians(100.0f),
            device->getDisplaySize().width / (float) device->getDisplaySize().height, 0.1f, 100.0f);
    proj[1][1] *= -1;

    glm::mat4 mvp = proj * view * model;

    uniformBuffer->update(frameIndex, [&mvp, &model](auto& ubo) {
      ubo.mvp = mvp;
      ubo.model = model;
      ubo.invTranspose = glm::transpose(glm::inverse(model));
    });
}

void createDescriptorPool() {
  std::array<VkDescriptorPoolSize, 4> poolSizes = {};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = device->getDisplayImagesSize();
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = device->getDisplayImagesSize();
  poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[2].descriptorCount = device->getDisplayImagesSize();
  poolSizes[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[3].descriptorCount = device->getDisplayImagesSize();

  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = poolSizes.size();
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = device->getDisplayImagesSize();

  CALL_VK(vkCreateDescriptorPool(device->getDevice(), &poolInfo, nullptr, &descriptorPool_));
}

void createDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(device->getDisplayImagesSize(), mesh->getDescriptorSetLayout());
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool_;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(device->getDisplayImagesSize());
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets_.resize(device->getDisplayImagesSize());
  CALL_VK(vkAllocateDescriptorSets(device->getDevice(), &allocInfo, descriptorSets_.data()));

  for (size_t i = 0; i < device->getDisplayImagesSize(); i++) {
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = uniformBuffer->getBuffer(i);
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(ModelViewProjection);

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textures[0]->getImageView();
    imageInfo.sampler = VK_NULL_HANDLE;

    VkDescriptorBufferInfo lightBlockInfo = {};
    lightBlockInfo.buffer = lightBuffer->getBuffer(i);
    lightBlockInfo.offset = 0;
    lightBlockInfo.range = sizeof(LightBlock);

    std::array<VkWriteDescriptorSet, 3> descriptorWrites = {};

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

    descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[2].dstSet = descriptorSets_[i];
    descriptorWrites[2].dstBinding = 2;
    descriptorWrites[2].dstArrayElement = 0;
    descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[2].descriptorCount = 1;
    descriptorWrites[2].pBufferInfo = &lightBlockInfo;

    vkUpdateDescriptorSets(device->getDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
  }
}

void createShaderState() {
  shaders = std::make_shared<ShaderState>("triangle", androidAppCtx, device->getDevice());
  shaders->addVertexInputBinding(0, 11 * sizeof(float));
  shaders->addVertexAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
  shaders->addVertexAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, 3 * sizeof(float));
  shaders->addVertexAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, 6 * sizeof(float));
  shaders->addVertexAttributeDescription(0, 3, VK_FORMAT_R32G32_SFLOAT, 9 * sizeof(float));
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
  device->setObjectName(reinterpret_cast<uint64_t>(device->getDevice()),
      VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT, "TEST NAME: VULKAN DEVICE");

  createUniformBuffers();

//  createDescriptorSetLayout();

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

  createShaderState();

  mesh = createPolyhedron(device, shaders, 12);

  const std::vector<float> vertexData = {
      -0.5f, -0.5f, 0.5f,          -0.5774f, -0.5774f, 0.5774f,       1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.5f, -0.5f, 0.5f,           0.5774f, -0.5774f, 0.5774f,        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
      0.5f, 0.5f, 0.5f,            0.5774f, 0.5774f, 0.5774f,         0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      -0.5f, 0.5f, 0.5f,           -0.5774f, 0.5774f, 0.5774f,      1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      0.0f, 0.0f, 0.0f,            0.0f, 1.0f, 0.0f,           1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
  };

  const std::vector<u_int16_t> indexData = {
      1, 2, 4, 2, 1, 0, 0, 3, 2, 2, 3, 4, 3, 0, 4, 0, 1, 4
  };
  
  createDepthBuffer();

  createFrameBuffers(render_pass, depthBuffer.image_view);

  texFiles.push_back("textures/sample_texture.png");

  createTextures();

  createDescriptorPool();

  createDescriptorSets();

  return true;
}

bool IsVulkanReady(void) { return device != nullptr && device->isInitialized(); }

void DeleteVulkan(void) {
  delete renderer;
  delete mesh;
  delete uniformBuffer;
  delete lightBuffer;

  shaders->cleanup();
  shaders.reset();

  for (int i = 0; i < device->getSwapchainLength(); ++i) {
    vkDestroyImageView(device->getDevice(), displayViews_[i], nullptr);
    vkDestroyFramebuffer(device->getDevice(), framebuffers_[i], nullptr);
  }

  vkDestroyDescriptorPool(device->getDevice(), descriptorPool_, nullptr);

  vkDestroyImageView(device->getDevice(), depthBuffer.image_view, nullptr);
  vkDestroyImage(device->getDevice(), depthBuffer.image, nullptr);
  vkFreeMemory(device->getDevice(), depthBuffer.device_memory, nullptr);

  vkDestroyRenderPass(device->getDevice(), render_pass, nullptr);

  delete device;
  device = nullptr;
}

bool VulkanDrawFrame(Input::Data *inputData) {
  TRACE_BEGIN_SECTION("Draw Frame");
  currentTime = std::chrono::high_resolution_clock::now();
  frameTime = std::chrono::duration<float>(currentTime - lastTime).count();;
  lastTime = currentTime;
  totalTime += frameTime;

  updateUniformBuffer(renderer->getCurrentFrame(), inputData);

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

  device->insertDebugMarker(renderer->getCurrentCommandBuffer(), "TEST MARKER: PIPELINE BINDING",
                            {1.0f, 0.0f, 1.0f, 0.0f});

  mesh->updatePipeline(render_pass);
  mesh->submitDraw(renderer->getCurrentCommandBuffer(), descriptorSets_[renderer->getCurrentFrame()]);

  vkCmdEndRenderPass(renderer->getCurrentCommandBuffer());

  renderer->endPrimaryCommandBufferRecording();
  renderer->endFrame();

  TRACE_END_SECTION();

  return true;
}

