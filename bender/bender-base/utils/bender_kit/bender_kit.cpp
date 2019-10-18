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
        vkDestroyImage(device_, displayImages_[i], nullptr);
    }
    vkDestroySwapchainKHR(device_, swapchain_, nullptr);

    // Delete Device
    vkDestroyDevice(device_, nullptr);
    vkDestroyInstance(instance_, nullptr);
}

VkImage& Device::getDisplayImages(int i) {
    assert(i < displayImages_.size() && i >= 0);
    return displayImages_[i];
}

// Create vulkan device
void Device::CreateVulkanDevice(ANativeWindow* platformWindow,
                        VkApplicationInfo* appInfo) {
    LOGI("->CreateVulkanDevice");
    std::vector<const char*> instance_extensions;
    std::vector<const char*> instance_layers;
    std::vector<const char*> device_extensions;

    instance_extensions.push_back("VK_KHR_surface");
    instance_extensions.push_back("VK_KHR_android_surface");
    instance_extensions.push_back("VK_EXT_debug_report");

#ifdef ENABLE_VALIDATION_LAYERS
    instance_layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    device_extensions.push_back("VK_KHR_swapchain");
    device_extensions.push_back("VK_EXT_debug_marker");

    // **********************************************************
    // Create the Vulkan instance
    VkInstanceCreateInfo instanceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .pApplicationInfo = appInfo,
            .enabledExtensionCount =
            static_cast<uint32_t>(instance_extensions.size()),
            .ppEnabledExtensionNames = instance_extensions.data(),
            .enabledLayerCount = static_cast<uint32_t>(instance_layers.size()),
            .ppEnabledLayerNames = instance_layers.data(),
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
    DebugMarker::setup(device_, gpuDevice_);
    LOGI("<-CreateVulkanDevice");
}

void Device::CreateSwapChain() {
    LOGI("->CreateSwapChain");

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

    // query display attachment to swapchain
    uint32_t SwapchainImagesCount = 0;
    CALL_VK(vkGetSwapchainImagesKHR(device_, swapchain_,
                                    &SwapchainImagesCount, nullptr));
    displayImages_.resize(SwapchainImagesCount);
    CALL_VK(vkGetSwapchainImagesKHR(device_, swapchain_,
                                    &SwapchainImagesCount,
                                    displayImages_.data()));
    LOGI("<-CreateSwapChain");
}

//Implementation based entirely on:
//https://github.com/SaschaWillems/Vulkan/blob/master/examples/debugmarker/debugmarker.cpp
namespace DebugMarker
{
    bool active = false;
    bool extensionPresent = false;

    // Get function pointers for the debug report extensions from the device
    void setup(VkDevice device, VkPhysicalDevice physicalDevice)
    {
        // Check if the debug marker extension is present (which is the case if run from a graphics debugger)
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());
        for (auto extension : extensions) {
            if (strcmp(extension.extensionName, VK_EXT_DEBUG_MARKER_EXTENSION_NAME) == 0) {
                extensionPresent = true;
                break;
            }
        }

        if (extensionPresent) {
            // The debug marker extension is not part of the core, so function pointers need to be loaded manually
            vkDebugMarkerSetObjectTagEXT = (PFN_vkDebugMarkerSetObjectTagEXT)vkGetDeviceProcAddr(device, "vkDebugMarkerSetObjectTagEXT");
            vkDebugMarkerSetObjectNameEXT = (PFN_vkDebugMarkerSetObjectNameEXT)vkGetDeviceProcAddr(device, "vkDebugMarkerSetObjectNameEXT");
            vkCmdDebugMarkerBeginEXT = (PFN_vkCmdDebugMarkerBeginEXT)vkGetDeviceProcAddr(device, "vkCmdDebugMarkerBeginEXT");
            vkCmdDebugMarkerEndEXT = (PFN_vkCmdDebugMarkerEndEXT)vkGetDeviceProcAddr(device, "vkCmdDebugMarkerEndEXT");
            vkCmdDebugMarkerInsertEXT = (PFN_vkCmdDebugMarkerInsertEXT)vkGetDeviceProcAddr(device, "vkCmdDebugMarkerInsertEXT");
            // Set flag if at least one function pointer is present
            active = (vkDebugMarkerSetObjectNameEXT != VK_NULL_HANDLE);
        }
        else {
            LOGE("Warning: VK_EXT_debug_marker not present, debug markers are disabled.");
            LOGE("Try running from inside a Vulkan graphics debugger (e.g. RenderDoc)");
        }
    }

    // Sets the debug name of an object
    // All Objects in Vulkan are represented by their 64-bit handles which are passed into this function
    // along with the object type
    void setObjectName(VkDevice device, uint64_t object, VkDebugReportObjectTypeEXT objectType, const char *name)
    {
        // Check for valid function pointer (may not be present if not running in a debugging application)
        if (active)
        {
            VkDebugMarkerObjectNameInfoEXT nameInfo = {};
            nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
            nameInfo.objectType = objectType;
            nameInfo.object = object;
            nameInfo.pObjectName = name;
            vkDebugMarkerSetObjectNameEXT(device, &nameInfo);
        }
    }

    // Set the tag for an object
    void setObjectTag(VkDevice device, uint64_t object, VkDebugReportObjectTypeEXT objectType, uint64_t name, size_t tagSize, const void* tag)
    {
        // Check for valid function pointer (may not be present if not running in a debugging application)
        if (active)
        {
            VkDebugMarkerObjectTagInfoEXT tagInfo = {};
            tagInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_TAG_INFO_EXT;
            tagInfo.objectType = objectType;
            tagInfo.object = object;
            tagInfo.tagName = name;
            tagInfo.tagSize = tagSize;
            tagInfo.pTag = tag;
            vkDebugMarkerSetObjectTagEXT(device, &tagInfo);
        }
    }

    // Start a new debug marker region
    void beginRegion(VkCommandBuffer cmdbuffer, const char* pMarkerName, float color[4])
    {
        // Check for valid function pointer (may not be present if not running in a debugging application)
        if (active)
        {
            VkDebugMarkerMarkerInfoEXT markerInfo = {};
            markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
            memcpy(markerInfo.color, &color[0], sizeof(float) * 4);
            markerInfo.pMarkerName = pMarkerName;
            vkCmdDebugMarkerBeginEXT(cmdbuffer, &markerInfo);
        }
    }

    // Insert a new debug marker into the command buffer
    void insert(VkCommandBuffer cmdbuffer, std::string markerName, float color[4])
    {
        // Check for valid function pointer (may not be present if not running in a debugging application)
        if (active)
        {
            VkDebugMarkerMarkerInfoEXT markerInfo = {};
            markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
            memcpy(markerInfo.color, &color[0], sizeof(float) * 4);
            markerInfo.pMarkerName = markerName.c_str();
            vkCmdDebugMarkerInsertEXT(cmdbuffer, &markerInfo);
        }
    }

    // End the current debug marker region
    void endRegion(VkCommandBuffer cmdBuffer)
    {
        // Check for valid function (may not be present if not runnin in a debugging application)
        if (vkCmdDebugMarkerEndEXT)
        {
            vkCmdDebugMarkerEndEXT(cmdBuffer);
        }
    }
};