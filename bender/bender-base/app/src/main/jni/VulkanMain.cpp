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

#include "VulkanMain.hpp"
#include <android_native_app_glue.h>
#include <cassert>
#include <vector>
#include <cstring>
#include "vulkan_wrapper.h"
#include "bender_kit.hpp"
#include "shader_state.hpp"


/// Global Variables ...

std::vector<VkImageView> displayViews_;
std::vector<VkFramebuffer> framebuffers_;

struct VulkanRenderInfo {
  VkRenderPass renderPass_;
  VkCommandPool cmdPool_;
  VkCommandBuffer* cmdBuffer_;
  uint32_t cmdBufferLen_;
  VkSemaphore semaphore_;
  VkFence fence_;
};
VulkanRenderInfo render;

struct VulkanGfxPipelineInfo {
    VkPipelineLayout layout_;
    VkPipelineCache cache_;
    VkPipeline pipeline_;
};
VulkanGfxPipelineInfo gfxPipeline;

struct VulkanBufferInfo {
    VkBuffer vertexBuf_;
    VkDeviceMemory bufferDeviceMemory;
    int vertexCount;

    VkBuffer indexBuf_;
    VkDeviceMemory indexBufferDeviceMemory;
    int indexCount;
};
VulkanBufferInfo buffers;


// Android Native App pointer...
android_app* androidAppCtx = nullptr;
BenderKit::Device *device;


/*
 * setImageLayout():
 *    Helper function to transition color buffer layout
 */
void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image,
                    VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
                    VkPipelineStageFlags srcStages,
                    VkPipelineStageFlags destStages);

void CreateFrameBuffers(VkRenderPass& renderPass,
                        VkImageView depthView = VK_NULL_HANDLE) {
  // create image view for each swapchain image
  displayViews_.resize(device->getDisplayImagesSize());
  for (uint32_t i = 0; i < device->getDisplayImagesSize(); i++) {
    VkImageViewCreateInfo viewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .image = device->getDisplayImages(i),
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

  // create a framebuffer from each swapchain image
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
            .attachmentCount = 1,  // 2 if using depth
            .pAttachments = attachments,
            .width = static_cast<uint32_t>(device->getDisplaySize().width),
            .height = static_cast<uint32_t>(device->getDisplaySize().height),
    };
    fbCreateInfo.attachmentCount = (depthView == VK_NULL_HANDLE ? 1 : 2);

    CALL_VK(vkCreateFramebuffer(device->getDevice(), &fbCreateInfo, nullptr,
                                &framebuffers_[i]));
  }
}

// A helper function for selecting the correct memory type for a buffer
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice gpuDevice) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(gpuDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    LOGE("failed to find suitable memory type!");
    return -1;
}

// Generalized helper function for creating different types of buffers
void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                  VkBuffer &buffer, VkDeviceMemory &bufferMemory) {
    VkBufferCreateInfo bufferInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .size = size,
            .usage = usage,
            .flags = 0,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
    };

    CALL_VK(vkCreateBuffer(device->getDevice(), &bufferInfo, nullptr, &buffer));

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device->getDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                              properties, device->getPhysicalDevice())
    };

    // Allocate memory for the buffer
    CALL_VK(vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &bufferMemory));
    CALL_VK(vkBindBufferMemory(device->getDevice(), buffer, bufferMemory, 0));
}

// Create buffers for vertex data
bool createVertexBuffer(void) {
    // Vertex positions
    const std::vector<float> vertexData = {
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
    };

    const std::vector<u_int16_t > indexData = {
            0, 1, 2, 2, 3, 0
    };

    buffers.vertexCount = vertexData.size();
    buffers.indexCount = indexData.size();

    VkDeviceSize bufferSizeVertex = sizeof(vertexData[0]) * vertexData.size();
    createBuffer(bufferSizeVertex, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 buffers.vertexBuf_, buffers.bufferDeviceMemory);

    VkDeviceSize bufferSizeIndex = sizeof(indexData[0]) * indexData.size();
    createBuffer(bufferSizeIndex, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 buffers.indexBuf_, buffers.indexBufferDeviceMemory);


    void* data;
    vkMapMemory(device->getDevice(), buffers.bufferDeviceMemory, 0, bufferSizeVertex, 0, &data);
    memcpy(data, vertexData.data(), bufferSizeVertex);
    vkUnmapMemory(device->getDevice(), buffers.bufferDeviceMemory);

    vkMapMemory(device->getDevice(), buffers.indexBufferDeviceMemory, 0, bufferSizeIndex, 0, &data);
    memcpy(data, indexData.data(), bufferSizeIndex);
    vkUnmapMemory(device->getDevice(), buffers.indexBufferDeviceMemory);

    return true;
}

