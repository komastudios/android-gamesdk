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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include <cstdlib>
#include <cassert>
#include <cstring>
#include "VkHelpers.hpp"

using namespace std;

VkResult InitGlobalExtensionProperties(LayerProperties &layerProps) {
    VkExtensionProperties *instanceExtensions;
    uint32_t instanceExtensionCount;
    VkResult res;
    char *layerName = nullptr;

    layerName = layerProps.properties.layerName;

    do {
        res = vkEnumerateInstanceExtensionProperties(layerName,
                                                     &instanceExtensionCount,
                                                     nullptr);
        if (res) return res;

        if (instanceExtensionCount == 0) {
            return VK_SUCCESS;
        }

        layerProps.instanceExtensions.resize(instanceExtensionCount);
        instanceExtensions = layerProps.instanceExtensions.data();
        res = vkEnumerateInstanceExtensionProperties(layerName,
                                                     &instanceExtensionCount,
                                                     instanceExtensions);
    } while (res == VK_INCOMPLETE);

    return res;
}

VkResult InitGlobalLayerProperties(struct VulkanInfo &info) {
    uint32_t instanceLayerCount;
    VkLayerProperties *vkProps = nullptr;
    VkResult res;
    // This place is the first place for samples to use Vulkan APIs.
    // Here, we are going to open Vulkan.so on the device and retrieve function
    // pointers using vulkan_wrapper helper.
    if (!InitVulkan()) {
        LOGE("Failed initializing Vulkan APIs!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    LOGI("Loaded Vulkan APIs.");

    /*
     * It's possible, though very rare, that the number of
     * instance layers could change. For example, installing something
     * could include new layers that the loader would pick up
     * between the initial query for the count and the
     * request for VkLayerProperties. The loader indicates that
     * by returning a VK_INCOMPLETE status and will update the
     * the count parameter.
     * The count parameter will be updated with the number of
     * entries loaded into the data pointer - in case the number
     * of layers went down or is smaller than the size given.
     */
    do {
        res = vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
        if (res) return res;

        if (instanceLayerCount == 0) {
            return VK_SUCCESS;
        }

        vkProps = (VkLayerProperties *) realloc(vkProps,
                                                instanceLayerCount *
                                                sizeof(VkLayerProperties));

        res = vkEnumerateInstanceLayerProperties(&instanceLayerCount, vkProps);
    } while (res == VK_INCOMPLETE);

    /*
     * Now gather the extension list for each instance layer.
     */
    for (uint32_t i = 0; i < instanceLayerCount; i++) {
        LayerProperties layerProps;
        layerProps.properties = vkProps[i];
        res = InitGlobalExtensionProperties(layerProps);
        if (res) return res;
        info.instanceLayerProperties.push_back(layerProps);
    }
    free(vkProps);

    return res;
}

VkResult InitDeviceExtensionProperties(struct VulkanInfo &info,
                                       LayerProperties &layerProps) {
    VkExtensionProperties *deviceExtensions;
    uint32_t deviceExtensionCount;
    VkResult res;
    char *layerName = nullptr;

    layerName = layerProps.properties.layerName;

    do {
        res = vkEnumerateDeviceExtensionProperties(info.gpus[0], layerName,
                                                   &deviceExtensionCount,
                                                   nullptr);
        if (res) return res;

        if (deviceExtensionCount == 0) {
            return VK_SUCCESS;
        }

        layerProps.deviceExtensions.resize(deviceExtensionCount);
        deviceExtensions = layerProps.deviceExtensions.data();
        res = vkEnumerateDeviceExtensionProperties(info.gpus[0], layerName,
                                                   &deviceExtensionCount,
                                                   deviceExtensions);
    } while (res == VK_INCOMPLETE);

    return res;
}

/*
 * Return 1 (true) if all layer names specified in check_names
 * can be found in given layer properties.
 */
VkBool32 DemoCheckLayers(const std::vector<LayerProperties> &layerProps,
                         const std::vector<const char *> &layerNames) {
    auto checkCount = static_cast<uint32_t>(layerNames.size());
    auto layerCount = static_cast<uint32_t>(layerProps.size());
    for (uint32_t i = 0; i < checkCount; i++) {
        VkBool32 found = 0;
        for (uint32_t j = 0; j < layerCount; j++) {
            if (!strcmp(layerNames[i], layerProps[j].properties.layerName)) {
                found = 1;
            }
        }
        if (!found) {
            std::cout << "Cannot find layer: " << layerNames[i] << std::endl;
            return 0;
        }
    }
    return 1;
}

void InitInstanceExtensionNames(struct VulkanInfo &info) {
    info.instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    info.instanceExtensionNames.push_back(
            VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
}

VkResult InitInstance(struct VulkanInfo &info,
                      char const *const appShortName) {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = appShortName;
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = appShortName;
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instInfo = {};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pNext = nullptr;
    instInfo.flags = 0;
    instInfo.pApplicationInfo = &appInfo;
    instInfo.enabledLayerCount =
            static_cast<uint32_t>(info.instanceLayerNames.size());
    instInfo.ppEnabledLayerNames = !info.instanceLayerNames.empty()
                                   ? info.instanceLayerNames.data()
                                   : nullptr;
    instInfo.enabledExtensionCount =
            static_cast<uint32_t>(info.instanceExtensionNames.size());
    instInfo.ppEnabledExtensionNames = info.instanceExtensionNames.data();

    VkResult res = vkCreateInstance(&instInfo, nullptr, &info.inst);
    assert(res == VK_SUCCESS);

    return res;
}

void InitDeviceExtensionNames(struct VulkanInfo &info) {
    info.deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

VkResult InitDevice(struct VulkanInfo &info) {
    VkResult res;
    VkDeviceQueueCreateInfo queueInfo = {};

    float queue_priorities[1] = {0.0};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = nullptr;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queue_priorities;
    queueInfo.queueFamilyIndex = info.graphicsQueueFamilyIndex;

    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = nullptr;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount =
            static_cast<uint32_t>(info.deviceExtensionNames.size());
    deviceInfo.ppEnabledExtensionNames = deviceInfo.enabledExtensionCount
                                         ? info.deviceExtensionNames.data()
                                         : nullptr;
    deviceInfo.pEnabledFeatures = nullptr;

    res = vkCreateDevice(info.gpus[0], &deviceInfo, nullptr, &info.device);
    assert(res == VK_SUCCESS);

    return res;
}

VkResult InitEnumerateDevice(struct VulkanInfo &info, uint32_t gpuCount) {
    uint32_t const U_ASSERT_ONLY reqCount = gpuCount;
    VkResult res{vkEnumeratePhysicalDevices(info.inst, &gpuCount, nullptr)};
    assert(gpuCount);
    info.gpus.resize(gpuCount);

    res = vkEnumeratePhysicalDevices(info.inst, &gpuCount, info.gpus.data());
    assert(!res && gpuCount >= reqCount);

    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0],
                                             &info.queueFamilyCount, nullptr);
    assert(info.queueFamilyCount >= 1);

    info.queueProps.resize(info.queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0],
                                             &info.queueFamilyCount,
                                             info.queueProps.data());
    assert(info.queueFamilyCount >= 1);

    /* This is as good a place as any to do this */
    vkGetPhysicalDeviceMemoryProperties(info.gpus[0], &info.memoryProperties);
    vkGetPhysicalDeviceProperties(info.gpus[0], &info.gpuProps);
    /* query device extensions for enabled layers */
    for (auto &layerProps : info.instanceLayerProperties) {
        InitDeviceExtensionProperties(info, layerProps);
    }

    return res;
}

void InitQueueFamilyIndex(struct VulkanInfo &info) {
    /* This routine simply finds a graphics queue for a later vkCreateDevice,
     * without consideration for which queue family can present an image.
     * Do not use this if your intent is to present later in your sample,
     * instead use the InitConnection, InitWindow, InitSwapchainExtension,
     * InitDevice call sequence to get a graphics and present compatible queue
     * family
     */

    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0],
                                             &info.queueFamilyCount, nullptr);
    assert(info.queueFamilyCount >= 1);

    info.queueProps.resize(info.queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0],
                                             &info.queueFamilyCount,
                                             info.queueProps.data());
    assert(info.queueFamilyCount >= 1);

    bool found = false;
    for (unsigned int i = 0; i < info.queueFamilyCount; i++) {
        if (info.queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            info.graphicsQueueFamilyIndex = i;
            found = true;
            break;
        }
    }
    assert(found);
}

