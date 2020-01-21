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

#include "device.h"
#include "debug_marker.h"
#include "bender_helpers.h"
#include "timing.h"

using namespace benderkit;
using namespace benderhelpers;

// **********************************************************
// Device class member functions
//
Device::Device(ANativeWindow *window) {
  timing::timer.Time("Create Bender Device", timing::OTHER, [this, window](){
    if (!InitVulkan()) {
      LOGW("Vulkan is unavailable, install vulkan and re-start");
      initialized_ = false;
      return;
    }

    // parameterize version number?
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .apiVersion = VK_MAKE_VERSION(1, 0, 0),
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .pApplicationName = "bender_main_window",
        .pEngineName = "bender",
    };

    CreateVulkanDevice(window, &app_info);

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu_device_, surface_,
                                              &surfaceCapabilities);
    uint32_t new_width = surfaceCapabilities.currentExtent.width;
    uint32_t new_height = surfaceCapabilities.currentExtent.height;
    if (surfaceCapabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR ||
        surfaceCapabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
      // Do not change the swapchain dimensions
      surfaceCapabilities.currentExtent.height = new_width;
      surfaceCapabilities.currentExtent.width = new_height;
    }
    display_size_identity_ = surfaceCapabilities.currentExtent;

    CreateSwapChain();

    initialized_ = true;
  });
}

Device::~Device() {
  vkDestroySwapchainKHR(device_, swapchain_, nullptr);
  vkDestroySurfaceKHR(instance_, surface_, nullptr);

  // Delete Device
  vkDestroyDevice(device_, nullptr);
  vkDestroyInstance(instance_, nullptr);
}

void Device::Present(VkSemaphore* wait_semaphores) {
  VkResult result;
  VkSwapchainKHR swapchains[] = { GetSwapchain() };
  VkPresentInfoKHR present_info {
          .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
          .pNext = nullptr,
          .swapchainCount = 1,
          .pSwapchains = swapchains,
          .pImageIndices = &current_frame_index_,
          .waitSemaphoreCount = 1,
          .pWaitSemaphores = wait_semaphores,
          .pResults = &result,
  };

  auto res = vkQueuePresentKHR(queue_, &present_info);
  if (res == VK_SUBOPTIMAL_KHR){
    window_resized_ = true;
  }
  current_frame_index_ = (current_frame_index_ + 1) % GetDisplayImages().size();
}

void Device::CreateVulkanDevice(ANativeWindow *platform_window,
                                VkApplicationInfo *app_info) {
  timing::timer.Time("Create Vulkan Device", timing::OTHER, [this, platform_window, app_info](){
    std::vector<const char *> instance_extensions;
    std::vector<const char *> instance_layers;
    std::vector<const char *> device_extensions;

    instance_extensions.push_back("VK_KHR_surface");
    instance_extensions.push_back("VK_KHR_android_surface");

#ifdef ENABLE_VALIDATION_LAYERS
    instance_extensions.push_back("VK_EXT_debug_report");
    instance_layers.push_back("VK_LAYER_KHRONOS_validation");
    device_extensions.push_back("VK_EXT_debug_marker");
#endif

    device_extensions.push_back("VK_KHR_swapchain");

    // **********************************************************
    // Create the Vulkan instance
    VkInstanceCreateInfo instance_create_info {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .pApplicationInfo = app_info,
        .enabledExtensionCount =
        static_cast<uint32_t>(instance_extensions.size()),
        .ppEnabledExtensionNames = instance_extensions.data(),
        .enabledLayerCount = static_cast<uint32_t>(instance_layers.size()),
        .ppEnabledLayerNames = instance_layers.data(),
    };
    CALL_VK(vkCreateInstance(&instance_create_info, nullptr, &instance_));
    VkAndroidSurfaceCreateInfoKHR create_info {
        .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .window = platform_window};

    CALL_VK(vkCreateAndroidSurfaceKHR(instance_, &create_info, nullptr,
                                      &surface_));
    // Find one GPU to use:

    // On Android, every GPU device is equal -- supporting
    // graphics/compute/present
    // for this sample, we use the very first GPU device found on the system
    uint32_t gpu_count = 0;
    CALL_VK(vkEnumeratePhysicalDevices(instance_, &gpu_count, nullptr));
    VkPhysicalDevice tmp_gpus[gpu_count];
    CALL_VK(vkEnumeratePhysicalDevices(instance_, &gpu_count, tmp_gpus));
    gpu_device_ = tmp_gpus[0];  // Pick up the first GPU Device
    vkGetPhysicalDeviceMemoryProperties(gpu_device_,
                                        &gpu_memory_properties_);

    // Find a GFX queue family
    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu_device_, &queue_family_count,
                                             nullptr);
    assert(queue_family_count);
    std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu_device_, &queue_family_count,
                                             queue_family_properties.data());

    uint32_t queue_family_index;
    for (queue_family_index = 0; queue_family_index < queue_family_count;
         queue_family_index++) {
      if (queue_family_properties[queue_family_index].queueFlags &
          VK_QUEUE_GRAPHICS_BIT) {
        break;
      }
    }
    assert(queue_family_index < queue_family_count);
    queue_family_index_ = queue_family_index;

    // Create a logical device (vulkan device)
    float priorities[] = {
        1.0f,
    };
    VkDeviceQueueCreateInfo queue_create_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCount = 1,
        .queueFamilyIndex = queue_family_index,
        .pQueuePriorities = priorities,
    };

    VkDeviceCreateInfo device_create_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_create_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(device_extensions.size()),
        .ppEnabledExtensionNames = device_extensions.data(),
        .pEnabledFeatures = nullptr,
    };

    CALL_VK(vkCreateDevice(gpu_device_, &device_create_info, nullptr,
                           &device_));
    vkGetDeviceQueue(device_, queue_family_index_, 0, &queue_);
    debugmarker::Setup(device_, gpu_device_);
  });
}