void createGraphicsPipeline() {
    ShaderState currShader("triangle", androidAppCtx, device->getDevice());
    currShader.addVertexInputBinding(0, 6 * sizeof(float));
    currShader.addVertexAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
    currShader.addVertexAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, 3 * sizeof(float));

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

    // Describes how the GPU should rasterize pixels from polygons
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
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
    };
    CALL_VK(vkCreatePipelineLayout(device->getDevice(), &pipelineLayoutInfo, nullptr,
                                   &gfxPipeline.layout_))

    // Create the pipeline cache
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
            .pStages = currShader.getShaderStages(),
            .pVertexInputState = currShader.getVertexInputState(),
            .pInputAssemblyState = currShader.getPipelineInputAssembly(),
            .pTessellationState = nullptr,
            .pViewportState = &pipelineViewportState,
            .pRasterizationState = &pipelineRasterizationState,
            .pMultisampleState = &pipelineMultisampleState,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlendInfo,
            .pDynamicState = nullptr,
            .layout = gfxPipeline.layout_,
            .renderPass = render.renderPass_,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0,
    };

    CALL_VK(vkCreateGraphicsPipelines(device->getDevice(), gfxPipeline.cache_, 1, &pipelineInfo,
                                      nullptr, &gfxPipeline.pipeline_));
    LOGI("Setup Graphics Pipeline");
    currShader.cleanup();
}

// InitVulkan:
//   Initialize Vulkan Context when android application window is created
//   upon return, vulkan is ready to draw frames
bool InitVulkan(android_app* app) {
  androidAppCtx = app;


  device = new BenderKit::Device(app->window);
  assert(device->isInitialized());

  // -----------------------------------------------------------------
  // Create render pass
  VkAttachmentDescription attachmentDescriptions{
      .format = device->getDisplayFormat(),
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };

  VkAttachmentReference colourReference = {
      .attachment = 0,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
  };

  VkSubpassDescription subpassDescription{
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .flags = 0,
      .inputAttachmentCount = 0,
      .pInputAttachments = nullptr,
      .colorAttachmentCount = 1,
      .pColorAttachments = &colourReference,
      .pResolveAttachments = nullptr,
      .pDepthStencilAttachment = nullptr,
      .preserveAttachmentCount = 0,
      .pPreserveAttachments = nullptr,
  };
  VkRenderPassCreateInfo renderPassCreateInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .pNext = nullptr,
      .attachmentCount = 1,
      .pAttachments = &attachmentDescriptions,
      .subpassCount = 1,
      .pSubpasses = &subpassDescription,
      .dependencyCount = 0,
      .pDependencies = nullptr,
  };
  CALL_VK(vkCreateRenderPass(device->getDevice(), &renderPassCreateInfo, nullptr,
                             &render.renderPass_));

  // ---------------------------------------------
  // Create the triangle vertex buffer with indices
  createVertexBuffer();

  // -----------------------------------------------------------------
  // Create 2 frame buffers.
  CreateFrameBuffers(render.renderPass_);

  // -----------------------------------------------------------------
  // Create Graphics Pipeline and layouts
  createGraphicsPipeline();

  // -----------------------------------------------
  // Create a pool of command buffers to allocate command buffer from
  VkCommandPoolCreateInfo cmdPoolCreateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = 0,
  };
  CALL_VK(vkCreateCommandPool(device->getDevice(), &cmdPoolCreateInfo, nullptr,
                              &render.cmdPool_));

  // Record a command buffer that just clear the screen
  // 1 command buffer draw in 1 framebuffer
  // In our case we need 2 command as we have 2 framebuffer
  render.cmdBufferLen_ = device->getSwapchainLength();
  render.cmdBuffer_ = new VkCommandBuffer[device->getSwapchainLength()];
  for (int bufferIndex = 0; bufferIndex < device->getSwapchainLength();
       bufferIndex++) {
    // We start by creating and declare the "beginning" our command buffer
    VkCommandBufferAllocateInfo cmdBufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = render.cmdPool_,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    CALL_VK(vkAllocateCommandBuffers(device->getDevice(), &cmdBufferCreateInfo,
                                     &render.cmdBuffer_[bufferIndex]));

    VkCommandBufferBeginInfo cmdBufferBeginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr,
    };
    CALL_VK(vkBeginCommandBuffer(render.cmdBuffer_[bufferIndex],
                                 &cmdBufferBeginInfo));
    // transition the display image to color attachment layout
    setImageLayout(render.cmdBuffer_[bufferIndex],
                   device->getDisplayImages(bufferIndex),
                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    // Now we start a renderpass. Any draw command has to be recorded in a
    // renderpass
    VkClearValue clearVals{
        .color.float32[0] = 0.0f,
        .color.float32[1] = 0.34f,
        .color.float32[2] = 0.90f,
        .color.float32[3] = 1.0f,
    };
    VkRenderPassBeginInfo renderPassBeginInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = render.renderPass_,
        .framebuffer = framebuffers_[bufferIndex],
        .renderArea = {.offset =
                           {
                               .x = 0, .y = 0,
                           },
                       .extent = device->getDisplaySize()},
        .clearValueCount = 1,
        .pClearValues = &clearVals};
    vkCmdBeginRenderPass(render.cmdBuffer_[bufferIndex], &renderPassBeginInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
    // Do more drawing !
    vkCmdBindPipeline(render.cmdBuffer_[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, gfxPipeline.pipeline_);
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(render.cmdBuffer_[bufferIndex], 0, 1, &buffers.vertexBuf_, &offset);
    vkCmdBindIndexBuffer(render.cmdBuffer_[bufferIndex], buffers.indexBuf_, offset, VK_INDEX_TYPE_UINT16 );
    vkCmdDrawIndexed(render.cmdBuffer_[bufferIndex], static_cast<u_int32_t>(buffers.indexCount), 1, 0, 0, 0);

    vkCmdEndRenderPass(render.cmdBuffer_[bufferIndex]);
    // transition back to swapchain image to PRESENT_SRC_KHR
    setImageLayout(render.cmdBuffer_[bufferIndex],
                   device->getDisplayImages(bufferIndex),
                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    CALL_VK(vkEndCommandBuffer(render.cmdBuffer_[bufferIndex]));
  }

  // We need to create a fence to be able, in the main loop, to wait for our
  // draw command(s) to finish before swapping the framebuffers
  VkFenceCreateInfo fenceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
  };
  CALL_VK(
      vkCreateFence(device->getDevice(), &fenceCreateInfo, nullptr, &render.fence_));

  // We need to create a semaphore to be able to wait, in the main loop, for our
  // framebuffer to be available for us before drawing.
  VkSemaphoreCreateInfo semaphoreCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
  };
  CALL_VK(vkCreateSemaphore(device->getDevice(), &semaphoreCreateInfo, nullptr,
                            &render.semaphore_));

  return true;
}

