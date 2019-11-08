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

#ifndef BENDER_BASE_UTILS_BENDER_KIT_BENDER_KIT_H_
#define BENDER_BASE_UTILS_BENDER_KIT_BENDER_KIT_H_

#include <android/log.h>
#include <vector>
#include <string>
#include <array>

static const char *kTAG = "BenderKit";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

#define CALL_VK(func)                                                     \
    if (VK_SUCCESS != (func)) {                                           \
        __android_log_print(ANDROID_LOG_ERROR, "Bender ",                 \
                            "Vulkan error. File[%s], line[%d]", __FILE__, \
                            __LINE__);                                    \
        assert(false);                                                    \
    }

namespace BenderKit {
class Device {
 public:
  Device(ANativeWindow *window);

  ~Device();

  bool isInitialized() { return initialized_; }

//  void setInitialized(bool flag) { initialized_ = flag; }

  void CreateImageView();

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory,
                    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VkDevice getDevice() const { return device_; }

  VkPhysicalDevice getPhysicalDevice() const { return gpuDevice_; }

  uint32_t getQueueFamilyIndex() const { return queueFamilyIndex_; }

  VkSurfaceKHR getSurface() const { return surface_; }

  VkQueue getQueue() const { return queue_; }

  VkSwapchainKHR getSwapchain() const { return swapchain_; }

  uint getSwapchainLength() const { return swapchainLength_; }

  VkSurfaceTransformFlagBitsKHR getPretransform() const { return pretransform_; }

  VkExtent2D getDisplaySize() const { return displaySize_; }

  VkFormat getDisplayFormat() const { return displayFormat_; }

  VkPhysicalDeviceMemoryProperties getGpuMemProperties() { return gpuMemoryProperties_; }

  const std::vector<VkImage> &getDisplayImages() { return displayImages_; }

  VkImage getDisplayImage(int i) const;

  size_t getDisplayImagesSize() const { return displayImages_.size(); }

  uint getCurrentFrameIndex() const { return current_frame_index_; }

  const bool windowResized() { return windowResized_; }

  void setWindowResized(bool isResized) { windowResized_ = isResized; }

  void present(VkSemaphore* wait_semaphores);

  void setObjectName(uint64_t object,
                     VkDebugReportObjectTypeEXT objectType,
                     const char *name);

  void setObjectTag(uint64_t object,
                    VkDebugReportObjectTypeEXT objectType,
                    uint64_t name,
                    size_t tagSize,
                    const void *tag);

  void beginDebugRegion(VkCommandBuffer cmdbuffer, const char *markerName,
                        std::array<float, 4> color = {
                                1.0f, 1.0f, 1.0f, 1.0f});
  void insertDebugMarker(VkCommandBuffer cmdbuffer, const char *markerName,
                         std::array<float, 4> color = {
                                 1.0f, 1.0f, 1.0f, 1.0f});

  void endDebugRegion(VkCommandBuffer cmdBuffer);

  void CreateSwapChain(VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);

 private:
  bool initialized_;
  bool windowResized_ = false;

  VkInstance instance_;
  VkPhysicalDevice gpuDevice_;
  VkPhysicalDeviceMemoryProperties gpuMemoryProperties_;
  VkDevice device_;
  uint32_t queueFamilyIndex_;
  VkSurfaceKHR surface_;
  VkQueue queue_;

  VkSwapchainKHR swapchain_;
  uint32_t swapchainLength_;
  VkSurfaceTransformFlagBitsKHR pretransform_;

  VkExtent2D displaySize_;
  VkFormat displayFormat_;

  uint current_frame_index_ = 0;

  std::vector<VkImage> displayImages_;

  void CreateVulkanDevice(ANativeWindow *platformWindow,
                          VkApplicationInfo *appInfo);

};
}

#endif //BENDER_BASE_BENDER_KIT_HPP
