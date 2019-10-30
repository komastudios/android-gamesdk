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
#include "trace.h"
#include <android_native_app_glue.h>
#include <debug_marker.h>

void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image,
                    VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
                    VkPipelineStageFlags srcStages,
                    VkPipelineStageFlags destStages);


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

void Renderer::beginFrame() {
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

void Renderer::endFrame() {
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
  VkSwapchainKHR swapchains[] = { device_->getSwapchain() };
  VkPresentInfoKHR present_info{
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,
      .swapchainCount = 1,
      .pSwapchains = swapchains,
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
}

VkCommandBuffer Renderer::getCurrentCommandBuffer() {
  return cmd_buffer_[current_frame];
}

uint32_t Renderer::getCurrentFrame() {
  return current_frame;
}

VkImage Renderer::getCurrentDisplayImage() {
  return device_->getDisplayImage(current_frame);
}

void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image,
                    VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
                    VkPipelineStageFlags srcStages,
                    VkPipelineStageFlags destStages) {
  VkImageMemoryBarrier imageMemoryBarrier = {
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .pNext = NULL,
          .srcAccessMask = 0,
          .dstAccessMask = 0,
          .oldLayout = oldImageLayout,
          .newLayout = newImageLayout,
          .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
          .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
          .image = image,
          .subresourceRange =
                  {
                          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                          .baseMipLevel = 0,
                          .levelCount = 1,
                          .baseArrayLayer = 0,
                          .layerCount = 1,
                  },
  };

  switch (oldImageLayout) {
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_PREINITIALIZED:imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
      break;

    default:break;
  }

  switch (newImageLayout) {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      break;

    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
      break;

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
      imageMemoryBarrier.dstAccessMask =
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;

    default:break;
  }

  vkCmdPipelineBarrier(cmdBuffer, srcStages, destStages, 0, 0, NULL, 0, NULL, 1,
                       &imageMemoryBarrier);
}
