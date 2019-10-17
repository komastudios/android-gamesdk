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
#include <vector>

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

        // Accessor for VkDeviceInfo
        VkPhysicalDevice &getPhysicalDevice() { return gpuDevice_; }

        VkDevice &getDevice() { return device_; }

        uint32_t &getQueueFamilyIndex() { return queueFamilyIndex_; }

        VkSurfaceKHR &getSurface() { return surface_; }

        VkQueue &getQueue() { return queue_; }

        // Accessor for VkSwapchainInfo
        VkSwapchainKHR &getSwapchain() { return swapchain_; }

        uint32_t &getSwapchainLength() { return swapchainLength_; }

        VkExtent2D &getDisplaySize() { return displaySize_; }

        VkFormat &getDisplayFormat() { return displayFormat_; }

        std::vector<VkImage> &getDisplayImages() { return displayImages_; }

        std::vector<VkImageView> &getDisplayViews() { return displayViews_; }

        std::vector<VkFramebuffer> &getFrameBuffers() { return framebuffers_; }


    private:
        // from VkDeviceInfo
        bool initialized_;
        VkInstance instance_;
        VkPhysicalDevice gpuDevice_;
        VkDevice device_;
        uint32_t queueFamilyIndex_;
        VkSurfaceKHR surface_;
        VkQueue queue_;

        // from VulkanSwapchainInfo
        VkSwapchainKHR swapchain_;
        uint32_t swapchainLength_;

        VkExtent2D displaySize_;
        VkFormat displayFormat_;

        // array of frame buffers and views
        std::vector<VkImage> displayImages_;
        std::vector<VkImageView> displayViews_;
        std::vector<VkFramebuffer> framebuffers_;

        void CreateVulkanDevice(ANativeWindow *platformWindow,
                                VkApplicationInfo *appInfo);

        void CreateSwapChain();
    };
}

#endif //BENDER_BASE_BENDER_KIT_HPP
