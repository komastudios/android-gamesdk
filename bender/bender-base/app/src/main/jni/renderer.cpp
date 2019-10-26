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
#include "renderer.hpp"
#include "Trace.h"
#include <android_native_app_glue.h>

Renderer::Renderer(BenderKit::Device *device) {
  device_ = device;
  current_frame = 0;

  init();
}

Renderer::~Renderer() {
  vkFreeCommandBuffers(device_->getDevice(), cmd_pool_, cmd_buffer_len_,
                       cmd_buffer_);
  delete[] cmd_buffer_;
  delete[] acquire_image_semaphore_;
  delete[] render_finished_semaphore_;
  delete[] fence_;

  vkDestroyCommandPool(device_->getDevice(), cmd_pool_, nullptr);
}

void Renderer::begin() {
  TRACE_BEGIN_SECTION("vkAcquireNextImageKHR");
  uint32_t nextIndex;
  CALL_VK(vkAcquireNextImageKHR(device_->getDevice(), device_->getSwapchain(),
                                UINT64_MAX, acquire_image_semaphore_[current_frame], VK_NULL_HANDLE,
                                &nextIndex));
  TRACE_END_SECTION();

  TRACE_BEGIN_SECTION("vkWaitForFences");
  CALL_VK(vkWaitForFences(device_->getDevice(), 1, &fence_[current_frame], VK_TRUE, 100000000));
  CALL_VK(vkResetFences(device_->getDevice(), 1, &fence_[current_frame]));
  TRACE_END_SECTION();
}

void Renderer::end() {
  VkPipelineStageFlags wait_stage_mask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &acquire_image_semaphore_[current_frame],
      .pWaitDstStageMask = &wait_stage_mask,
      .commandBufferCount = 1,
      .pCommandBuffers = &cmd_buffer_[current_frame],
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &render_finished_semaphore_[current_frame]};
  TRACE_BEGIN_SECTION("vkQueueSubmit");
  CALL_VK(vkQueueSubmit(device_->getQueue(), 1, &submit_info, fence_[current_frame]));
  TRACE_END_SECTION();

  VkResult result;
  VkPresentInfoKHR present_info{
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,
      .swapchainCount = 1,
      .pSwapchains = &device_->getSwapchain(),
      .pImageIndices = &current_frame,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &render_finished_semaphore_[current_frame],
      .pResults = &result,
  };
  TRACE_BEGIN_SECTION("vkQueuePresentKHR");
  vkQueuePresentKHR(device_->getQueue(), &present_info);
  TRACE_END_SECTION();
  current_frame = (current_frame + 1) % device_->getDisplayImagesSize();
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
}

VkCommandBuffer Renderer::getCurrentCommandBuffer() {
  return cmd_buffer_[current_frame];
}

uint32_t Renderer::getCurrentFrame() {
  return current_frame;
}

VkImage& Renderer::getCurrentDisplayImage() {
  return device_->getDisplayImages(current_frame);
}
