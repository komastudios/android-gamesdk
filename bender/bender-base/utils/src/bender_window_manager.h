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

#ifndef BENDER_BASE_UTILS_SRC_BENDER_WINDOW_MANAGER_H_
#define BENDER_BASE_UTILS_SRC_BENDER_WINDOW_MANAGER_H_

#include <vulkan_wrapper.h>
#include <stdexcept>
#include <android/native_window.h>

extern VkInstance benderInstance;
extern VkPhysicalDevice benderGpu;
extern VkDevice benderDevice;
extern VkQueue benderGraphicsQueue;
extern VkPhysicalDeviceMemoryProperties benderMemoryProperties;

extern VkSurfaceKHR benderSurface;
extern VkSwapchainKHR benderSwapchain;
extern VkExtent2D benderDisplaySize;
extern VkFormat benderDisplayFormat;
extern uint32_t benderSwapchainLength;

extern VkFramebuffer *benderFramebuffer;

void benderInitWindow(ANativeWindow *platformWindow,
                      VkApplicationInfo *appInfo);
void benderCreateSwapChain();
void benderCreateFrameBuffers(VkRenderPass &renderPass,
                              VkImageView depthView = VK_NULL_HANDLE);
void benderCleanup();

#endif  // BENDER_WINDOW_MANAGER_HPP
