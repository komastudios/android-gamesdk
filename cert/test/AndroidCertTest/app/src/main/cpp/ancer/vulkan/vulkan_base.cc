#include "vulkan_base.h"

#include <dlfcn.h>
#include <bitset>
#include <cstring>

#include "context.h"
#include "android_helper.h"

namespace ancer {
namespace vulkan {

Result Result::kSuccess = Result(VK_SUCCESS);

VulkanRequirements::VulkanRequirements() : instance_layers(),
                                           instance_extensions(),
                                           device_layers(),
                                           device_extensions() {
  std::memset(&features, 0, sizeof(features));
}

void VulkanRequirements::Swapchain() {
  InstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME);
  InstanceExtension(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
  DeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

Result Vulkan::Initialize(const VulkanRequirements & requirements) {
  VK_GOTO_FAIL(InitEntry(requirements));
  VK_GOTO_FAIL(InitInstance(requirements));
  VK_GOTO_FAIL(InitDebugReporting(requirements));

  if(HaveInstanceExtension(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME)) {
    VK_GOTO_FAIL(InitSurface(requirements));
  }

  VK_GOTO_FAIL(InitPhysicalDevice(requirements));
  VK_GOTO_FAIL(InitDevice(requirements));

  if(false) {
fail:
    Shutdown();
    return Result(VK_ERROR_INITIALIZATION_FAILED);
  }

  return Result::kSuccess;
}

void Vulkan::Shutdown() {
}

Result Vulkan::AllocateFence(Fence &fence) {
  if(vk->free_fences.size() > 0) {
    fence = vk->free_fences.back();
    vk->free_fences.pop_back();
    return Result::kSuccess;
  }

  VkFenceCreateInfo create_info = {
    /* sType */ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    /* pNext */ nullptr,
    /* flags */ 0
  };

  fence.data.reset(new Fence::Data());
  fence.data->waited = false;
  fence.data->frame = 0;
  fence.data->references = 1;
  return Result(vk->createFence(vk->device, &create_info, nullptr,
                                &fence.data->fence));
}

Result Vulkan::WaitForFence(Fence &fence, bool &completed, bool force) {
  if(fence.data == nullptr) {
    // this copy of the fence data became invalid, assume to have been waited
    // on before
    completed = true;
    return Result::kSuccess;
  }

  if(!fence.data->waited) {
    auto timeout = force ? 100000 : 0;
    auto wait = vk->waitForFences(vk->device, 1, &fence.data->fence, VK_TRUE,
                                  timeout);
    if(wait == VK_SUCCESS) {
      fence.data->waited = true;
    } else if(wait < VK_SUCCESS) {
      return Result(wait);
    }
  }

  if(!fence.data->waited) {
    completed = true;
    return Result::kSuccess;
  }

  if(fence.data->references > 0) {
    --fence.data->references;
    if(fence.data->references == 0) {
      // TODO(sarahburns@google.com): signal to run cleanup on frame
      vk->free_fences.push_back(fence);
      fence.data.reset();
    }
  }

  return Result::kSuccess;
}

Result Vulkan::SubmitToQueue(GraphicsContext &context, Fence &fence) {
  VkSubmitInfo submit_info = {
    /* sType                */ VK_STRUCTURE_TYPE_SUBMIT_INFO,
    /* pNext                */ nullptr,
    /* waitSemaphoreCount   */ 0,
    /* pWaitSemaphores      */ nullptr,
    /* pWaitDstStageMask    */ nullptr,
    /* commandBufferCount   */ 0,
    /* pCommandBuffers      */ nullptr,
    /* signalSemaphoreCount */ 0,
    /* pSignalSemaphores    */ nullptr
  };

  context.SubmitInfo(submit_info);

  VK_RETURN_FAIL(vk->queueSubmit(vk->graphics_queue, 1, &submit_info,
                                 fence.data->fence));

  return Result::kSuccess;
}

Result Vulkan::InitEntry(const VulkanRequirements & requirements) {
  void * libVulkan = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
  if(!libVulkan) return Result::kSuccess;

#define LOAD_ENTRY(NAME, ID) vk->ID = \
  reinterpret_cast<PFN_ ## NAME>(dlsym(libVulkan, #NAME));

  vk.reset(new Data);
  LOAD_ENTRY(vkCreateInstance, createInstance);
  LOAD_ENTRY(vkDestroyInstance, destroyInstance);
  LOAD_ENTRY(vkEnumeratePhysicalDevices, enumeratePhysicalDevices);
  LOAD_ENTRY(vkGetPhysicalDeviceFeatures, getPhysicalDeviceFeatures);
  LOAD_ENTRY(vkGetPhysicalDeviceFormatProperties, getPhysicalDeviceFormatProperties);
  LOAD_ENTRY(vkGetPhysicalDeviceImageFormatProperties, getPhysicalDeviceImageFormatProperties);
  LOAD_ENTRY(vkGetPhysicalDeviceProperties, getPhysicalDeviceProperties);
  LOAD_ENTRY(vkGetPhysicalDeviceQueueFamilyProperties, getPhysicalDeviceQueueFamilyProperties);
  LOAD_ENTRY(vkGetPhysicalDeviceMemoryProperties, getPhysicalDeviceMemoryProperties);
  LOAD_ENTRY(vkGetInstanceProcAddr, getInstanceProcAddr);
  LOAD_ENTRY(vkGetDeviceProcAddr, getDeviceProcAddr);
  LOAD_ENTRY(vkCreateDevice, createDevice);
  LOAD_ENTRY(vkDestroyDevice, destroyDevice);
  LOAD_ENTRY(vkEnumerateInstanceExtensionProperties, enumerateInstanceExtensionProperties);
  LOAD_ENTRY(vkEnumerateDeviceExtensionProperties, enumerateDeviceExtensionProperties);
  LOAD_ENTRY(vkEnumerateInstanceLayerProperties, enumerateInstanceLayerProperties);
  LOAD_ENTRY(vkEnumerateDeviceLayerProperties, enumerateDeviceLayerProperties);
  LOAD_ENTRY(vkGetDeviceQueue, getDeviceQueue);
  LOAD_ENTRY(vkQueueSubmit, queueSubmit);
  LOAD_ENTRY(vkQueueWaitIdle, queueWaitIdle);
  LOAD_ENTRY(vkDeviceWaitIdle, deviceWaitIdle);
  LOAD_ENTRY(vkAllocateMemory, allocateMemory);
  LOAD_ENTRY(vkFreeMemory, freeMemory);
  LOAD_ENTRY(vkMapMemory, mapMemory);
  LOAD_ENTRY(vkUnmapMemory, unmapMemory);
  LOAD_ENTRY(vkFlushMappedMemoryRanges, flushMappedMemoryRanges);
  LOAD_ENTRY(vkInvalidateMappedMemoryRanges, invalidateMappedMemoryRanges);
  LOAD_ENTRY(vkGetDeviceMemoryCommitment, getDeviceMemoryCommitment);
  LOAD_ENTRY(vkBindBufferMemory, bindBufferMemory);
  LOAD_ENTRY(vkBindImageMemory, bindImageMemory);
  LOAD_ENTRY(vkGetBufferMemoryRequirements, getBufferMemoryRequirements);
  LOAD_ENTRY(vkGetImageMemoryRequirements, getImageMemoryRequirements);
  LOAD_ENTRY(vkGetImageSparseMemoryRequirements, getImageSparseMemoryRequirements);
  LOAD_ENTRY(vkGetPhysicalDeviceSparseImageFormatProperties, getPhysicalDeviceSparseImageFormatProperties);
  LOAD_ENTRY(vkQueueBindSparse, queueBindSparse);
  LOAD_ENTRY(vkCreateFence, createFence);
  LOAD_ENTRY(vkDestroyFence, destroyFence);
  LOAD_ENTRY(vkResetFences, resetFences);
  LOAD_ENTRY(vkGetFenceStatus, getFenceStatus);
  LOAD_ENTRY(vkWaitForFences, waitForFences);
  LOAD_ENTRY(vkCreateSemaphore, createSemaphore);
  LOAD_ENTRY(vkDestroySemaphore, destroySemaphore);
  LOAD_ENTRY(vkCreateEvent, createEvent);
  LOAD_ENTRY(vkDestroyEvent, destroyEvent);
  LOAD_ENTRY(vkGetEventStatus, getEventStatus);
  LOAD_ENTRY(vkSetEvent, setEvent);
  LOAD_ENTRY(vkResetEvent, resetEvent);
  LOAD_ENTRY(vkCreateQueryPool, createQueryPool);
  LOAD_ENTRY(vkDestroyQueryPool, destroyQueryPool);
  LOAD_ENTRY(vkGetQueryPoolResults, getQueryPoolResults);
  LOAD_ENTRY(vkCreateBuffer, createBuffer);
  LOAD_ENTRY(vkDestroyBuffer, destroyBuffer);
  LOAD_ENTRY(vkCreateBufferView, createBufferView);
  LOAD_ENTRY(vkDestroyBufferView, destroyBufferView);
  LOAD_ENTRY(vkCreateImage, createImage);
  LOAD_ENTRY(vkDestroyImage, destroyImage);
  LOAD_ENTRY(vkGetImageSubresourceLayout, getImageSubresourceLayout);
  LOAD_ENTRY(vkCreateImageView, createImageView);
  LOAD_ENTRY(vkDestroyImageView, destroyImageView);
  LOAD_ENTRY(vkCreateShaderModule, createShaderModule);
  LOAD_ENTRY(vkDestroyShaderModule, destroyShaderModule);
  LOAD_ENTRY(vkCreatePipelineCache, createPipelineCache);
  LOAD_ENTRY(vkDestroyPipelineCache, destroyPipelineCache);
  LOAD_ENTRY(vkGetPipelineCacheData, getPipelineCacheData);
  LOAD_ENTRY(vkMergePipelineCaches, mergePipelineCaches);
  LOAD_ENTRY(vkCreateGraphicsPipelines, createGraphicsPipelines);
  LOAD_ENTRY(vkCreateComputePipelines, createComputePipelines);
  LOAD_ENTRY(vkDestroyPipeline, destroyPipeline);
  LOAD_ENTRY(vkCreatePipelineLayout, createPipelineLayout);
  LOAD_ENTRY(vkDestroyPipelineLayout, destroyPipelineLayout);
  LOAD_ENTRY(vkCreateSampler, createSampler);
  LOAD_ENTRY(vkDestroySampler, destroySampler);
  LOAD_ENTRY(vkCreateDescriptorSetLayout, createDescriptorSetLayout);
  LOAD_ENTRY(vkDestroyDescriptorSetLayout, destroyDescriptorSetLayout);
  LOAD_ENTRY(vkCreateDescriptorPool, createDescriptorPool);
  LOAD_ENTRY(vkDestroyDescriptorPool, destroyDescriptorPool);
  LOAD_ENTRY(vkResetDescriptorPool, resetDescriptorPool);
  LOAD_ENTRY(vkAllocateDescriptorSets, allocateDescriptorSets);
  LOAD_ENTRY(vkFreeDescriptorSets, freeDescriptorSets);
  LOAD_ENTRY(vkUpdateDescriptorSets, updateDescriptorSets);
  LOAD_ENTRY(vkCreateFramebuffer, createFramebuffer);
  LOAD_ENTRY(vkDestroyFramebuffer, destroyFramebuffer);
  LOAD_ENTRY(vkCreateRenderPass, createRenderPass);
  LOAD_ENTRY(vkDestroyRenderPass, destroyRenderPass);
  LOAD_ENTRY(vkGetRenderAreaGranularity, getRenderAreaGranularity);
  LOAD_ENTRY(vkCreateCommandPool, createCommandPool);
  LOAD_ENTRY(vkDestroyCommandPool, destroyCommandPool);
  LOAD_ENTRY(vkResetCommandPool, resetCommandPool);
  LOAD_ENTRY(vkAllocateCommandBuffers, allocateCommandBuffers);
  LOAD_ENTRY(vkFreeCommandBuffers, freeCommandBuffers);
  LOAD_ENTRY(vkBeginCommandBuffer, beginCommandBuffer);
  LOAD_ENTRY(vkEndCommandBuffer, endCommandBuffer);
  LOAD_ENTRY(vkResetCommandBuffer, resetCommandBuffer);
  LOAD_ENTRY(vkCmdBindPipeline, cmdBindPipeline);
  LOAD_ENTRY(vkCmdSetViewport, cmdSetViewport);
  LOAD_ENTRY(vkCmdSetScissor, cmdSetScissor);
  LOAD_ENTRY(vkCmdSetLineWidth, cmdSetLineWidth);
  LOAD_ENTRY(vkCmdSetDepthBias, cmdSetDepthBias);
  LOAD_ENTRY(vkCmdSetBlendConstants, cmdSetBlendConstants);
  LOAD_ENTRY(vkCmdSetDepthBounds, cmdSetDepthBounds);
  LOAD_ENTRY(vkCmdSetStencilCompareMask, cmdSetStencilCompareMask);
  LOAD_ENTRY(vkCmdSetStencilWriteMask, cmdSetStencilWriteMask);
  LOAD_ENTRY(vkCmdSetStencilReference, cmdSetStencilReference);
  LOAD_ENTRY(vkCmdBindDescriptorSets, cmdBindDescriptorSets);
  LOAD_ENTRY(vkCmdBindIndexBuffer, cmdBindIndexBuffer);
  LOAD_ENTRY(vkCmdBindVertexBuffers, cmdBindVertexBuffers);
  LOAD_ENTRY(vkCmdDraw, cmdDraw);
  LOAD_ENTRY(vkCmdDrawIndexed, cmdDrawIndexed);
  LOAD_ENTRY(vkCmdDrawIndirect, cmdDrawIndirect);
  LOAD_ENTRY(vkCmdDrawIndexedIndirect, cmdDrawIndexedIndirect);
  LOAD_ENTRY(vkCmdDispatch, cmdDispatch);
  LOAD_ENTRY(vkCmdDispatchIndirect, cmdDispatchIndirect);
  LOAD_ENTRY(vkCmdCopyBuffer, cmdCopyBuffer);
  LOAD_ENTRY(vkCmdCopyImage, cmdCopyImage);
  LOAD_ENTRY(vkCmdBlitImage, cmdBlitImage);
  LOAD_ENTRY(vkCmdCopyBufferToImage, cmdCopyBufferToImage);
  LOAD_ENTRY(vkCmdCopyImageToBuffer, cmdCopyImageToBuffer);
  LOAD_ENTRY(vkCmdUpdateBuffer, cmdUpdateBuffer);
  LOAD_ENTRY(vkCmdFillBuffer, cmdFillBuffer);
  LOAD_ENTRY(vkCmdClearColorImage, cmdClearColorImage);
  LOAD_ENTRY(vkCmdClearDepthStencilImage, cmdClearDepthStencilImage);
  LOAD_ENTRY(vkCmdClearAttachments, cmdClearAttachments);
  LOAD_ENTRY(vkCmdResolveImage, cmdResolveImage);
  LOAD_ENTRY(vkCmdSetEvent, cmdSetEvent);
  LOAD_ENTRY(vkCmdResetEvent, cmdResetEvent);
  LOAD_ENTRY(vkCmdWaitEvents, cmdWaitEvents);
  LOAD_ENTRY(vkCmdPipelineBarrier, cmdPipelineBarrier);
  LOAD_ENTRY(vkCmdBeginQuery, cmdBeginQuery);
  LOAD_ENTRY(vkCmdEndQuery, cmdEndQuery);
  LOAD_ENTRY(vkCmdResetQueryPool, cmdResetQueryPool);
  LOAD_ENTRY(vkCmdWriteTimestamp, cmdWriteTimestamp);
  LOAD_ENTRY(vkCmdCopyQueryPoolResults, cmdCopyQueryPoolResults);
  LOAD_ENTRY(vkCmdPushConstants, cmdPushConstants);
  LOAD_ENTRY(vkCmdBeginRenderPass, cmdBeginRenderPass);
  LOAD_ENTRY(vkCmdNextSubpass, cmdNextSubpass);
  LOAD_ENTRY(vkCmdEndRenderPass, cmdEndRenderPass);
  LOAD_ENTRY(vkCmdExecuteCommands, cmdExecuteCommands);

#undef LOAD_ENTRY

  return Result::kSuccess;
}

Result Vulkan::InitInstanceExtensions(const VulkanRequirements & requirements) {
  uint32_t count;
  std::vector<VkLayerProperties> layers;
  std::vector<VkExtensionProperties> extensions;

  vk->available_instance_layers.clear();
  vk->available_instance_extensions.clear();

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
  VkResult res;
  do {
    count = 0;
    VK_RETURN_FAIL(vk->enumerateInstanceLayerProperties(&count, nullptr));
    layers.resize(count);
    res = vk->enumerateInstanceLayerProperties(&count, layers.data());
    VK_RETURN_FAIL(res);
  } while(res == VK_INCOMPLETE);
  for(const auto & layer : layers) {
    vk->available_instance_layers.insert(layer.layerName);
  }

#define LOAD_EXTENSIONS(layer) \
  do { \
    count = 0; \
    VK_RETURN_FAIL(vk->enumerateInstanceExtensionProperties(layer, \
                                                            &count, nullptr)); \
    extensions.resize(count); \
    VK_RETURN_FAIL(vk->enumerateInstanceExtensionProperties(layer, \
                                                            &count, \
                                                           extensions.data())); \
    for(const auto & extension : extensions) { \
      vk->available_instance_extensions.insert(extension.extensionName); \
    } \
  } while(false)

  LOAD_EXTENSIONS(nullptr);

  // get the layer extensions
  for(const auto & layer : vk->available_instance_layers) {
    LOAD_EXTENSIONS(layer.c_str());
  }

#undef LOAD_EXTENSIONS

  // make sure we have the required instance layers
  for(const auto & layer : requirements.instance_layers) {
    if(vk->available_instance_extensions.count(layer))
      vk->enabled_instance_extensions.insert(layer);
    else
      return Result(VK_ERROR_LAYER_NOT_PRESENT);
  }

  // make sure we have the required instance extensions
  for(const auto & extension : requirements.instance_extensions) {
    if(vk->available_instance_extensions.count(extension))
      vk->enabled_instance_extensions.insert(extension);
    else
      return Result(VK_ERROR_LAYER_NOT_PRESENT);
  }

  return Result::kSuccess;
}

Result Vulkan::InitInstance(const VulkanRequirements & requirements) {
  VK_RETURN_FAIL(InitInstanceExtensions(requirements));

  // map to c_str for VkInstanceCreateInfo
  std::vector<const char *> enabled_layers(
                                       vk->enabled_instance_layers.size());
  for(const auto & layer : vk->enabled_instance_layers) {
    enabled_layers.emplace_back(layer.c_str());
  }

  // map to c_str for VkInstanceCreateInfo
  std::vector<const char *> enabled_extensions(
                                       vk->enabled_instance_extensions.size());
  for(const auto & extension : vk->enabled_instance_extensions) {
    enabled_extensions.emplace_back(extension.c_str());
  }

  VkApplicationInfo application_info = {
    /* sType              */ VK_STRUCTURE_TYPE_APPLICATION_INFO,
    /* pNext              */ nullptr,
    /* pApplicationName   */ "",
    /* applicationVersion */ 1,
    /* pEngineName        */ "",
    /* engineVersion      */ 1,
    /* apiVersion         */ VK_API_VERSION_1_0
  };
  VkInstanceCreateInfo create_info = {
    /* sType                   */ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    /* pNext                   */ nullptr,
    /* flags                   */ 0,
    /* pApplicationInfo        */ &application_info,
    /* enabledLayerCount       */
      static_cast<uint32_t>(enabled_layers.size()),
    /* ppEnabledLayerNames     */ enabled_layers.empty() ? nullptr :
                                  enabled_layers.data(),
    /* enabledExtensionCount   */
      static_cast<uint32_t>(enabled_extensions.size()),
    /* ppEnabledExtensionNames */ enabled_extensions.empty() ? nullptr :
                                  enabled_extensions.data()
  };
  VK_RETURN_FAIL(vk->createInstance(&create_info, nullptr, &vk->instance));

#define LOAD_ENTRY(NAME, ID) vk->ID = \
  reinterpret_cast<PFN_ ## NAME>(vk->getInstanceProcAddr(vk->instance, #NAME));

  if(HaveInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME)) {
    LOAD_ENTRY(vkDestroySurfaceKHR, destroySurfaceKHR);
    LOAD_ENTRY(vkGetPhysicalDeviceSurfaceSupportKHR, getPhysicalDeviceSurfaceSupportKHR);
    LOAD_ENTRY(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, getPhysicalDeviceSurfaceCapabilitiesKHR);
    LOAD_ENTRY(vkGetPhysicalDeviceSurfaceFormatsKHR, getPhysicalDeviceSurfaceFormatsKHR);
    LOAD_ENTRY(vkGetPhysicalDeviceSurfacePresentModesKHR, getPhysicalDeviceSurfacePresentModesKHR);
  }

#ifdef VK_USE_PLATFORM_ANDROID_KHR
  if(HaveInstanceExtension(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME)) {
    LOAD_ENTRY(vkCreateAndroidSurfaceKHR, createAndroidSurfaceKHR);
  }
#endif

#undef LOAD_ENTRY

  return Result::kSuccess;
}

Result Vulkan::InitDebugReporting(const VulkanRequirements & requirements) {
  // TODO(sarahburns@google.com)
  return Result::kSuccess;
}

Result Vulkan::InitPhysicalDevice(const VulkanRequirements & requirements) {
  uint32_t count = 0;
  vk->enumeratePhysicalDevices(vk->instance, &count, nullptr);

  std::vector<VkPhysicalDevice> physical_devices(count);
  vk->enumeratePhysicalDevices(vk->instance, &count, physical_devices.data());

  // use the first physical device enumerated
  vk->physical_device = VK_NULL_HANDLE;

  for(const auto & physical_device : physical_devices) {
    vk->getPhysicalDeviceFeatures(physical_device,
                                  &vk->physical_device_features);

    // TODO(sarahburns@google.com): test features for required ones

    vk->physical_device = physical_device;
  }

  vk->getPhysicalDeviceFeatures(vk->physical_device,
                                &vk->physical_device_features);

  vk->getPhysicalDeviceMemoryProperties(vk->physical_device,
                                       &vk->physical_device_memory_properties);

  vk->getPhysicalDeviceProperties(vk->physical_device,
                                  &vk->physical_device_properties);

  return Result::kSuccess;
}

Result Vulkan::InitSurface(const VulkanRequirements & requirements) {
#ifdef VK_USE_PLATFORM_ANDROID_KHR
  AndroidHelper::WindowSize(vk->window_width, vk->window_height);

  VkAndroidSurfaceCreateInfoKHR create_info = {
    /* sType  */ VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
    /* pNext  */ nullptr,
    /* flags  */ 0,
    /* window */ AndroidHelper::Window()
  };
  VK_RETURN_FAIL(vk->createAndroidSurfaceKHR(vk->instance, &create_info,
                                             nullptr, &vk->surface));
#endif

  // by now, vk->window_width, vk->window_height, and vk->surface are valid

  VK_RETURN_FAIL(vk->getPhysicalDeviceSurfaceCapabilitiesKHR(
                                                   vk->physical_device,
                                                   vk->surface,
                                                   &vk->surface_capabilities));

  uint32_t count = 0;
  VK_RETURN_FAIL(vk->getPhysicalDeviceSurfacePresentModesKHR(
                                                           vk->physical_device,
                                                           vk->surface,
                                                           &count, nullptr));

  vk->surface_present_modes.resize(count);
  VK_RETURN_FAIL(vk->getPhysicalDeviceSurfacePresentModesKHR(
                                            vk->physical_device,
                                            vk->surface,
                                            &count,
                                            vk->surface_present_modes.data()));

  count = 0;
  VK_RETURN_FAIL(vk->getPhysicalDeviceSurfaceFormatsKHR(vk->physical_device,
                                                        vk->surface,
                                                        &count, nullptr));

  vk->surface_formats.resize(count);
  VK_RETURN_FAIL(vk->getPhysicalDeviceSurfaceFormatsKHR(
                                                  vk->physical_device,
                                                  vk->surface,
                                                  &count,
                                                  vk->surface_formats.data()));

  return Result::kSuccess;
}

Result Vulkan::InitDeviceExtensions(const VulkanRequirements & requirements) {
  uint32_t count;
  VkResult res;

  std::vector<VkLayerProperties> layers;
  std::vector<VkExtensionProperties> extensions;

  vk->available_device_layers.clear();
  vk->available_device_extensions.clear();

  // get device layers
  count = 0;
  VK_RETURN_FAIL(vk->enumerateDeviceLayerProperties(vk->physical_device,
                                                    &count, nullptr));
  layers.resize(count);
  VK_RETURN_FAIL(vk->enumerateDeviceLayerProperties(vk->physical_device,
                                                    &count, layers.data()));
  for(auto const & layer : layers)
    vk->available_device_layers.insert(layer.layerName);

  // get extensions for layers (and "")
#define LOAD_EXTENSIONS(layer) \
  do { \
    count = 0; \
    VK_RETURN_FAIL(vk->enumerateDeviceExtensionProperties(vk->physical_device, \
                                                          layer, \
                                                          &count, nullptr)); \
    extensions.resize(count); \
    VK_RETURN_FAIL(vk->enumerateDeviceExtensionProperties(vk->physical_device, \
                                                          layer, \
                                                          &count, \
                                                          extensions.data())); \
    for(const auto & extension : extensions) { \
      vk->available_device_extensions.insert(extension.extensionName); \
    } \
  } while(false)

  LOAD_EXTENSIONS(nullptr);

  for(const auto & layer : vk->available_device_layers) {
    LOAD_EXTENSIONS(layer.c_str());
  }

#undef LOAD_EXTENSIONS

  // make sure we have the required device extensions
  for(const auto & layer : requirements.device_layers) {
    if(vk->available_device_extensions.count(layer))
      vk->enabled_device_extensions.insert(layer);
    else
      return Result(VK_ERROR_LAYER_NOT_PRESENT);
  }

  // make sure we have the required device extensions
  for(const auto & extension : requirements.device_extensions) {
    if(vk->available_device_extensions.count(extension))
      vk->enabled_device_extensions.insert(extension);
    else
      return Result(VK_ERROR_LAYER_NOT_PRESENT);
  }

  return Result::kSuccess;
}

namespace {
  const uint32_t VK_QUEUE_PRESENT_BIT = 1 << 31;

