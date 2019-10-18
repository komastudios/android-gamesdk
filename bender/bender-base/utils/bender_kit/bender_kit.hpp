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

#ifndef BENDER_BASE_BENDER_KIT_HPP
#define BENDER_BASE_BENDER_KIT_HPP

#include <android/log.h>
#include <string>

// Android log function wrappers
static const char* kTAG = "Bender";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

// Vulkan call wrapper
#define CALL_VK(func)                                                 \
  if (VK_SUCCESS != (func)) {                                         \
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
      void setInitialized(bool flag) { initialized_ = flag; }
      VkPhysicalDevice& getPhysicalDevice() { return gpuDevice_; }
      VkDevice& getDevice() { return device_; }
      uint32_t& getQueueFamilyIndex() { return queueFamilyIndex_; }
      VkSurfaceKHR& getSurface() { return surface_; }
      VkQueue& getQueue() { return queue_;}

  private:
      // from VkDeviceInfo
      bool initialized_;
      VkInstance instance_;
      VkPhysicalDevice gpuDevice_;
      VkDevice device_;
      uint32_t queueFamilyIndex_;
      VkSurfaceKHR surface_;
      VkQueue queue_;

      void CreateVulkanDevice(ANativeWindow *platformWindow,
                              VkApplicationInfo *appInfo);
  };
}

namespace DebugMarker {
    void setup(VkDevice device, VkPhysicalDevice physicalDevice);
    void setObjectName(VkDevice device, uint64_t object, VkDebugReportObjectTypeEXT objectType, const char *name);
    void setObjectTag(VkDevice device, uint64_t object, VkDebugReportObjectTypeEXT objectType, uint64_t name, size_t tagSize, const void* tag);
    void beginRegion(VkCommandBuffer cmdbuffer, const char* pMarkerName, float color[4]);
    void insert(VkCommandBuffer cmdbuffer, std::string markerName, float color[4]);
    void endRegion(VkCommandBuffer cmdBuffer);
}
#endif //BENDER_BASE_BENDER_KIT_HPP
