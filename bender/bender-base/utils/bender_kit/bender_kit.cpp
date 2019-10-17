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
#include <cassert>
#include "vulkan_wrapper.h"
#include "bender_kit.hpp"

// #include "BenderHelpers.hpp"


using namespace BenderKit;

// **********************************************************
// Device class member functions
//
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
    CreateSwapChain();
    initialized_ = true;
}

Device::~Device() {
    // Delete Swapchain
    for (uint32_t i = 0; i < swapchainLength_; ++i) {
        vkDestroyImageView(device_, displayViews_[i], nullptr);
        vkDestroyImage(device_, displayImages_[i], nullptr);
    }
    vkDestroySwapchainKHR(device_, swapchain_, nullptr);

    // Delete Device
    vkDestroyDevice(device_, nullptr);
    vkDestroyInstance(instance_, nullptr);
}

// Create vulkan device
void Device::CreateVulkanDevice(ANativeWindow* platformWindow,
                        VkApplicationInfo* appInfo) {
    LOGI("->CreateVulkanDevice");
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
    LOGI("<-CreateVulkanDevice");
}

void Device::CreateSwapChain() {
    LOGI("->createSwapChain");

    // **********************************************************
    // Get the surface capabilities because:
    //   - It contains the minimal and max length of the chain, we will need it
    //   - It's necessary to query the supported surface format (R8G8B8A8 for
    //   instance ...)
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpuDevice_, surface_,
                                              &surfaceCapabilities);
    // Query the list of supported surface format and choose one we like
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpuDevice_, surface_,
                                         &formatCount, nullptr);
    VkSurfaceFormatKHR* formats = new VkSurfaceFormatKHR[formatCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpuDevice_, surface_,
                                         &formatCount, formats);
    LOGI("Got %d formats", formatCount);

    uint32_t chosenFormat;
    for (chosenFormat = 0; chosenFormat < formatCount; chosenFormat++) {
        if (formats[chosenFormat].format == VK_FORMAT_R8G8B8A8_SRGB) break;
    }
    assert(chosenFormat < formatCount);

    displaySize_ = surfaceCapabilities.currentExtent;
    displayFormat_ = formats[chosenFormat].format;

    // **********************************************************
    // Create a swap chain (here we choose the minimum available number of surface
    // in the chain)
    VkSwapchainCreateInfoKHR swapchainCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .surface = surface_,
            .minImageCount = surfaceCapabilities.minImageCount,
            .imageFormat = formats[chosenFormat].format,
            .imageColorSpace = formats[chosenFormat].colorSpace,
            .imageExtent = surfaceCapabilities.currentExtent,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .imageArrayLayers = 1,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &queueFamilyIndex_,
            .presentMode = VK_PRESENT_MODE_FIFO_KHR,
            .oldSwapchain = VK_NULL_HANDLE,
            .clipped = VK_FALSE,
    };
    CALL_VK(vkCreateSwapchainKHR(device_, &swapchainCreateInfo, nullptr,
                                 &swapchain_));

    // Get the length of the created swap chain
    CALL_VK(vkGetSwapchainImagesKHR(device_, swapchain_,
                                    &swapchainLength_, nullptr));
    delete[] formats;
    LOGI("<-createSwapChain");
}

void Device::CreateImageView(){
    LOGI("->CreateImageView");
    // query display attachment to swapchain
    uint32_t SwapchainImagesCount = 0;
    CALL_VK(vkGetSwapchainImagesKHR(device_, swapchain_,
                                    &SwapchainImagesCount, nullptr));
    displayImages_.resize(SwapchainImagesCount);
    CALL_VK(vkGetSwapchainImagesKHR(device_, swapchain_,
                                    &SwapchainImagesCount,
                                    displayImages_.data()));

    // create image view for each swapchain image
    displayViews_.resize(SwapchainImagesCount);
    for (uint32_t i = 0; i < SwapchainImagesCount; i++) {
        VkImageViewCreateInfo viewCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .image = displayImages_[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = displayFormat_,
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
        CALL_VK(vkCreateImageView(device_, &viewCreateInfo, nullptr,
                                  &displayViews_[i]));
    }
    LOGI("<-CreateImageView");

}