  struct QueueAddress {
    uint32_t queue_family_index;
    uint32_t index;
  };

  struct QueueResolver {
    std::vector<VkQueueFamilyProperties> families;
    std::vector<uint32_t> used;
    std::vector<VkDeviceQueueCreateInfo> create_info;
    std::vector<float> priorities;
    Vulkan & vulkan;

    QueueResolver(Vulkan & _vulkan,
                  std::vector<VkQueueFamilyProperties> && _families) :
      vulkan(_vulkan),
      families(std::move(_families)),
      used(families.size()),
      create_info() {
    }

    QueueAddress Allocate(VkQueueFlags required_flags) {
      // find the queue family with minimal diviation from required_flags
      uint32_t qfi = 0;

      uint32_t best_qfi = UINT32_MAX;
      uint32_t best_bits = UINT32_MAX;
      for(; qfi < families.size(); ++qfi) {
        uint32_t flags = families[qfi].queueFlags;

        if(vulkan.HaveInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME)) {
          VkBool32 present_support = VK_FALSE;
          VK_GOTO_FAIL(vulkan.vk->getPhysicalDeviceSurfaceSupportKHR(
                                                 vulkan.vk->physical_device,
                                                 qfi, vulkan.vk->surface,
                                                 &present_support));
          if(present_support) {
            flags |= VK_QUEUE_PRESENT_BIT;
          }
        }

        /**
         * All commands that are allowed on a queue that supports transfer
         * operations are also allowed on a queue that supports either
         * graphics or compute operations. Thus, if the capabilities of a queue
         * family include VK_QUEUE_GRAPHICS_BIT or VK_QUEUE_COMPUTE_BIT, then
         * reporting the VK_QUEUE_TRANSFER_BIT capability separately for that
         * queue family is optional.
         */
        if((flags & VK_QUEUE_GRAPHICS_BIT) || (flags & VK_QUEUE_COMPUTE_BIT)) {
          flags |= VK_QUEUE_TRANSFER_BIT;
        }

        if((flags & required_flags) == 0)
          continue;

        if(used[qfi] >= families[qfi].queueCount)
          continue;

        const uint32_t num_bits = std::bitset<8>(flags).count();

        if(best_qfi == UINT32_MAX || num_bits < best_bits) {
          best_qfi = qfi;
          best_bits = num_bits;
        }
      }