void Device::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer &buffer,
                  VkDeviceMemory &buffer_memory, VkMemoryPropertyFlags properties) {
  VkBufferCreateInfo buffer_info = {
          .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
          .pNext = nullptr,
          .size = size,
          .usage = usage,
          .flags = 0,
          .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
          .queueFamilyIndexCount = 1,
  };

  CALL_VK(vkCreateBuffer(device_, &buffer_info, nullptr, &buffer));

  VkMemoryRequirements mem_requirements;
  vkGetBufferMemoryRequirements(device_, buffer, &mem_requirements);

  VkMemoryAllocateInfo alloc_info = {
          .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
          .pNext = nullptr,
          .allocationSize = mem_requirements.size,
          .memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits,
                  properties, gpu_device_),
  };

  CALL_VK(vkAllocateMemory(device_, &alloc_info, nullptr, &buffer_memory));
  CALL_VK(vkBindBufferMemory(device_, buffer, buffer_memory, 0));
}

void Device::CreateSwapChain(VkSwapchainKHR oldSwapchain) {
  timing::timer.Time("Create Swapchain", timing::OTHER, [this, oldSwapchain](){
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu_device_, surface_,
                                              &surfaceCapabilities);
    pretransform_flag_ = surfaceCapabilities.currentTransform;

    // Query the list of supported surface format and choose one we like
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu_device_, surface_,
                                         &formatCount, nullptr);
    VkSurfaceFormatKHR *formats = new VkSurfaceFormatKHR[formatCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu_device_, surface_,
                                         &formatCount, formats);
    LOGI("Got %d formats", formatCount);

    uint32_t chosenFormat;
    for (chosenFormat = 0; chosenFormat < formatCount; chosenFormat++) {
      if (formats[chosenFormat].format == VK_FORMAT_R8G8B8A8_SRGB) break;
    }
    assert(chosenFormat < formatCount);

    display_format_ = formats[chosenFormat].format;

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
        .imageExtent = display_size_identity_,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = pretransform_flag_,
        .compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        .imageArrayLayers = 1,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &queue_family_index_,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .oldSwapchain = oldSwapchain,
        .clipped = VK_FALSE,
    };
    CALL_VK(vkCreateSwapchainKHR(device_, &swapchainCreateInfo, nullptr,
                                 &swapchain_));

    if (oldSwapchain != VK_NULL_HANDLE) {
      vkDestroySwapchainKHR(device_, oldSwapchain, nullptr);
    }

    // Get the length of the created swap chain
    CALL_VK(vkGetSwapchainImagesKHR(device_, swapchain_,
                                    &swapchain_length_, nullptr));
    delete[] formats;

    uint32_t SwapchainImagesCount = 0;
    CALL_VK(vkGetSwapchainImagesKHR(device_, swapchain_,
                                    &SwapchainImagesCount, nullptr));
    display_images_.resize(SwapchainImagesCount);
    CALL_VK(vkGetSwapchainImagesKHR(device_, swapchain_,
                                    &SwapchainImagesCount,
                                    display_images_.data()));

    current_frame_index_ = 0;
  });
}

void Device::SetObjectName(uint64_t object,
                   VkDebugReportObjectTypeEXT objectType,
                   const char *name) {
  debugmarker::SetObjectName(GetDevice(), object, objectType, name);
}

void Device::SetObjectTag(uint64_t object,
                  VkDebugReportObjectTypeEXT objectType,
                  uint64_t name,
                  size_t tagSize,
                  const void *tag) {
  debugmarker::SetObjectTag(GetDevice(), object, objectType, name, tagSize, tag);
}

void Device::BeginDebugRegion(VkCommandBuffer cmdbuffer, const char *pMarkerName,
                              std::array<float, 4> color) {
  debugmarker::BeginRegion(cmdbuffer, pMarkerName, color);
}

void Device::InsertDebugMarker(VkCommandBuffer cmdbuffer, const char *markerName,
                               std::array<float, 4> color) {
  debugmarker::Insert(cmdbuffer, markerName, color);
}

void Device::EndDebugRegion(VkCommandBuffer cmdBuffer) {
  debugmarker::EndRegion(cmdBuffer);
}
