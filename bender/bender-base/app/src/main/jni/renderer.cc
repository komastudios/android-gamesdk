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

#include "renderer.h"
#include <android_native_app_glue.h>
#include <debug_marker.h>

#include "trace.h"
#include "bender_helpers.h"

using namespace BenderHelpers;

Renderer::Renderer(BenderKit::Device *device) : device_(device) {
  init();
}

Renderer::~Renderer() {
  vkFreeCommandBuffers(device_->getDevice(), cmd_pool_, cmd_buffer_len_,
                       cmd_buffer_);
  delete[] cmd_buffer_;
  delete[] acquire_image_semaphore_;
  delete[] render_finished_semaphore_;
  delete[] fence_;
  delete light_buffer_;

  vkDestroyDescriptorPool(device_->getDevice(), descriptor_pool_, nullptr);
  vkDestroyCommandPool(device_->getDevice(), cmd_pool_, nullptr);
}

void Renderer::beginFrame() {
  TRACE_BEGIN_SECTION("vkAcquireNextImageKHR");
  uint32_t nextIndex;
  CALL_VK(vkAcquireNextImageKHR(device_->getDevice(), device_->getSwapchain(),
                                UINT64_MAX, acquire_image_semaphore_[getCurrentFrame()], VK_NULL_HANDLE,
                                &nextIndex));
  TRACE_END_SECTION();

  TRACE_BEGIN_SECTION("vkWaitForFences");
  CALL_VK(vkWaitForFences(device_->getDevice(), 1, &fence_[getCurrentFrame()], VK_TRUE, 100000000));
  CALL_VK(vkResetFences(device_->getDevice(), 1, &fence_[getCurrentFrame()]));
  TRACE_END_SECTION();
}

void Renderer::endFrame() {
  VkPipelineStageFlags wait_stage_mask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &acquire_image_semaphore_[getCurrentFrame()],
      .pWaitDstStageMask = &wait_stage_mask,
      .commandBufferCount = 1,
      .pCommandBuffers = &cmd_buffer_[getCurrentFrame()],
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &render_finished_semaphore_[getCurrentFrame()]};

  TRACE_BEGIN_SECTION("vkQueueSubmit");
  CALL_VK(vkQueueSubmit(device_->getQueue(), 1, &submit_info, fence_[getCurrentFrame()]));
  TRACE_END_SECTION();

  TRACE_BEGIN_SECTION("Device::Present");
  device_->present(&render_finished_semaphore_[getCurrentFrame()]);
  TRACE_END_SECTION();
}

void Renderer::beginPrimaryCommandBufferRecording() {
  TRACE_BEGIN_SECTION("CommandBufferRecording")

  uint32_t current_frame = getCurrentFrame();
  VkCommandBufferBeginInfo cmd_buffer_beginInfo{
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
          .pNext = nullptr,
          .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
          .pInheritanceInfo = nullptr,
  };
  CALL_VK(vkBeginCommandBuffer(getCurrentCommandBuffer(),
                               &cmd_buffer_beginInfo));

  // transition the display image to color attachment layout
  setImageLayout(getCurrentCommandBuffer(),
                 device_->getDisplayImage(current_frame),
                 VK_IMAGE_LAYOUT_UNDEFINED,
                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                 VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
}

void Renderer::endPrimaryCommandBufferRecording() {
  setImageLayout(getCurrentCommandBuffer(),
                 getCurrentDisplayImage(),
                 VK_IMAGE_LAYOUT_UNDEFINED,
                 VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                 VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

  CALL_VK(vkEndCommandBuffer(getCurrentCommandBuffer()));

  TRACE_END_SECTION();
}

void Renderer::updateLightBuffer(glm::vec3 camera) {
    light_buffer_->update(getCurrentFrame(), [&camera](auto& lightBuffer) {
        lightBuffer.pointLight.position = {0.0f, 0.0f, -6.0f};
        lightBuffer.pointLight.color = {1.0f, 1.0f, 1.0f};
        lightBuffer.pointLight.intensity = 1.0f;
        lightBuffer.ambientLight.color = {1.0f, 1.0f, 1.0f};
        lightBuffer.ambientLight.intensity = 0.1f;
        lightBuffer.cameraPos = camera;
    });
}

void Renderer::init() {
  VkCommandPoolCreateInfo cmd_pool_createInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = 0,
  };
  CALL_VK(vkCreateCommandPool(device_->getDevice(), &cmd_pool_createInfo, nullptr,
                              &cmd_pool_));

  cmd_buffer_len_ = device_->getSwapchainLength();
  cmd_buffer_ = new VkCommandBuffer[device_->getSwapchainLength()];
  acquire_image_semaphore_ = new VkSemaphore[device_->getSwapchainLength()];
  render_finished_semaphore_ = new VkSemaphore[device_->getSwapchainLength()];
  fence_ = new VkFence[device_->getSwapchainLength()];

  for (int i = 0; i < device_->getSwapchainLength(); ++i) {
    VkCommandBufferAllocateInfo cmd_buffer_createInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = cmd_pool_,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    CALL_VK(vkAllocateCommandBuffers(device_->getDevice(), &cmd_buffer_createInfo,
                                     &cmd_buffer_[i]));

    DebugMarker::setObjectName(device_->getDevice(), (uint64_t)cmd_buffer_[i],
        VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT,
        ("TEST NAME: COMMAND BUFFER" + std::to_string(i)).c_str());

    VkSemaphoreCreateInfo semaphore_create_info{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    CALL_VK(vkCreateSemaphore(device_->getDevice(),
                              &semaphore_create_info,
                              nullptr,
                              &acquire_image_semaphore_[i]));

    CALL_VK(vkCreateSemaphore(device_->getDevice(),
                              &semaphore_create_info,
                              nullptr,
                              &render_finished_semaphore_[i]));

    VkFenceCreateInfo fenceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    CALL_VK(
        vkCreateFence(device_->getDevice(),
                      &fenceCreateInfo,
                      nullptr,
                      &fence_[i]));
  }

  createDescriptorPool();
  light_buffer_ = new UniformBufferObject<LightBlock>(*device_);
}

void Renderer::createDescriptorPool() {
    int MAX_MESHES = 2;

  std::array<VkDescriptorPoolSize, 3> poolSizes = {};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = MAX_MESHES * device_->getDisplayImagesSize();
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = MAX_MESHES * device_->getDisplayImagesSize();
  poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[2].descriptorCount = MAX_MESHES * device_->getDisplayImagesSize();

  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = poolSizes.size();
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = MAX_MESHES * device_->getDisplayImagesSize();

  CALL_VK(vkCreateDescriptorPool(device_->getDevice(), &poolInfo, nullptr, &descriptor_pool_));
}

VkCommandBuffer Renderer::getCurrentCommandBuffer() const {
  return cmd_buffer_[getCurrentFrame()];
}

uint32_t Renderer::getCurrentFrame() const {
  return device_->getCurrentFrameIndex();
}

VkImage Renderer::getCurrentDisplayImage() {
  return device_->getDisplayImage(getCurrentFrame());
}