// IsVulkanReady():
//    native app poll to see if we are ready to draw...
bool IsVulkanReady(void) { return device != nullptr && device->isInitialized(); }

void DeleteVulkan(void) {
  vkFreeCommandBuffers(device->getDevice(), render.cmdPool_, render.cmdBufferLen_,
                       render.cmdBuffer_);
  delete[] render.cmdBuffer_;

  vkDestroyCommandPool(device->getDevice(), render.cmdPool_, nullptr);
  vkDestroyRenderPass(device->getDevice(), render.renderPass_, nullptr);

  vkDestroyBuffer(device->getDevice(), buffers.vertexBuf_, nullptr);
  vkFreeMemory(device->getDevice(), buffers.bufferDeviceMemory, nullptr);
  vkDestroyBuffer(device->getDevice(), buffers.indexBuf_, nullptr);
  vkFreeMemory(device->getDevice(), buffers.indexBufferDeviceMemory, nullptr);

  for (int i = 0; i < device->getSwapchainLength(); ++i) {
    vkDestroyImageView(device->getDevice(), displayViews_[i], nullptr);
    vkDestroyFramebuffer(device->getDevice(), framebuffers_[i], nullptr);
  }

  vkDestroyPipeline(device->getDevice(), gfxPipeline.pipeline_, nullptr);
  vkDestroyPipelineCache(device->getDevice(), gfxPipeline.cache_, nullptr);
  vkDestroyPipelineLayout(device->getDevice(), gfxPipeline.layout_, nullptr);
  vkDestroyRenderPass(device->getDevice(), render.renderPass_, nullptr);

  delete device;
  device = nullptr;
}

// Draw one frame
bool VulkanDrawFrame(void) {
  uint32_t nextIndex;
  // Get the framebuffer index we should draw in
  CALL_VK(vkAcquireNextImageKHR(device->getDevice(), device->getSwapchain(),
                                UINT64_MAX, render.semaphore_, VK_NULL_HANDLE,
                                &nextIndex));
  CALL_VK(vkResetFences(device->getDevice(), 1, &render.fence_));
  VkPipelineStageFlags waitStageMask =
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                              .pNext = nullptr,
                              .waitSemaphoreCount = 1,
                              .pWaitSemaphores = &render.semaphore_,
                              .pWaitDstStageMask = &waitStageMask,
                              .commandBufferCount = 1,
                              .pCommandBuffers = &render.cmdBuffer_[nextIndex],
                              .signalSemaphoreCount = 0,
                              .pSignalSemaphores = nullptr};
  CALL_VK(vkQueueSubmit(device->getQueue(), 1, &submit_info, render.fence_));
  CALL_VK(
      vkWaitForFences(device->getDevice(), 1, &render.fence_, VK_TRUE, 100000000));

  LOGI("Drawing frames......");

  VkResult result;
  VkPresentInfoKHR presentInfo{
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,
      .swapchainCount = 1,
      .pSwapchains = &device->getSwapchain(),
      .pImageIndices = &nextIndex,
      .waitSemaphoreCount = 0,
      .pWaitSemaphores = nullptr,
      .pResults = &result,
  };
  vkQueuePresentKHR(device->getQueue(), &presentInfo);
  return true;
}

/*
 * setImageLayout():
 *    Helper function to transition color buffer layout
 */
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

    case VK_IMAGE_LAYOUT_PREINITIALIZED:
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
      break;

    default:
      break;
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

    default:
      break;
  }

  vkCmdPipelineBarrier(cmdBuffer, srcStages, destStages, 0, 0, NULL, 0, NULL, 1,
                       &imageMemoryBarrier);
}