      if(best_qfi == UINT32_MAX)
        fail:
        return QueueAddress { UINT32_MAX, UINT32_MAX };

      priorities.push_back(0.0f);

      uint32_t index = used[best_qfi];
      ++used[best_qfi];
      return QueueAddress { best_qfi, index };
    }

    void Finalize() {
      uint32_t priority_index = 0;

      for(uint32_t qfi = 0; qfi < families.size(); ++qfi) {
        if(used[qfi] == 0)
          continue;

        const float flat_priority = 1.0f / float(used[qfi]);

        for(uint32_t index = 0; index < used[qfi]; ++index) {
          priorities[priority_index + index] = flat_priority;
        }

        create_info.emplace_back(VkDeviceQueueCreateInfo {
          /* sType            */ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          /* pNext            */ nullptr,
          /* flags            */ 0,
          /* queueFamilyIndex */ qfi,
          /* queueCount       */ used[qfi],
          /* pQueuePriorities */ priorities.data() + priority_index
        });

        priority_index += used[qfi];
      }
    }
  };
}

Result Vulkan::InitDevice(const VulkanRequirements & requirements) {
  VK_RETURN_FAIL(InitDeviceExtensions(requirements));

  uint32_t count = 0;
  vk->getPhysicalDeviceQueueFamilyProperties(vk->physical_device, &count,
                                             nullptr);
  std::vector<VkQueueFamilyProperties> queue_family_properties(count);
  vk->getPhysicalDeviceQueueFamilyProperties(vk->physical_device, &count,
                                             queue_family_properties.data());

  // requests for a queue for a device are resolved to satisfy some rules
  QueueResolver queue_resolver(*this, std::move(queue_family_properties));

  auto graphics = queue_resolver.Allocate(VK_QUEUE_GRAPHICS_BIT);
  auto compute = queue_resolver.Allocate(VK_QUEUE_COMPUTE_BIT);
  auto transfer = queue_resolver.Allocate(VK_QUEUE_TRANSFER_BIT);
  auto present = queue_resolver.Allocate(VK_QUEUE_PRESENT_BIT);

  // get a final queue create info
  queue_resolver.Finalize();

  // map to c_str for VkDeviceCreateInfo
  std::vector<const char *> enabled_layers(
                                       vk->enabled_instance_layers.size());
  for(const auto & layer : vk->enabled_instance_layers)
    enabled_layers.emplace_back(layer.c_str());

  // map to c_str for VkDeviceCreateInfo
  std::vector<const char *> enabled_extensions(
                                       vk->enabled_instance_extensions.size());
  for(const auto & extension : vk->enabled_instance_extensions)
    enabled_extensions.emplace_back(extension.c_str());

  VkDeviceCreateInfo create_info = {
    /* sType                   */ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    /* pNext                   */ nullptr,
    /* flags                   */ 0,
    /* queueCreateInfoCount    */
      static_cast<uint32_t>(queue_resolver.create_info.size()),
    /* pQueueCreateInfos       */ queue_resolver.create_info.data(),
    /* enabledLayerCount       */
      static_cast<uint32_t>(enabled_layers.size()),
    /* ppEnabledLayerNames     */ enabled_layers.empty() ? nullptr :
                                  enabled_layers.data(),
    /* enabledExtensionCount   */
      static_cast<uint32_t>(enabled_extensions.size()),
    /* ppEnabledExtensionNames */ enabled_extensions.empty() ? nullptr :
                                  enabled_extensions.data(),
    /* pEnabledFeatures        */ nullptr
  };

  VK_RETURN_FAIL(vk->createDevice(vk->physical_device, &create_info, nullptr,
                                  &vk->device));

#define RESOLVE(NAME) \
  do {\
    if(NAME.queue_family_index != UINT32_MAX) { \
      vk->NAME ## _queue_family_index = NAME.queue_family_index; \
      vk->getDeviceQueue(vk->device, NAME.queue_family_index, NAME.index, \
                         &vk->NAME ## _queue); \
    } \
  } while(false)

  RESOLVE(graphics);
  RESOLVE(compute);
  RESOLVE(transfer);
  RESOLVE(present);

#define LOAD_ENTRY(NAME, ID) vk->ID = \
  reinterpret_cast<PFN_ ## NAME>(vk->getDeviceProcAddr(vk->device, #NAME));

  if(HaveDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
    LOAD_ENTRY(vkCreateSwapchainKHR, createSwapchainKHR);
    LOAD_ENTRY(vkDestroySwapchainKHR, destroySwapchainKHR);
    LOAD_ENTRY(vkGetSwapchainImagesKHR, getSwapchainImagesKHR);
    LOAD_ENTRY(vkAcquireNextImageKHR, acquireNextImageKHR);
    LOAD_ENTRY(vkQueuePresentKHR, queuePresentKHR);
    LOAD_ENTRY(vkGetDeviceGroupPresentCapabilitiesKHR, getDeviceGroupPresentCapabilitiesKHR);
    LOAD_ENTRY(vkGetDeviceGroupSurfacePresentModesKHR, getDeviceGroupSurfacePresentModesKHR);
    LOAD_ENTRY(vkGetPhysicalDevicePresentRectanglesKHR, getPhysicalDevicePresentRectanglesKHR);
    LOAD_ENTRY(vkAcquireNextImage2KHR, acquireNextImage2KHR);
  }

#undef LOAD_ENTRY

  return Result::kSuccess;
}

Result Vulkan::DebugName(VkDebugReportObjectTypeEXT object_type,
                         uint64_t object, const char * format, va_list args) {
  if(vk->debug_enabled) {
    // TODO(sarahburns@google.com)
  }
  return Result::kSuccess;
}

}
}
