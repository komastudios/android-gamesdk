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


#include <android_native_app_glue.h>
#include <vector>
#include <cassert>
#include "vulkan_wrapper.h"
#include "bender_kit.hpp"

// #include "BenderHelpers.hpp"


using namespace BenderKit;

Device::Device(ANativeWindow* window) {
    if (!InitVulkan()) {
        LOGW("Vulkan is unavailable, install vulkan and re-start");
        initialized_ = false;
        return;
    }

    // parameterize version number?
    VkApplicationInfo appInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .apiVersion = VK_MAKE_VERSION(1, 0, 0),
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .pApplicationName = "bender_main_window",
            .pEngineName = "bender",
    };

    CreateVulkanDevice(window, &appInfo);
    initialized_ = true;
}

Device::~Device() {
    vkDestroyDevice(device_, nullptr);
    vkDestroyInstance(instance_, nullptr);
}

// Create vulkan device
void Device::CreateVulkanDevice(ANativeWindow* platformWindow,
                        VkApplicationInfo* appInfo) {
    std::vector<const char*> instance_extensions;
    std::vector<const char*> device_extensions;

    instance_extensions.push_back("VK_KHR_surface");
    instance_extensions.push_back("VK_KHR_android_surface");

    device_extensions.push_back("VK_KHR_swapchain");

    // **********************************************************
    // Create the Vulkan instance
    VkInstanceCreateInfo instanceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .pApplicationInfo = appInfo,
            .enabledExtensionCount =
            static_cast<uint32_t>(instance_extensions.size()),
            .ppEnabledExtensionNames = instance_extensions.data(),
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
    };
    CALL_VK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance_));
    VkAndroidSurfaceCreateInfoKHR createInfo{
            .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .window = platformWindow};

    CALL_VK(vkCreateAndroidSurfaceKHR(instance_, &createInfo, nullptr,
                                      &surface_));
    // Find one GPU to use:

    // On Android, every GPU device is equal -- supporting
    // graphics/compute/present
    // for this sample, we use the very first GPU device found on the system
    uint32_t gpuCount = 0;
    CALL_VK(vkEnumeratePhysicalDevices(instance_, &gpuCount, nullptr));
    VkPhysicalDevice tmpGpus[gpuCount];
    CALL_VK(vkEnumeratePhysicalDevices(instance_, &gpuCount, tmpGpus));
    gpuDevice_ = tmpGpus[0];  // Pick up the first GPU Device

    // Find a GFX queue family
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(gpuDevice_, &queueFamilyCount,
                                             nullptr);
    assert(queueFamilyCount);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gpuDevice_, &queueFamilyCount,
                                             queueFamilyProperties.data());

    uint32_t queueFamilyIndex;
    for (queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount;
         queueFamilyIndex++) {
        if (queueFamilyProperties[queueFamilyIndex].queueFlags &
            VK_QUEUE_GRAPHICS_BIT) {
            break;
        }
    }
    assert(queueFamilyIndex < queueFamilyCount);
    queueFamilyIndex_ = queueFamilyIndex;

    // Create a logical device (vulkan device)
    float priorities[] = {
            1.0f,
    };
    VkDeviceQueueCreateInfo queueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCount = 1,
            .queueFamilyIndex = queueFamilyIndex,
            .pQueuePriorities = priorities,
    };

    VkDeviceCreateInfo deviceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queueCreateInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(device_extensions.size()),
            .ppEnabledExtensionNames = device_extensions.data(),
            .pEnabledFeatures = nullptr,
    };

    CALL_VK(vkCreateDevice(gpuDevice_, &deviceCreateInfo, nullptr,
                           &device_));
    vkGetDeviceQueue(device_, queueFamilyIndex_, 0, &queue_);
}



