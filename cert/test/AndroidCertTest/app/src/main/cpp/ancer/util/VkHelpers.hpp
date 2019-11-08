/*
 * Copyright 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VK_HELPERS
#define VK_HELPERS

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "ancer/util/vulkan/VkHelpersImpl.hpp"

VkResult InitGlobalExtensionProperties(LayerProperties &layerProps);

VkResult InitGlobalLayerProperties(VulkanInfo &info);

VkResult InitDeviceExtensionProperties(struct VulkanInfo &info,
                                       LayerProperties &layerProps);

void InitInstanceExtensionNames(struct VulkanInfo &info);

VkResult InitInstance(struct VulkanInfo &info,
                      char const *const appShortName);

void InitDeviceExtensionNames(struct VulkanInfo &info);

VkResult InitDevice(struct VulkanInfo &info);

VkResult InitEnumerateDevice(struct VulkanInfo &info,
                             uint32_t gpuCount = 1);

VkBool32 DemoCheckLayers(const std::vector<LayerProperties> &layerProps,
                         const std::vector<const char *> &layerNames);

void InitConnection(struct VulkanInfo &info);

void InitWindow(struct VulkanInfo &info);

void InitQueueFamilyIndex(struct VulkanInfo &info);

void InitPresentableImage(struct VulkanInfo &info);

void ExecuteQueueCmdbuf(struct VulkanInfo &info,
                        const VkCommandBuffer *cmd_bufs, VkFence &fence);

void ExecutePrePresentBarrier(struct VulkanInfo &info);

void ExecutePresentImage(struct VulkanInfo &info);

void InitSwapchainExtension(struct VulkanInfo &info);

void InitCommandPool(struct VulkanInfo &info);

void InitCommandBuffer(struct VulkanInfo &info);

void ExecuteBeginCommandBuffer(struct VulkanInfo &info);

void ExecuteEndCommandBuffer(struct VulkanInfo &info);

void ExecuteQueueCommandBuffer(struct VulkanInfo &info);

void InitDeviceQueue(struct VulkanInfo &info);

void InitSwapChain(
        struct VulkanInfo &info,
        VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                       VK_IMAGE_USAGE_TRANSFER_SRC_BIT);

void InitDepthBuffer(struct VulkanInfo &info);

void InitUniformBuffer(struct VulkanInfo &info);

void InitDescriptorAndPipelineLayouts(struct VulkanInfo &info,
                                      bool useTexture,
                                      VkDescriptorSetLayoutCreateFlags
                                      descSetLayoutCreateFlags = 0);

void InitRenderpass(
        struct VulkanInfo &info, bool includeDepth, bool clear = true,
        VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

void InitVertexBuffer(struct VulkanInfo &info, const void *vertexData,
                      uint32_t dataSize, uint32_t dataStride,
                      bool useTexture);

void InitFramebuffers(struct VulkanInfo &info, bool includeDepth);

void InitDescriptorPool(struct VulkanInfo &info, bool use_texture);

void InitDescriptorSet(struct VulkanInfo &info, bool useTexture);

void InitShaders(struct VulkanInfo &info, const char *vertShaderText,
                 const char *fragShaderText);

void InitPipelineCache(struct VulkanInfo &info);

void InitPipeline(struct VulkanInfo &info, VkBool32 include_depth,
                  VkBool32 include_vi = static_cast<VkBool32>(true));

void InitSampler(struct VulkanInfo &info, VkSampler &sampler);

void InitImage(struct VulkanInfo &info, TextureObject &texObj,
               const char *textureName, VkImageUsageFlags extraUsages = 0,
               VkFormatFeatureFlags extraFeatures = 0);

void InitTexture(struct VulkanInfo &info, const char *textureName = nullptr,
                 VkImageUsageFlags extraUsages = 0,
                 VkFormatFeatureFlags extraFeatures = 0);

void InitViewports(struct VulkanInfo &info);

void InitScissors(struct VulkanInfo &info);

void InitFence(struct VulkanInfo &info, VkFence &fence);

void InitSubmitInfo(struct VulkanInfo &info, VkSubmitInfo &submitInfo,
                    VkPipelineStageFlags &pipeStageFlags);

void InitPresentInfo(struct VulkanInfo &info, VkPresentInfoKHR &present);

void InitClearColorAndDepth(struct VulkanInfo &info,
                            VkClearValue *clearValues);

void InitRenderPassBeginInfo(struct VulkanInfo &info,
                             VkRenderPassBeginInfo &rp_begin);

void InitWindowSize(struct VulkanInfo &info, int32_t default_width,
                    int32_t default_height);

VkResult InitDebugReportCallback(struct VulkanInfo &info,
                                 PFN_vkDebugReportCallbackEXT dbgFunc);

void DestroyDebugReportCallback(struct VulkanInfo &info);

void DestroyPipeline(struct VulkanInfo &info);

void DestroyPipelineCache(struct VulkanInfo &info);

void DestroyDescriptorPool(struct VulkanInfo &info);

void DestroyVertexBuffer(struct VulkanInfo &info);

void DestroyTextures(struct VulkanInfo &info);

void DestroyFramebuffers(struct VulkanInfo &info);

void DestroyShaders(struct VulkanInfo &info);

void DestroyRenderpass(struct VulkanInfo &info);

void DestroyDescriptorAndPipelineLayouts(struct VulkanInfo &info);

void DestroyUniformBuffer(struct VulkanInfo &info);

void DestroyDepthBuffer(struct VulkanInfo &info);

void DestroySwapChain(struct VulkanInfo &info);

void DestroyCommandBuffer(struct VulkanInfo &info);

void DestroyCommandPool(struct VulkanInfo &info);

void DestroyDevice(struct VulkanInfo &info);

void DestroyInstance(struct VulkanInfo &info);

void DestroyWindow(struct VulkanInfo &info);

#pragma clang diagnostic pop

#endif // VK_HELPERS