VkResult InitDebugReportCallback(struct VulkanInfo &info,
                                 PFN_vkDebugReportCallbackEXT dbgFunc) {
    VkResult res;
    VkDebugReportCallbackEXT debugReportCallback;

    info.dbgCreateDebugReportCallback =
            (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(
                    info.inst,
                    "vkCreateDebugReportCallbackEXT");
    if (!info.dbgCreateDebugReportCallback) {
        std::cout << "GetInstanceProcAddr: Unable to find "
                     "vkCreateDebugReportCallbackEXT function."
                  << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    std::cout << "Got dbgCreateDebugReportCallback function\n";

    info.dbgDestroyDebugReportCallback =
            (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(
                    info.inst,
                    "vkDestroyDebugReportCallbackEXT");
    if (!info.dbgDestroyDebugReportCallback) {
        std::cout << "GetInstanceProcAddr: Unable to find "
                     "vkDestroyDebugReportCallbackEXT function."
                  << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    std::cout << "Got dbgDestroyDebugReportCallback function\n";

    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    createInfo.pNext = nullptr;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT
                       | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    createInfo.pfnCallback = dbgFunc;
    createInfo.pUserData = nullptr;

    res = info.dbgCreateDebugReportCallback(info.inst, &createInfo, nullptr,
                                            &debugReportCallback);
    switch (res) {
        case VK_SUCCESS:
            std::cout << "Successfully created debug report callback object\n";
            info.debugReportCallbacks.push_back(debugReportCallback);
            break;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            std::cout << "dbgCreateDebugReportCallback: out of host memory"
                         " pointer\n" << std::endl;
            return VK_ERROR_INITIALIZATION_FAILED;
        default:
            std::cout << "dbgCreateDebugReportCallback: unknown failure\n"
                      << std::endl;
            return VK_ERROR_INITIALIZATION_FAILED;
    }
    return res;
}

void DestroyDebugReportCallback(struct VulkanInfo &info) {
    while (!info.debugReportCallbacks.empty()) {
        info.dbgDestroyDebugReportCallback(info.inst,
                                           info.debugReportCallbacks.back(),
                                           nullptr);
        info.debugReportCallbacks.pop_back();
    }
}

void InitConnection(struct VulkanInfo &info) {}

// Android implementation.
void InitWindow(struct VulkanInfo &info) {}

void DestroyWindow(struct VulkanInfo &info) {}

void InitWindowSize(struct VulkanInfo &info, int32_t default_width,
                    int32_t default_height) {
    AndroidGetWindowSize(&info.width, &info.height);
}

void InitDepthBuffer(struct VulkanInfo &info) {
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY pass;
    VkImageCreateInfo imageInfo = {};

    // Depth format needs to be VK_FORMAT_D24_UNORM_S8_UINT on Android.
    info.depth.format = VK_FORMAT_D24_UNORM_S8_UINT;

    const VkFormat depthFormat = info.depth.format;
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(info.gpus[0], depthFormat, &props);
    if (props.linearTilingFeatures
        & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
    } else if (props.optimalTilingFeatures
               & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    } else {
        /* Try other depth formats? */
        std::cout << "depthFormat " << depthFormat << " Unsupported.\n";
        exit(-1);
    }

    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = nullptr;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = depthFormat;
    imageInfo.extent.width = static_cast<uint32_t>(info.width);
    imageInfo.extent.height = static_cast<uint32_t>(info.height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = NUM_SAMPLES;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.queueFamilyIndexCount = 0;
    imageInfo.pQueueFamilyIndices = nullptr;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.flags = 0;

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.pNext = nullptr;
    memAlloc.allocationSize = 0;
    memAlloc.memoryTypeIndex = 0;

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.pNext = nullptr;
    viewInfo.image = VK_NULL_HANDLE;
    viewInfo.format = depthFormat;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.flags = 0;

    if (depthFormat == VK_FORMAT_D16_UNORM_S8_UINT ||
        depthFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
        depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT) {
        viewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkMemoryRequirements mem_reqs;

    /* Create image */
    res = vkCreateImage(info.device, &imageInfo, nullptr, &info.depth.image);
    assert(res == VK_SUCCESS);

    vkGetImageMemoryRequirements(info.device, info.depth.image, &mem_reqs);

    memAlloc.allocationSize = mem_reqs.size;
    /* Use the memory properties to determine the type of memory required */
    pass = MemoryTypeFromProperties(info, mem_reqs.memoryTypeBits,
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                    &memAlloc.memoryTypeIndex);
    assert(pass);

    /* Allocate memory */
    res = vkAllocateMemory(info.device, &memAlloc, nullptr, &info.depth.mem);
    assert(res == VK_SUCCESS);

    /* Bind memory */
    res = vkBindImageMemory(info.device, info.depth.image, info.depth.mem, 0);
    assert(res == VK_SUCCESS);

    /* Create image view */
    viewInfo.image = info.depth.image;
    res = vkCreateImageView(info.device, &viewInfo, nullptr, &info.depth.view);
    assert(res == VK_SUCCESS);
}

void InitSwapchainExtension(struct VulkanInfo &info) {
    /* DEPENDS on InitConnection() and InitWindow() */

    VkResult U_ASSERT_ONLY res;

// Construct the surface description:
    GET_INSTANCE_PROC_ADDR(info.inst, CreateAndroidSurfaceKHR);

    VkAndroidSurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.window = AndroidGetApplicationWindow();
    res = info.fpCreateAndroidSurfaceKHR(info.inst, &createInfo, nullptr,
                                         &info.surface);
    assert(res == VK_SUCCESS);

    // Iterate over each queue to learn whether it supports presenting:
    auto *pSupportsPresent = (VkBool32 *) malloc(info.queueFamilyCount
                                                 * sizeof(VkBool32));
    for (uint32_t i = 0; i < info.queueFamilyCount; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(info.gpus[0], i, info.surface,
                                             &pSupportsPresent[i]);
    }

    // Search for a graphics and a present queue in the array of queue
    // families, try to find one that supports both
    info.graphicsQueueFamilyIndex = UINT32_MAX;
    info.presentQueueFamilyIndex = UINT32_MAX;
    for (uint32_t i = 0; i < info.queueFamilyCount; ++i) {
        if ((info.queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            if (info.graphicsQueueFamilyIndex == UINT32_MAX) {
                info.graphicsQueueFamilyIndex = i;
            }

            if (pSupportsPresent[i] == VK_TRUE) {
                info.graphicsQueueFamilyIndex = i;
                info.presentQueueFamilyIndex = i;
                break;
            }
        }
    }

    if (info.presentQueueFamilyIndex == UINT32_MAX) {
        // If didn't find a queue that supports both graphics and present, then
        // find a separate present queue.
        for (size_t i = 0; i < info.queueFamilyCount; ++i)
            if (pSupportsPresent[i] == VK_TRUE) {
                info.presentQueueFamilyIndex = static_cast<uint32_t>(i);
                break;
            }
    }
    free(pSupportsPresent);

    // Generate error if could not find queues that support graphics
    // and present
    if (info.graphicsQueueFamilyIndex == UINT32_MAX
        || info.presentQueueFamilyIndex == UINT32_MAX) {
        std::cout << "Could not find a queues for both graphics and present";
        exit(-1);
    }

    // Get the list of VkFormats that are supported:
    uint32_t formatCount;
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(info.gpus[0], info.surface,
                                               &formatCount, nullptr);
    assert(res == VK_SUCCESS);
    auto *surfFormats =
            (VkSurfaceFormatKHR *) malloc(
                    formatCount * sizeof(VkSurfaceFormatKHR));
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(info.gpus[0], info.surface,
                                               &formatCount, surfFormats);
    assert(res == VK_SUCCESS);
    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {
        info.format = VK_FORMAT_B8G8R8A8_UNORM;
    } else {
        assert(formatCount >= 1);
        info.format = surfFormats[0].format;
    }
    free(surfFormats);
}

void InitPresentableImage(struct VulkanInfo &info) {
    /* DEPENDS on InitSwapChain() */

    VkResult U_ASSERT_ONLY res;
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType =
            VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = nullptr;
    imageAcquiredSemaphoreCreateInfo.flags = 0;

    res = vkCreateSemaphore(info.device, &imageAcquiredSemaphoreCreateInfo,
                            nullptr, &info.imageAcquiredSemaphore);
    assert(!res);

    // Get the index of the next available swapchain image:
    res = vkAcquireNextImageKHR(info.device, info.swapChain, UINT64_MAX,
                                info.imageAcquiredSemaphore, VK_NULL_HANDLE,
                                &info.currentBuffer);
    assert(!res);
}

void ExecuteQueueCmdbuf(struct VulkanInfo &info,
                        const VkCommandBuffer *cmd_bufs, VkFence &fence) {
    VkResult U_ASSERT_ONLY res;

    VkPipelineStageFlags pipe_stage_flags =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo[1] = {};
    submitInfo[0].pNext = nullptr;
    submitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo[0].waitSemaphoreCount = 1;
    submitInfo[0].pWaitSemaphores = &info.imageAcquiredSemaphore;
    submitInfo[0].pWaitDstStageMask = nullptr;
    submitInfo[0].commandBufferCount = 1;
    submitInfo[0].pCommandBuffers = cmd_bufs;
    submitInfo[0].pWaitDstStageMask = &pipe_stage_flags;
    submitInfo[0].signalSemaphoreCount = 0;
    submitInfo[0].pSignalSemaphores = nullptr;

    /* Queue the command buffer for execution */
    res = vkQueueSubmit(info.graphicsQueue, 1, submitInfo, fence);
    assert(!res);
}

void ExecutePrePresentBarrier(struct VulkanInfo &info) {
    /* DEPENDS on InitSwapChain() */
    /* Add mem barrier to change layout to present */

    VkImageMemoryBarrier prePresentBarrier = {};
    prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    prePresentBarrier.pNext = nullptr;
    prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    prePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    prePresentBarrier.subresourceRange.baseMipLevel = 0;
    prePresentBarrier.subresourceRange.levelCount = 1;
    prePresentBarrier.subresourceRange.baseArrayLayer = 0;
    prePresentBarrier.subresourceRange.layerCount = 1;
    prePresentBarrier.image = info.buffers[info.currentBuffer].image;
    vkCmdPipelineBarrier(info.cmd,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &prePresentBarrier);
}

void ExecutePresentImage(struct VulkanInfo &info) {
    /* DEPENDS on InitPresentableImage() and InitSwapChain()*/
    /* Present the image in the window */

    VkResult U_ASSERT_ONLY res;
    VkPresentInfoKHR present;
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = nullptr;
    present.swapchainCount = 1;
    present.pSwapchains = &info.swapChain;
    present.pImageIndices = &info.currentBuffer;
    present.pWaitSemaphores = nullptr;
    present.waitSemaphoreCount = 0;
    present.pResults = nullptr;

    res = vkQueuePresentKHR(info.presentQueue, &present);
    assert(!res);
}

void InitSwapChain(struct VulkanInfo &info, VkImageUsageFlags usageFlags) {
    /* DEPENDS on info.cmd and info.queue initialized */

    VkResult U_ASSERT_ONLY res;
    VkSurfaceCapabilitiesKHR surfCapabilities;

    res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(info.gpus[0], info.surface,
                                                    &surfCapabilities);
    assert(res == VK_SUCCESS);

    uint32_t presentModeCount;
    res = vkGetPhysicalDeviceSurfacePresentModesKHR(info.gpus[0], info.surface,
                                                    &presentModeCount, nullptr);
    assert(res == VK_SUCCESS);
    auto *presentModes = (VkPresentModeKHR *) malloc(presentModeCount *
                                                     sizeof(VkPresentModeKHR));
    assert(presentModes);
    res = vkGetPhysicalDeviceSurfacePresentModesKHR(info.gpus[0], info.surface,
                                                    &presentModeCount,
                                                    presentModes);
    assert(res == VK_SUCCESS);

    VkExtent2D swapchainExtent;
    // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
    if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapchainExtent.width = static_cast<uint32_t>(info.width);
        swapchainExtent.height = static_cast<uint32_t>(info.height);
        if (swapchainExtent.width < surfCapabilities.minImageExtent.width) {
            swapchainExtent.width = surfCapabilities.minImageExtent.width;
        } else if (swapchainExtent.width >
                   surfCapabilities.maxImageExtent.width) {
            swapchainExtent.width = surfCapabilities.maxImageExtent.width;
        }

        if (swapchainExtent.height < surfCapabilities.minImageExtent.height) {
            swapchainExtent.height = surfCapabilities.minImageExtent.height;
        } else if (swapchainExtent.height >
                   surfCapabilities.maxImageExtent.height) {
            swapchainExtent.height = surfCapabilities.maxImageExtent.height;
        }
    } else {
        // If the surface size is defined, the swap chain size must match
        swapchainExtent = surfCapabilities.currentExtent;
    }

    // The FIFO present mode is guaranteed by the spec to be supported
    // Also note that current Android driver only supports FIFO
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    // Determine the number of VkImage's to use in the swap chain.
    // We need to acquire only 1 presentable image at at time.
    // Asking for minImageCount images ensures that we can acquire
    // 1 presentable image as long as we present it before attempting
    // to acquire another.
    uint32_t desiredNumberOfSwapChainImages = surfCapabilities.minImageCount;

    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfCapabilities.supportedTransforms
        & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        preTransform = surfCapabilities.currentTransform;
    }

    // Find a supported composite alpha mode - one of these is guaranteed to be
    // set
    VkCompositeAlphaFlagBitsKHR compositeAlpha =
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++) {
        if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
            compositeAlpha = compositeAlphaFlags[i];
            break;
        }
    }

    VkSwapchainCreateInfoKHR swapchainCi = {};
    swapchainCi.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCi.pNext = nullptr;
    swapchainCi.surface = info.surface;
    swapchainCi.minImageCount = desiredNumberOfSwapChainImages;
    swapchainCi.imageFormat = info.format;
    swapchainCi.imageExtent.width = swapchainExtent.width;
    swapchainCi.imageExtent.height = swapchainExtent.height;
    swapchainCi.preTransform = preTransform;
    swapchainCi.compositeAlpha = compositeAlpha;
    swapchainCi.imageArrayLayers = 1;
    swapchainCi.presentMode = swapchainPresentMode;
    swapchainCi.oldSwapchain = VK_NULL_HANDLE;
    swapchainCi.clipped = static_cast<VkBool32>(false);
    swapchainCi.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchainCi.imageUsage = usageFlags;
    swapchainCi.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCi.queueFamilyIndexCount = 0;
    swapchainCi.pQueueFamilyIndices = nullptr;
    uint32_t queueFamilyIndices[2] =
            {(uint32_t) info.graphicsQueueFamilyIndex,
             (uint32_t) info.presentQueueFamilyIndex};
    if (info.graphicsQueueFamilyIndex != info.presentQueueFamilyIndex) {
        // If the graphics and present queues are from different queue families,
        // we either have to explicitly transfer ownership of images between the
        // queues, or we have to create the swapchain with imageSharingMode
        // as VK_SHARING_MODE_CONCURRENT
        swapchainCi.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCi.queueFamilyIndexCount = 2;
        swapchainCi.pQueueFamilyIndices = queueFamilyIndices;
    }

    res = vkCreateSwapchainKHR(info.device, &swapchainCi, nullptr,
                               &info.swapChain);
    assert(res == VK_SUCCESS);

    res = vkGetSwapchainImagesKHR(info.device, info.swapChain,
                                  &info.swapchainImageCount, nullptr);
    assert(res == VK_SUCCESS);

    auto *swapchainImages = (VkImage *) malloc(info.swapchainImageCount *
                                               sizeof(VkImage));
    assert(swapchainImages);
    res = vkGetSwapchainImagesKHR(info.device, info.swapChain,
                                  &info.swapchainImageCount, swapchainImages);
    assert(res == VK_SUCCESS);

    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        SwapChainBuffer sc_buffer;

        VkImageViewCreateInfo colorImageView = {};
        colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        colorImageView.pNext = nullptr;
        colorImageView.format = info.format;
        colorImageView.components.r = VK_COMPONENT_SWIZZLE_R;
        colorImageView.components.g = VK_COMPONENT_SWIZZLE_G;
        colorImageView.components.b = VK_COMPONENT_SWIZZLE_B;
        colorImageView.components.a = VK_COMPONENT_SWIZZLE_A;
        colorImageView.subresourceRange.aspectMask =
                VK_IMAGE_ASPECT_COLOR_BIT;
        colorImageView.subresourceRange.baseMipLevel = 0;
        colorImageView.subresourceRange.levelCount = 1;
        colorImageView.subresourceRange.baseArrayLayer = 0;
        colorImageView.subresourceRange.layerCount = 1;
        colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
        colorImageView.flags = 0;

        sc_buffer.image = swapchainImages[i];

        colorImageView.image = sc_buffer.image;

        res = vkCreateImageView(info.device, &colorImageView, nullptr,
                                &sc_buffer.view);
        info.buffers.push_back(sc_buffer);
        assert(res == VK_SUCCESS);
    }
    free(swapchainImages);
    info.currentBuffer = 0;

    if (presentModes) {
        free(presentModes);
    }
}

void InitUniformBuffer(struct VulkanInfo &info) {
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY pass;
    float fov = glm::radians(45.0F);
    if (info.width > info.height) {
        fov *= static_cast<float>(info.height) / static_cast<float>(info.width);
    }
    info.Projection = glm::perspective(fov, static_cast<float>(info.width) /
                                            static_cast<float>(info.height),
                                       0.1F, 100.0F);
    info.View = glm::lookAt(
            glm::vec3(-5, 3, -10),  // Camera is at (-5,3,-10), in World Space
            glm::vec3(0, 0, 0),     // and looks at the origin
            glm::vec3(0, -1,
                      0)     // Head is up (set to 0,-1,0 to look upside-down)
    );
    info.Model = glm::mat4(1.0F);
    // Vulkan clip space has inverted Y and half Z.
    info.Clip = glm::mat4(1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F, 0.0F,
                          0.0F, 0.5F, 0.0F, 0.0F, 0.0F, 0.5F, 1.0F);

    info.MVP = info.Clip * info.Projection * info.View * info.Model;

    /* VULKAN_KEY_START */
    VkBufferCreateInfo bufInfo = {};
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.pNext = nullptr;
    bufInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufInfo.size = sizeof(info.MVP);
    bufInfo.queueFamilyIndexCount = 0;
    bufInfo.pQueueFamilyIndices = nullptr;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufInfo.flags = 0;
    res = vkCreateBuffer(info.device, &bufInfo, nullptr, &info.uniformData.buf);
    assert(res == VK_SUCCESS);

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(info.device, info.uniformData.buf,
                                  &memReqs);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.memoryTypeIndex = 0;

    allocInfo.allocationSize = memReqs.size;
    pass = MemoryTypeFromProperties(info, memReqs.memoryTypeBits,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    &allocInfo.memoryTypeIndex);
    assert(pass && "No mappable, coherent memory");

    res = vkAllocateMemory(info.device, &allocInfo, nullptr,
                           &(info.uniformData.mem));
    assert(res == VK_SUCCESS);

    uint8_t *pData;
    res = vkMapMemory(info.device, info.uniformData.mem, 0, memReqs.size, 0,
                      (void **) &pData);
    assert(res == VK_SUCCESS);

    memcpy(pData, &info.MVP, sizeof(info.MVP));

    vkUnmapMemory(info.device, info.uniformData.mem);

    res = vkBindBufferMemory(info.device, info.uniformData.buf,
                             info.uniformData.mem, 0);
    assert(res == VK_SUCCESS);

    info.uniformData.bufferInfo.buffer = info.uniformData.buf;
    info.uniformData.bufferInfo.offset = 0;
    info.uniformData.bufferInfo.range = sizeof(info.MVP);
}

void
InitDescriptorAndPipelineLayouts(struct VulkanInfo &info, bool useTexture,
                                 VkDescriptorSetLayoutCreateFlags
                                 descSetLayoutCreateFlags) {
    VkDescriptorSetLayoutBinding layoutBindings[2];
    layoutBindings[0].binding = 0;
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBindings[0].pImmutableSamplers = nullptr;

    if (useTexture) {
        layoutBindings[1].binding = 1;
        layoutBindings[1].descriptorType =
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layoutBindings[1].descriptorCount = 1;
        layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        layoutBindings[1].pImmutableSamplers = nullptr;
    }

    /* Next take layout bindings and use them to create a descriptor set layout
     */
    VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
    descriptorLayout.sType =
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext = nullptr;
    descriptorLayout.flags = descSetLayoutCreateFlags;
    descriptorLayout.bindingCount = useTexture ? 2 : 1;
    descriptorLayout.pBindings = layoutBindings;

    VkResult U_ASSERT_ONLY res;

    info.descLayout.resize(NUM_DESCRIPTOR_SETS);
    res = vkCreateDescriptorSetLayout(info.device, &descriptorLayout, nullptr,
                                      info.descLayout.data());
    assert(res == VK_SUCCESS);

    /* Now use the descriptor layout to create a pipeline layout */
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = nullptr;
    pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pPipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    pPipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
    pPipelineLayoutCreateInfo.pSetLayouts = info.descLayout.data();

    res = vkCreatePipelineLayout(info.device, &pPipelineLayoutCreateInfo,
                                 nullptr,
                                 &info.pipelineLayout);
    assert(res == VK_SUCCESS);
}

void InitRenderpass(struct VulkanInfo &info, bool includeDepth, bool clear,
                    VkImageLayout finalLayout) {
    /* DEPENDS on InitSwapChain() and InitDepthBuffer() */

    VkResult U_ASSERT_ONLY res;
    /* Need attachments for render target and depth buffer */
    VkAttachmentDescription attachments[2];
    attachments[0].format = info.format;
    attachments[0].samples = NUM_SAMPLES;
    attachments[0].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                  : VK_ATTACHMENT_LOAD_OP_LOAD;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = finalLayout;
    attachments[0].flags = 0;

    if (includeDepth) {
        attachments[1].format = info.depth.format;
        attachments[1].samples = NUM_SAMPLES;
        attachments[1].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                      : VK_ATTACHMENT_LOAD_OP_LOAD;
        attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[1].finalLayout =
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[1].flags = 0;
    }

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorReference;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = includeDepth ? &depthReference
                                                   : nullptr;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    VkRenderPassCreateInfo rpInfo = {};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfo.pNext = nullptr;
    rpInfo.attachmentCount = includeDepth ? 2 : 1;
    rpInfo.pAttachments = attachments;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpass;
    rpInfo.dependencyCount = 0;
    rpInfo.pDependencies = nullptr;

    res = vkCreateRenderPass(info.device, &rpInfo, nullptr, &info.renderPass);
    assert(res == VK_SUCCESS);
}

void InitFramebuffers(struct VulkanInfo &info, bool includeDepth) {
    /* DEPENDS on InitDepthBuffer(), InitRenderpass() and
     * InitSwapchainExtension() */

    VkResult U_ASSERT_ONLY res;
    VkImageView attachments[2];
    attachments[1] = info.depth.view;

    VkFramebufferCreateInfo fbInfo = {};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.pNext = nullptr;
    fbInfo.renderPass = info.renderPass;
    fbInfo.attachmentCount = includeDepth ? 2 : 1;
    fbInfo.pAttachments = attachments;
    fbInfo.width = static_cast<uint32_t>(info.width);
    fbInfo.height = static_cast<uint32_t>(info.height);
    fbInfo.layers = 1;

    uint32_t i;

    info.framebuffers = (VkFramebuffer *) malloc(
            info.swapchainImageCount * sizeof(VkFramebuffer));

    for (i = 0; i < info.swapchainImageCount; i++) {
        attachments[0] = info.buffers[i].view;
        res = vkCreateFramebuffer(info.device, &fbInfo, nullptr,
                                  &info.framebuffers[i]);
        assert(res == VK_SUCCESS);
    }
}

void InitCommandPool(struct VulkanInfo &info) {
    /* DEPENDS on InitSwapchainExtension() */
    VkResult U_ASSERT_ONLY res;

    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.pNext = nullptr;
    cmdPoolInfo.queueFamilyIndex = info.graphicsQueueFamilyIndex;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    res = vkCreateCommandPool(info.device, &cmdPoolInfo, nullptr,
                              &info.cmdPool);
    assert(res == VK_SUCCESS);
}

void InitCommandBuffer(struct VulkanInfo &info) {
    /* DEPENDS on InitSwapchainExtension() and InitCommandPool() */
    VkResult U_ASSERT_ONLY res;

    VkCommandBufferAllocateInfo cmd = {};
    cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd.pNext = nullptr;
    cmd.commandPool = info.cmdPool;
    cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd.commandBufferCount = 1;

    res = vkAllocateCommandBuffers(info.device, &cmd, &info.cmd);
    assert(res == VK_SUCCESS);
}

void ExecuteBeginCommandBuffer(struct VulkanInfo &info) {
    /* DEPENDS on InitCommandBuffer() */
    VkResult U_ASSERT_ONLY res;

    VkCommandBufferBeginInfo cmdBufInfo = {};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;
    cmdBufInfo.flags = 0;
    cmdBufInfo.pInheritanceInfo = nullptr;

    res = vkBeginCommandBuffer(info.cmd, &cmdBufInfo);
    assert(res == VK_SUCCESS);
}

void ExecuteEndCommandBuffer(struct VulkanInfo &info) {
    VkResult U_ASSERT_ONLY res;

    res = vkEndCommandBuffer(info.cmd);
    assert(res == VK_SUCCESS);
}

void ExecuteQueueCommandBuffer(struct VulkanInfo &info) {
    VkResult U_ASSERT_ONLY res;

    /* Queue the command buffer for execution */
    const VkCommandBuffer cmd_bufs[] = {info.cmd};
    VkFenceCreateInfo fenceInfo;
    VkFence drawFence;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = 0;
    vkCreateFence(info.device, &fenceInfo, nullptr, &drawFence);

    VkPipelineStageFlags pipeStageFlags =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo[1] = {};
    submitInfo[0].pNext = nullptr;
    submitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo[0].waitSemaphoreCount = 0;
    submitInfo[0].pWaitSemaphores = nullptr;
    submitInfo[0].pWaitDstStageMask = &pipeStageFlags;
    submitInfo[0].commandBufferCount = 1;
    submitInfo[0].pCommandBuffers = cmd_bufs;
    submitInfo[0].signalSemaphoreCount = 0;
    submitInfo[0].pSignalSemaphores = nullptr;

    res = vkQueueSubmit(info.graphicsQueue, 1, submitInfo, drawFence);
    assert(res == VK_SUCCESS);

    do {
        res = vkWaitForFences(info.device, 1, &drawFence, VK_TRUE,
                              FENCE_TIMEOUT);
    } while (res == VK_TIMEOUT);
    assert(res == VK_SUCCESS);

    vkDestroyFence(info.device, drawFence, nullptr);
}

void InitDeviceQueue(struct VulkanInfo &info) {
    /* DEPENDS on InitSwapchainExtension() */

    vkGetDeviceQueue(info.device, info.graphicsQueueFamilyIndex, 0,
                     &info.graphicsQueue);
    if (info.graphicsQueueFamilyIndex == info.presentQueueFamilyIndex) {
        info.presentQueue = info.graphicsQueue;
    } else {
        vkGetDeviceQueue(info.device, info.presentQueueFamilyIndex, 0,
                         &info.presentQueue);
    }
}

void InitVertexBuffer(struct VulkanInfo &info, const void *vertexData,
                      uint32_t dataSize, uint32_t dataStride,
                      bool useTexture) {
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY pass;

    VkBufferCreateInfo bufInfo = {};
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.pNext = nullptr;
    bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufInfo.size = dataSize;
    bufInfo.queueFamilyIndexCount = 0;
    bufInfo.pQueueFamilyIndices = nullptr;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufInfo.flags = 0;
    res = vkCreateBuffer(info.device, &bufInfo, nullptr,
                         &info.vertexBuffer.buf);
    assert(res == VK_SUCCESS);

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(info.device, info.vertexBuffer.buf,
                                  &memReqs);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.memoryTypeIndex = 0;

    allocInfo.allocationSize = memReqs.size;
    pass = MemoryTypeFromProperties(info, memReqs.memoryTypeBits,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    &allocInfo.memoryTypeIndex);
    assert(pass && "No mappable, coherent memory");

    res = vkAllocateMemory(info.device, &allocInfo, nullptr,
                           &(info.vertexBuffer.mem));
    assert(res == VK_SUCCESS);
    info.vertexBuffer.bufferInfo.range = memReqs.size;
    info.vertexBuffer.bufferInfo.offset = 0;

    uint8_t *pData;
    res = vkMapMemory(info.device, info.vertexBuffer.mem, 0, memReqs.size, 0,
                      (void **) &pData);
    assert(res == VK_SUCCESS);

    memcpy(pData, vertexData, dataSize);

    vkUnmapMemory(info.device, info.vertexBuffer.mem);

    res = vkBindBufferMemory(info.device, info.vertexBuffer.buf,
                             info.vertexBuffer.mem, 0);
    assert(res == VK_SUCCESS);

    info.viBinding.binding = 0;
    info.viBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    info.viBinding.stride = dataStride;

    info.viAttribs[0].binding = 0;
    info.viAttribs[0].location = 0;
    info.viAttribs[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    info.viAttribs[0].offset = 0;
    info.viAttribs[1].binding = 0;
    info.viAttribs[1].location = 1;
    info.viAttribs[1].format = useTexture ? VK_FORMAT_R32G32_SFLOAT
                                          : VK_FORMAT_R32G32B32A32_SFLOAT;
    info.viAttribs[1].offset = 16;
}

void InitDescriptorPool(struct VulkanInfo &info, bool use_texture) {
    /* DEPENDS on InitUniformBuffer() and
     * InitDescriptorAndPipelineLayouts() */

    VkResult U_ASSERT_ONLY res;
    VkDescriptorPoolSize typeCount[2];
    typeCount[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    typeCount[0].descriptorCount = 1;
    if (use_texture) {
        typeCount[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        typeCount[1].descriptorCount = 1;
    }

    VkDescriptorPoolCreateInfo descriptorPool = {};
    descriptorPool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPool.pNext = nullptr;
    descriptorPool.maxSets = 1;
    descriptorPool.poolSizeCount = use_texture ? 2 : 1;
    descriptorPool.pPoolSizes = typeCount;

    res = vkCreateDescriptorPool(info.device, &descriptorPool, nullptr,
                                 &info.descPool);
    assert(res == VK_SUCCESS);
}

void InitDescriptorSet(struct VulkanInfo &info, bool useTexture) {
    /* DEPENDS on InitDescriptorPool() */

    VkResult U_ASSERT_ONLY res;

    VkDescriptorSetAllocateInfo allocInfo[1];
    allocInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo[0].pNext = nullptr;
    allocInfo[0].descriptorPool = info.descPool;
    allocInfo[0].descriptorSetCount = NUM_DESCRIPTOR_SETS;
    allocInfo[0].pSetLayouts = info.descLayout.data();

    info.descSet.resize(NUM_DESCRIPTOR_SETS);
    res = vkAllocateDescriptorSets(info.device, allocInfo,
                                   info.descSet.data());
    assert(res == VK_SUCCESS);

    VkWriteDescriptorSet writes[2];

    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = nullptr;
    writes[0].dstSet = info.descSet[0];
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &info.uniformData.bufferInfo;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    if (useTexture) {
        writes[1] = {};
        writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[1].dstSet = info.descSet[0];
        writes[1].dstBinding = 1;
        writes[1].descriptorCount = 1;
        writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[1].pImageInfo = &info.textureData.imageInfo;
        writes[1].dstArrayElement = 0;
    }

    vkUpdateDescriptorSets(info.device, useTexture ? 2 : 1, writes, 0,
                           nullptr);
}

void InitShaders(struct VulkanInfo &info, const char *vertShaderText,
                 const char *fragShaderText) {
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY retVal;

    // If no shaders were submitted, just return
    if (!(vertShaderText || fragShaderText)) return;

    InitGlslang();
    VkShaderModuleCreateInfo moduleCreateInfo;

    if (vertShaderText) {
        std::vector<unsigned int> vtxSpv;
        info.shaderStages[0].sType =
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.shaderStages[0].pNext = nullptr;
        info.shaderStages[0].pSpecializationInfo = nullptr;
        info.shaderStages[0].flags = 0;
        info.shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        info.shaderStages[0].pName = "main";

        retVal = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, vertShaderText, vtxSpv);
        assert(retVal);

        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.pNext = nullptr;
        moduleCreateInfo.flags = 0;
        moduleCreateInfo.codeSize = vtxSpv.size() * sizeof(unsigned int);
        moduleCreateInfo.pCode = vtxSpv.data();
        res = vkCreateShaderModule(info.device, &moduleCreateInfo, nullptr,
                                   &info.shaderStages[0].module);
        assert(res == VK_SUCCESS);
    }

    if (fragShaderText) {
        std::vector<unsigned int> fragSpv;
        info.shaderStages[1].sType =
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.shaderStages[1].pNext = nullptr;
        info.shaderStages[1].pSpecializationInfo = nullptr;
        info.shaderStages[1].flags = 0;
        info.shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        info.shaderStages[1].pName = "main";

        retVal = GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderText,
                           fragSpv);
        assert(retVal);

        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.pNext = nullptr;
        moduleCreateInfo.flags = 0;
        moduleCreateInfo.codeSize = fragSpv.size() * sizeof(unsigned int);
        moduleCreateInfo.pCode = fragSpv.data();
        res = vkCreateShaderModule(info.device, &moduleCreateInfo, nullptr,
                                   &info.shaderStages[1].module);
        assert(res == VK_SUCCESS);
    }

    FinalizeGlslang();
}

void InitPipelineCache(struct VulkanInfo &info) {
    VkResult U_ASSERT_ONLY res;

    VkPipelineCacheCreateInfo pipelineCache;
    pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCache.pNext = nullptr;
    pipelineCache.initialDataSize = 0;
    pipelineCache.pInitialData = nullptr;
    pipelineCache.flags = 0;
    res = vkCreatePipelineCache(info.device, &pipelineCache, nullptr,
                                &info.pipelineCache);
    assert(res == VK_SUCCESS);
}

void InitPipeline(struct VulkanInfo &info, VkBool32 include_depth,
                  VkBool32 include_vi) {
    VkResult U_ASSERT_ONLY res;

    VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.pDynamicStates = dynamicStateEnables;
    dynamicState.dynamicStateCount = 0;

    VkPipelineVertexInputStateCreateInfo vi;
    memset(&vi, 0, sizeof(vi));
    vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    if (include_vi) {
        vi.pNext = nullptr;
        vi.flags = 0;
        vi.vertexBindingDescriptionCount = 1;
        vi.pVertexBindingDescriptions = &info.viBinding;
        vi.vertexAttributeDescriptionCount = 2;
        vi.pVertexAttributeDescriptions = info.viAttribs;
    }
    VkPipelineInputAssemblyStateCreateInfo ia;
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.pNext = nullptr;
    ia.flags = 0;
    ia.primitiveRestartEnable = VK_FALSE;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineRasterizationStateCreateInfo rs;
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.pNext = nullptr;
    rs.flags = 0;
    rs.polygonMode = VK_POLYGON_MODE_FILL;
    rs.cullMode = VK_CULL_MODE_BACK_BIT;
    rs.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rs.depthClampEnable = VK_FALSE;
    rs.rasterizerDiscardEnable = VK_FALSE;
    rs.depthBiasEnable = VK_FALSE;
    rs.depthBiasConstantFactor = 0;
    rs.depthBiasClamp = 0;
    rs.depthBiasSlopeFactor = 0;
    rs.lineWidth = 1.0F;

    VkPipelineColorBlendStateCreateInfo cb;
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb.flags = 0;
    cb.pNext = nullptr;
    VkPipelineColorBlendAttachmentState attState[1];
    attState[0].colorWriteMask = 0xf;
    attState[0].blendEnable = VK_FALSE;
    attState[0].alphaBlendOp = VK_BLEND_OP_ADD;
    attState[0].colorBlendOp = VK_BLEND_OP_ADD;
    attState[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    attState[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    attState[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    attState[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    cb.attachmentCount = 1;
    cb.pAttachments = attState;
    cb.logicOpEnable = VK_FALSE;
    cb.logicOp = VK_LOGIC_OP_NO_OP;
    cb.blendConstants[0] = 1.0F;
    cb.blendConstants[1] = 1.0F;
    cb.blendConstants[2] = 1.0F;
    cb.blendConstants[3] = 1.0F;

    VkPipelineViewportStateCreateInfo vp = {};
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.pNext = nullptr;
    vp.flags = 0;
    // Temporary disabling dynamic viewport on Android because some of drivers
    // doesn't support the feature.
    VkViewport viewports;
    viewports.minDepth = 0.0F;
    viewports.maxDepth = 1.0F;
    viewports.x = 0;
    viewports.y = 0;
    viewports.width = info.width;
    viewports.height = info.height;
    VkRect2D scissor;
    scissor.extent.width = static_cast<uint32_t>(info.width);
    scissor.extent.height = static_cast<uint32_t>(info.height);
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vp.viewportCount = NUM_VIEWPORTS;
    vp.scissorCount = NUM_SCISSORS;
    vp.pScissors = &scissor;
    vp.pViewports = &viewports;
    VkPipelineDepthStencilStateCreateInfo ds;
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.pNext = nullptr;
    ds.flags = 0;
    ds.depthTestEnable = include_depth;
    ds.depthWriteEnable = include_depth;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    ds.depthBoundsTestEnable = VK_FALSE;
    ds.stencilTestEnable = VK_FALSE;
    ds.back.failOp = VK_STENCIL_OP_KEEP;
    ds.back.passOp = VK_STENCIL_OP_KEEP;
    ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
    ds.back.compareMask = 0;
    ds.back.reference = 0;
    ds.back.depthFailOp = VK_STENCIL_OP_KEEP;
    ds.back.writeMask = 0;
    ds.minDepthBounds = 0;
    ds.maxDepthBounds = 0;
    ds.stencilTestEnable = VK_FALSE;
    ds.front = ds.back;

    VkPipelineMultisampleStateCreateInfo ms;
    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms.pNext = nullptr;
    ms.flags = 0;
    ms.pSampleMask = nullptr;
    ms.rasterizationSamples = NUM_SAMPLES;
    ms.sampleShadingEnable = VK_FALSE;
    ms.alphaToCoverageEnable = VK_FALSE;
    ms.alphaToOneEnable = VK_FALSE;
    ms.minSampleShading = 0.0;

    VkGraphicsPipelineCreateInfo pipeline;
    pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline.pNext = nullptr;
    pipeline.layout = info.pipelineLayout;
    pipeline.basePipelineHandle = VK_NULL_HANDLE;
    pipeline.basePipelineIndex = 0;
    pipeline.flags = 0;
    pipeline.pVertexInputState = &vi;
    pipeline.pInputAssemblyState = &ia;
    pipeline.pRasterizationState = &rs;
    pipeline.pColorBlendState = &cb;
    pipeline.pTessellationState = nullptr;
    pipeline.pMultisampleState = &ms;
    pipeline.pDynamicState = &dynamicState;
    pipeline.pViewportState = &vp;
    pipeline.pDepthStencilState = &ds;
    pipeline.pStages = info.shaderStages;
    pipeline.stageCount = 2;
    pipeline.renderPass = info.renderPass;
    pipeline.subpass = 0;

    res = vkCreateGraphicsPipelines(info.device, info.pipelineCache, 1,
                                    &pipeline, nullptr, &info.pipeline);
    assert(res == VK_SUCCESS);
}

void InitSampler(struct VulkanInfo &info, VkSampler &sampler) {
    VkResult U_ASSERT_ONLY res;

    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
    samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.mipLodBias = 0.0;
    samplerCreateInfo.anisotropyEnable = VK_FALSE;
    samplerCreateInfo.maxAnisotropy = 1;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerCreateInfo.minLod = 0.0;
    samplerCreateInfo.maxLod = 0.0;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    /* create sampler */
    res = vkCreateSampler(info.device, &samplerCreateInfo, nullptr, &sampler);
    assert(res == VK_SUCCESS);
}

void InitImage(struct VulkanInfo &info, TextureObject &texObj,
               const char *textureName, VkImageUsageFlags extraUsages,
               VkFormatFeatureFlags extraFeatures) {
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY pass;
    std::string filename = GetBaseDataDir();

    if (textureName == nullptr)
        filename.append("lunarg.ppm");
    else
        filename.append(textureName);

    if (!ReadPpm(filename.c_str(), texObj.texWidth, texObj.texHeight, 0,
                 nullptr)) {
        std::cout << "Try relative path\n";
        filename = "../../API-Samples/data/";
        if (textureName == nullptr)
            filename.append("lunarg.ppm");
        else
            filename.append(textureName);
        if (!ReadPpm(filename.c_str(), texObj.texWidth, texObj.texHeight, 0,
                     nullptr)) {
            std::cout << "Could not read texture file " << filename;
            exit(-1);
        }
    }

    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(info.gpus[0], VK_FORMAT_R8G8B8A8_UNORM,
                                        &formatProps);

    /* See if we can use a linear tiled image for a texture, if not, we will
     * need a staging image for the texture data */
    VkFormatFeatureFlags allFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
                                        extraFeatures);
    bool needStaging = (formatProps.linearTilingFeatures & allFeatures) !=
                       allFeatures;

    if (needStaging) {
        assert((formatProps.optimalTilingFeatures & allFeatures) ==
               allFeatures);
    }

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageCreateInfo.extent.width = static_cast<uint32_t>(texObj.texWidth);
    imageCreateInfo.extent.height = static_cast<uint32_t>(texObj.texHeight);
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = NUM_SAMPLES;
    imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    imageCreateInfo.usage =
            needStaging ? (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | extraUsages) : (
                    VK_IMAGE_USAGE_SAMPLED_BIT | extraUsages);
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.flags = 0;

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.pNext = nullptr;
    memAlloc.allocationSize = 0;
    memAlloc.memoryTypeIndex = 0;

    VkImage mappableImage;
    VkDeviceMemory mappableMemory;

    VkMemoryRequirements memReqs;

    /* Create a mappable image.  It will be the texture if linear images are ok
     * to be textures or it will be the staging image if they are not. */
    res = vkCreateImage(info.device, &imageCreateInfo, nullptr,
                        &mappableImage);
    assert(res == VK_SUCCESS);

    vkGetImageMemoryRequirements(info.device, mappableImage, &memReqs);
    assert(res == VK_SUCCESS);

    memAlloc.allocationSize = memReqs.size;

    /* Find the memory type that is host mappable */
    pass = MemoryTypeFromProperties(info, memReqs.memoryTypeBits,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    &memAlloc.memoryTypeIndex);
    assert(pass && "No mappable, coherent memory");

    /* allocate memory */
    res = vkAllocateMemory(info.device, &memAlloc, nullptr, &(mappableMemory));
    assert(res == VK_SUCCESS);

    /* bind memory */
    res = vkBindImageMemory(info.device, mappableImage, mappableMemory, 0);
    assert(res == VK_SUCCESS);

    res = vkEndCommandBuffer(info.cmd);
    assert(res == VK_SUCCESS);
    const VkCommandBuffer cmd_bufs[] = {info.cmd};
    VkFenceCreateInfo fenceInfo;
    VkFence cmdFence;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = 0;
    vkCreateFence(info.device, &fenceInfo, nullptr, &cmdFence);

    VkSubmitInfo submitInfo[1] = {};
    submitInfo[0].pNext = nullptr;
    submitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo[0].waitSemaphoreCount = 0;
    submitInfo[0].pWaitSemaphores = nullptr;
    submitInfo[0].pWaitDstStageMask = nullptr;
    submitInfo[0].commandBufferCount = 1;
    submitInfo[0].pCommandBuffers = cmd_bufs;
    submitInfo[0].signalSemaphoreCount = 0;
    submitInfo[0].pSignalSemaphores = nullptr;

    /* Queue the command buffer for execution */
    res = vkQueueSubmit(info.graphicsQueue, 1, submitInfo, cmdFence);
    assert(res == VK_SUCCESS);

    VkImageSubresource subres = {};
    subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subres.mipLevel = 0;
    subres.arrayLayer = 0;

    VkSubresourceLayout layout;
    void *data;

    /* Get the subresource layout so we know what the row pitch is */
    vkGetImageSubresourceLayout(info.device, mappableImage, &subres, &layout);

    /* Make sure command buffer is finished before mapping */
    do {
        res = vkWaitForFences(info.device, 1, &cmdFence, VK_TRUE,
                              FENCE_TIMEOUT);
    } while (res == VK_TIMEOUT);
    assert(res == VK_SUCCESS);

    vkDestroyFence(info.device, cmdFence, nullptr);

    res = vkMapMemory(info.device, mappableMemory, 0, memReqs.size, 0, &data);
    assert(res == VK_SUCCESS);

    /* Read the ppm file into the mappable image's memory */
    if (!ReadPpm(filename.c_str(), texObj.texWidth, texObj.texHeight,
                 layout.rowPitch, (unsigned char *) data)) {
        std::cout << "Could not load texture file lunarg.ppm\n";
        exit(-1);
    }

    vkUnmapMemory(info.device, mappableMemory);

    VkCommandBufferBeginInfo cmdBufInfo = {};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;
    cmdBufInfo.flags = 0;
    cmdBufInfo.pInheritanceInfo = nullptr;

    res = vkResetCommandBuffer(info.cmd, 0);
    res = vkBeginCommandBuffer(info.cmd, &cmdBufInfo);
    assert(res == VK_SUCCESS);

    if (!needStaging) {
        /* If we can use the linear tiled image as a texture, just do it */
        texObj.image = mappableImage;
        texObj.mem = mappableMemory;
        texObj.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        SetImageLayout(info, texObj.image, VK_IMAGE_ASPECT_COLOR_BIT,
                       VK_IMAGE_LAYOUT_PREINITIALIZED, texObj.imageLayout,
                       VK_PIPELINE_STAGE_HOST_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        /* No staging resources to free later */
        info.stagingImage = VK_NULL_HANDLE;
        info.stagingMemory = VK_NULL_HANDLE;
    } else {
        /* The mappable image cannot be our texture, so create an optimally
         * tiled image and blit to it */
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage =
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        res = vkCreateImage(info.device, &imageCreateInfo, nullptr,
                            &texObj.image);
        assert(res == VK_SUCCESS);

        vkGetImageMemoryRequirements(info.device, texObj.image, &memReqs);

        memAlloc.allocationSize = memReqs.size;

        /* Find memory type - dont specify any mapping requirements */
        pass = MemoryTypeFromProperties(info, memReqs.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        &memAlloc.memoryTypeIndex);
        assert(pass);

        /* allocate memory */
        res = vkAllocateMemory(info.device, &memAlloc, nullptr, &texObj.mem);
        assert(res == VK_SUCCESS);

        /* bind memory */
        res = vkBindImageMemory(info.device, texObj.image, texObj.mem, 0);
        assert(res == VK_SUCCESS);

        /* Since we're going to blit from the mappable image, set its layout to
         * SOURCE_OPTIMAL. Side effect is that this will create info.cmd */
        SetImageLayout(info, mappableImage, VK_IMAGE_ASPECT_COLOR_BIT,
                       VK_IMAGE_LAYOUT_PREINITIALIZED,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       VK_PIPELINE_STAGE_HOST_BIT,
                       VK_PIPELINE_STAGE_TRANSFER_BIT);

        /* Since we're going to blit to the texture image, set its layout to
         * DESTINATION_OPTIMAL */
        SetImageLayout(info, texObj.image, VK_IMAGE_ASPECT_COLOR_BIT,
                       VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_TRANSFER_BIT);

        VkImageCopy copyRegion;
        copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.srcSubresource.mipLevel = 0;
        copyRegion.srcSubresource.baseArrayLayer = 0;
        copyRegion.srcSubresource.layerCount = 1;
        copyRegion.srcOffset.x = 0;
        copyRegion.srcOffset.y = 0;
        copyRegion.srcOffset.z = 0;
        copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.dstSubresource.mipLevel = 0;
        copyRegion.dstSubresource.baseArrayLayer = 0;
        copyRegion.dstSubresource.layerCount = 1;
        copyRegion.dstOffset.x = 0;
        copyRegion.dstOffset.y = 0;
        copyRegion.dstOffset.z = 0;
        copyRegion.extent.width = static_cast<uint32_t>(texObj.texWidth);
        copyRegion.extent.height = static_cast<uint32_t>(texObj.texHeight);
        copyRegion.extent.depth = 1;

        /* Put the copy command into the command buffer */
        vkCmdCopyImage(info.cmd, mappableImage,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, texObj.image,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        /* Set the layout for the texture image from DESTINATION_OPTIMAL to
         * SHADER_READ_ONLY */
        texObj.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        SetImageLayout(info, texObj.image, VK_IMAGE_ASPECT_COLOR_BIT,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       texObj.imageLayout,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        /* Remember staging resources to free later */
        info.stagingImage = mappableImage;
        info.stagingMemory = mappableMemory;
    }

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.pNext = nullptr;
    viewInfo.image = VK_NULL_HANDLE;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    /* create image view */
    viewInfo.image = texObj.image;
    res = vkCreateImageView(info.device, &viewInfo, nullptr, &texObj.view);
    assert(res == VK_SUCCESS);
}

void InitTexture(struct VulkanInfo &info, const char *textureName,
                 VkImageUsageFlags extraUsages,
                 VkFormatFeatureFlags extraFeatures) {
    struct TextureObject texObj{};

    /* create image */
    InitImage(info, texObj, textureName, extraUsages, extraFeatures);

    /* create sampler */
    InitSampler(info, texObj.sampler);

    info.textures.push_back(texObj);

    /* track a description of the texture */
    info.textureData.imageInfo.imageView = info.textures.back().view;
    info.textureData.imageInfo.sampler = info.textures.back().sampler;
    info.textureData.imageInfo.imageLayout =
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void InitViewports(struct VulkanInfo &info) {
// Disable dynamic viewport on Android. Some drive has an issue with the dynamic
// viewport feature.
}

void InitScissors(struct VulkanInfo &info) {
// Disable dynamic viewport on Android. Some drive has an issue with the dynamic
// scissors feature.
}

void InitFence(struct VulkanInfo &info, VkFence &fence) {
    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = 0;
    vkCreateFence(info.device, &fenceInfo, nullptr, &fence);
}

void InitSubmitInfo(struct VulkanInfo &info, VkSubmitInfo &submitInfo,
                    VkPipelineStageFlags &pipeStageFlags) {
    submitInfo.pNext = nullptr;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &info.imageAcquiredSemaphore;
    submitInfo.pWaitDstStageMask = &pipeStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &info.cmd;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;
}

void InitPresentInfo(struct VulkanInfo &info, VkPresentInfoKHR &present) {
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = nullptr;
    present.swapchainCount = 1;
    present.pSwapchains = &info.swapChain;
    present.pImageIndices = &info.currentBuffer;
    present.pWaitSemaphores = nullptr;
    present.waitSemaphoreCount = 0;
    present.pResults = nullptr;
}

void InitClearColorAndDepth(struct VulkanInfo &info,
                            VkClearValue *clearValues) {
    clearValues[0].color.float32[0] = 0.2F;
    clearValues[0].color.float32[1] = 0.2F;
    clearValues[0].color.float32[2] = 0.2F;
    clearValues[0].color.float32[3] = 0.2F;
    clearValues[1].depthStencil.depth = 1.0F;
    clearValues[1].depthStencil.stencil = 0;
}

void InitRenderPassBeginInfo(struct VulkanInfo &info,
                             VkRenderPassBeginInfo &rp_begin) {
    rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin.pNext = nullptr;
    rp_begin.renderPass = info.renderPass;
    rp_begin.framebuffer = info.framebuffers[info.currentBuffer];
    rp_begin.renderArea.offset.x = 0;
    rp_begin.renderArea.offset.y = 0;
    rp_begin.renderArea.extent.width = static_cast<uint32_t>(info.width);
    rp_begin.renderArea.extent.height = static_cast<uint32_t>(info.height);
    rp_begin.clearValueCount = 0;
    rp_begin.pClearValues = nullptr;
}

void DestroyPipeline(struct VulkanInfo &info) {
    vkDestroyPipeline(info.device, info.pipeline, nullptr);
}

void DestroyPipelineCache(struct VulkanInfo &info) {
    vkDestroyPipelineCache(info.device, info.pipelineCache, nullptr);
}

void DestroyUniformBuffer(struct VulkanInfo &info) {
    vkDestroyBuffer(info.device, info.uniformData.buf, nullptr);
    vkFreeMemory(info.device, info.uniformData.mem, nullptr);
}

void DestroyDescriptorAndPipelineLayouts(struct VulkanInfo &info) {
    for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++)
        vkDestroyDescriptorSetLayout(info.device, info.descLayout[i], nullptr);
    vkDestroyPipelineLayout(info.device, info.pipelineLayout, nullptr);
}

void DestroyDescriptorPool(struct VulkanInfo &info) {
    vkDestroyDescriptorPool(info.device, info.descPool, nullptr);
}

void DestroyShaders(struct VulkanInfo &info) {
    vkDestroyShaderModule(info.device, info.shaderStages[0].module, nullptr);
    vkDestroyShaderModule(info.device, info.shaderStages[1].module, nullptr);
}

void DestroyCommandBuffer(struct VulkanInfo &info) {
    VkCommandBuffer cmdBufs[1] = {info.cmd};
    vkFreeCommandBuffers(info.device, info.cmdPool, 1, cmdBufs);
}

void DestroyCommandPool(struct VulkanInfo &info) {
    vkDestroyCommandPool(info.device, info.cmdPool, nullptr);
}

void DestroyDepthBuffer(struct VulkanInfo &info) {
    vkDestroyImageView(info.device, info.depth.view, nullptr);
    vkDestroyImage(info.device, info.depth.image, nullptr);
    vkFreeMemory(info.device, info.depth.mem, nullptr);
}

void DestroyVertexBuffer(struct VulkanInfo &info) {
    vkDestroyBuffer(info.device, info.vertexBuffer.buf, nullptr);
    vkFreeMemory(info.device, info.vertexBuffer.mem, nullptr);
}

void DestroySwapChain(struct VulkanInfo &info) {
    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        vkDestroyImageView(info.device, info.buffers[i].view, nullptr);
    }
    vkDestroySwapchainKHR(info.device, info.swapChain, nullptr);
}

void DestroyFramebuffers(struct VulkanInfo &info) {
    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        vkDestroyFramebuffer(info.device, info.framebuffers[i], nullptr);
    }
    free(info.framebuffers);
}

void DestroyRenderpass(struct VulkanInfo &info) {
    vkDestroyRenderPass(info.device, info.renderPass, nullptr);
}

void DestroyDevice(struct VulkanInfo &info) {
    vkDeviceWaitIdle(info.device);
    vkDestroyDevice(info.device, nullptr);
}

void DestroyInstance(struct VulkanInfo &info) {
    vkDestroyInstance(info.inst, nullptr);
}

void DestroyTextures(struct VulkanInfo &info) {
    for (size_t i = 0; i < info.textures.size(); i++) {
        vkDestroySampler(info.device, info.textures[i].sampler, nullptr);
        vkDestroyImageView(info.device, info.textures[i].view, nullptr);
        vkDestroyImage(info.device, info.textures[i].image, nullptr);
        vkFreeMemory(info.device, info.textures[i].mem, nullptr);
    }
    if (info.stagingImage) {
        vkDestroyImage(info.device, info.stagingImage, nullptr);
    }
    if (info.stagingMemory) {
        vkFreeMemory(info.device, info.stagingMemory, nullptr);
    }
}

#pragma clang diagnostic pop