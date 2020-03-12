#include "vulkan_base.h"

#include <dlfcn.h>
#include <swappy/swappyVk.h>

#include <ancer/util/Log.hpp>
#include <bitset>
#include <cstring>

#include "android_helper.h"
#include "context.h"
#include "resources.h"
#include "upload.h"

using namespace ancer;

namespace {
static Log::Tag TAG{"VulkanBase"};
}

namespace ancer {
namespace vulkan {

Result Result::kSuccess = Result(VK_SUCCESS);

VulkanRequirements::VulkanRequirements()
    : _instance_layers(),
      _instance_extensions(),
      _device_layers(),
      _device_extensions() {
  std::memset(&_features, 0, sizeof(_features));
  _upload_buffer_size = DEFAULT_UPLOAD_BUFFER_SIZE;
  _concurrent_uploads = DEFAULT_CONCRRENT_UPLOADS;
}

void VulkanRequirements::Swapchain() {
  InstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME);
  InstanceExtension(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
  DeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

Result MemoryAllocation::Invalidate(Vulkan &vk) const {
  VkMappedMemoryRange range = {
      /* sType  */ VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
      /* pNext  */ nullptr,
      /* memory */ Memory(),
      /* offset */ Offset(),
      /* size   */ Size()};

  return Result(vk->invalidateMappedMemoryRanges(vk->device, 1, &range));
}

Result Vulkan::Initialize(const VulkanRequirements &requirements) {
  VK_GOTO_FAIL(InitEntry(requirements));
  VK_GOTO_FAIL(InitInstance(requirements));
  VK_GOTO_FAIL(InitDebugReporting(requirements));

  if (HaveInstanceExtension(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME)) {
    VK_GOTO_FAIL(InitSurface(requirements));
  }

  VK_GOTO_FAIL(InitPhysicalDevice(requirements));
  VK_GOTO_FAIL(InitDevice(requirements));

  // initialize pipeline cache
  {
    VkPipelineCacheCreateInfo pipeline_cache_create_info = {
        /* sType           */ VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        /* pNext           */ nullptr,
        /* flags           */ 0,
        /* initialDataSize */ 0,
        /* pInitialData    */ nullptr};
    VK_GOTO_FAIL(vk->createPipelineCache(
        vk->device, &pipeline_cache_create_info, nullptr, &vk->pipeline_cache));
  }

  // initialize upload
  vk->uploader = new Uploader();
  VK_GOTO_FAIL(vk->uploader->Initialize(*this, requirements._concurrent_uploads,
                                        requirements._upload_buffer_size));

  vk->resources_store = new ResourcesStore();
  VK_GOTO_FAIL(vk->resources_store->Initialize(*this, 1024, 1024));

  {
    // initialize empty pipeline layout
    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
        /* sType                  */
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        /* pNext                  */ nullptr,
        /* flags                  */ 0,
        /* setLayoutCount         */ 0,
        /* pSetLayouts            */ nullptr,
        /* pushConstantRangeCount */ 0,
        /* pPushConstantRanges    */ nullptr};
    VK_GOTO_FAIL(vk->createPipelineLayout(vk->device,
                                          &pipeline_layout_create_info, nullptr,
                                          &vk->empty_pipeline_layout));
  }

  if (false) {
  fail:
    Shutdown();
    return Result(VK_ERROR_INITIALIZATION_FAILED);
  }

  return Result::kSuccess;
}

void Vulkan::Shutdown() {}

Result Vulkan::AllocateFence(Fence &fence, bool advance) {
  {
    std::lock_guard<std::mutex> guard(vk->free_fences_mutex);
    if (vk->free_fences.size() > 0) {
      fence = vk->free_fences.back();
      fence.AdvanceFrame(advance);
      vk->free_fences.pop_back();
      return Result::kSuccess;
    }
  }

  VkFenceCreateInfo create_info = {
      /* sType */ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      /* pNext */ nullptr,
      /* flags */ 0};

  fence.data = std::make_shared<Fence::Data>();
  fence.data->waited = false;
  fence.data->advance_frame = advance;
  fence.data->frame = vk->frame;
  fence.data->references = 0;
  return Result(
      vk->createFence(vk->device, &create_info, nullptr, &fence.data->fence));
}

Result Vulkan::WaitForFence(Fence &fence, bool &completed, bool force) {
  if (fence.data == nullptr) {
    // this copy of the fence data became invalid, assume to have been waited
    // on before
    completed = true;
    return Result::kSuccess;
  }

  if (!fence.data->waited) {
    auto timeout = force ? 100000 : 0;
    auto wait =
        vk->waitForFences(vk->device, 1, &fence.data->fence, VK_TRUE, timeout);
    if (wait == VK_SUCCESS) {
      fence.data->waited = true;
    } else if (wait < VK_SUCCESS) {
      return Result(wait);
    }
  }

  if (fence.data->waited) {
    completed = true;
    return Result::kSuccess;
  }

  if (fence.data->references > 0) {
    --fence.data->references;
    if (fence.data->references == 0) {
      if (fence.data->advance_frame) AdvanceFrame(fence.data->frame);

      {
        std::lock_guard<std::mutex> guard(vk->free_fences_mutex);
        vk->free_fences.push_back(fence);
      }

      fence.data.reset();
    }
  }

  return Result::kSuccess;
}

Result Vulkan::SubmitToQueue(EQueue queue, Context &context, Fence &fence) {
  VkSubmitInfo submit_info = {
      /* sType                */ VK_STRUCTURE_TYPE_SUBMIT_INFO,
      /* pNext                */ nullptr,
      /* waitSemaphoreCount   */ 0,
      /* pWaitSemaphores      */ nullptr,
      /* pWaitDstStageMask    */ nullptr,
      /* commandBufferCount   */ 0,
      /* pCommandBuffers      */ nullptr,
      /* signalSemaphoreCount */ 0,
      /* pSignalSemaphores    */ nullptr};

  context.SubmitInfo(submit_info);

  VK_RETURN_FAIL(vk->queueSubmit(vk->queue[queue].queue, 1, &submit_info,
                                 fence.data->fence));

  ++fence.data->references;

  return Result::kSuccess;
}

// ============================================================================
Result Vulkan::AllocateMemory(VkMemoryRequirements &requirements,
                              VkMemoryPropertyFlags flags,
                              MemoryAllocation &allocation) {
  // TODO(sarahburns@google.com): fix this to use pages or the AMD allocator

  VkMemoryAllocateInfo allocate_info = {
      /* sType           */ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      /* pNext           */ nullptr,
      /* allocationSize  */ requirements.size,
      /* memoryTypeIndex */ 0};

  uint32_t &mti = allocate_info.memoryTypeIndex;
  const auto &memory_properties = vk->physical_device_memory_properties;

  for (; mti < memory_properties.memoryTypeCount; ++mti) {
    if ((requirements.memoryTypeBits & (1 << mti)) == 0) continue;
    if ((memory_properties.memoryTypes[mti].propertyFlags & flags) != flags)
      continue;
    break;
  }

  VK_RETURN_FAIL(vk->allocateMemory(vk->device, &allocate_info, nullptr,
                                    &allocation._memory));

  allocation._start = 0;
  allocation._offset = 0;
  allocation._end = requirements.size;

  Result res;

  if ((flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
    VK_GOTO_FAIL(res = vk->mapMemory(vk->device, allocation._memory, 0,
                                     requirements.size, 0, &allocation._map));

  if (false) {
  fail:
    vk->freeMemory(vk->device, allocation._memory, nullptr);
    return res;
  }

  return Result::kSuccess;
}

void Vulkan::Free(const MemoryAllocation &allocation) {
  // TODO(sarahburns@google.com): fix this to use pages or the AMD allocator
  if (allocation._memory == VK_NULL_HANDLE) {
    return;
  }

  Destroy(allocation._memory);
}

// ============================================================================
Result Vulkan::GetFramebuffer(VkRenderPass render_pass, uint32_t width,
                              uint32_t height, uint32_t layers,
                              std::initializer_list<VkImageView> image_views,
                              VkFramebuffer &framebuffer) {
  uint64_t h1 = 0;
  uint64_t h2 = 0;
  std::hash<uint64_t> hasher;

  auto mash = [&](uint64_t x) {
    h1 = hasher(x ^ (h1 << 1));
    h2 = hasher(x + h2);
  };

  mash(uint64_t(render_pass));
  mash(uint64_t(width));
  mash(uint64_t(height));
  mash(uint64_t(layers));
  for (auto iv : image_views) {
    mash(uint64_t(iv));
  }

  {
    std::lock_guard<std::mutex> guard(vk->framebuffer_mutex);
    auto i = vk->framebuffers.find(h1);
    if (i != vk->framebuffers.end()) {
      assert(i->second.h1 == h1);
      assert(i->second.h2 == h2);
      i->second.frame = vk->frame + FRAMEBUFFER_SAVED_FRAMES;
      framebuffer = i->second.framebuffer;
      return Result::kSuccess;
    }
  }

  std::vector<VkImageView> image_views_(image_views);

  VkFramebufferCreateInfo create_info = {
      /* sType           */ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      /* pNext           */ nullptr,
      /* flags           */ 0,
      /* renderPass      */ render_pass,
      /* attachmentCount */ static_cast<uint32_t>(image_views_.size()),
      /* pAttachments    */ image_views_.data(),
      /* width           */ width,
      /* height          */ height,
      /* layers          */ layers};

  VK_RETURN_FAIL(
      vk->createFramebuffer(vk->device, &create_info, nullptr, &framebuffer));

  {
    std::lock_guard<std::mutex> guard(vk->framebuffer_mutex);
    vk->framebuffers[h1] =
        Framebuffer{/* h1          */ h1,
                    /* h2          */ h2,
                    /* frame       */ vk->frame + FRAMEBUFFER_SAVED_FRAMES,
                    /* framebuffer */ framebuffer};
  }

  return Result::kSuccess;
}

// ============================================================================
Result Vulkan::AcquireTemporaryCommandBuffer(EQueue queue,
                                             VkCommandBuffer &cmd_buffer,
                                             bool fresh) {
  auto thread_data = GetThreadData();

  VkCommandPool command_pool = thread_data->temporary_command_pool[queue];

  // find or create the command pool for this (thread, qfi)
  if (command_pool == VK_NULL_HANDLE) {
    VkCommandPoolCreateInfo create_info = {
        /* sType            */ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        /* pNext            */ nullptr,
        /* flags            */ VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        /* queueFamilyIndex */ vk->queue[queue].family_index};
    VK_RETURN_FAIL(vk->createCommandPool(vk->device, &create_info, nullptr,
                                         &command_pool));
    thread_data->temporary_command_pool[queue] = command_pool;
  }

  cmd_buffer = VK_NULL_HANDLE;

  // find a suitable command buffer
  for (auto &&cbuf : thread_data->temporary_command_buffers) {
    if (cbuf.pool != command_pool) continue;

    if (cbuf.state == TemporaryCommandBuffer::InFlight) {
      bool completed;
      VK_RETURN_FAIL(WaitForFence(cbuf.fence, completed, false));
      if (completed) {
        cbuf.state = TemporaryCommandBuffer::Initial;
      }
    }

    if (fresh && cbuf.state != TemporaryCommandBuffer::Initial) {
      continue;
    }

    if (cbuf.state == TemporaryCommandBuffer::Initial) {
      VkCommandBufferBeginInfo begin_info = {
          /* sType            */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
          /* pNext            */ nullptr,
          /* flags            */ 0,
          /* pInheritanceInfo */ nullptr};
      VK_RETURN_FAIL(vk->beginCommandBuffer(cbuf.buffer, &begin_info));
      cbuf.state = TemporaryCommandBuffer::Recording;
    }

    if (cbuf.state == TemporaryCommandBuffer::Recording) {
      cbuf.state = TemporaryCommandBuffer::Acquired;
      cmd_buffer = cbuf.buffer;
      break;
    }
  }

  // create one if we need to
  if (cmd_buffer == VK_NULL_HANDLE) {
    VkCommandBufferAllocateInfo allocate_info = {
        /* sType              */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        /* pNext              */ nullptr,
        /* commandPool        */ command_pool,
        /* level              */ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        /* commandBufferCount */ 1};
    VK_RETURN_FAIL(
        vk->allocateCommandBuffers(vk->device, &allocate_info, &cmd_buffer));

    VkCommandBufferBeginInfo begin_info = {
        /* sType            */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        /* pNext            */ nullptr,
        /* flags            */ 0,
        /* pInheritanceInfo */ nullptr};
    VK_RETURN_FAIL(vk->beginCommandBuffer(cmd_buffer, &begin_info));

    thread_data->temporary_command_buffers.push_back(
        TemporaryCommandBuffer{/* state  */ TemporaryCommandBuffer::Acquired,
                               /* pool   */ command_pool,
                               /* buffer */ cmd_buffer,
                               /* fence  */ Fence()});
  }

  return Result::kSuccess;
}

Result Vulkan::ReleaseTemporaryCommandBuffer(VkCommandBuffer cmd_buffer) {
  auto thread_data = GetThreadData();

  // find this temporary command buffer
  for (auto &&cbuf : thread_data->temporary_command_buffers) {
    if (cbuf.buffer == cmd_buffer) {
      cbuf.state = TemporaryCommandBuffer::Recording;
      break;
    }
  }

  return Result::kSuccess;
}

Result Vulkan::QueueTemporaryCommandBuffer(VkCommandBuffer cmd_buffer,
                                           Fence &fence) {
  auto thread_data = GetThreadData();

  VK_RETURN_FAIL(vk->endCommandBuffer(cmd_buffer));

  VK_RETURN_FAIL(AllocateFence(fence));

  // find this temporary command buffer
  for (auto &&cbuf : thread_data->temporary_command_buffers) {
    if (cbuf.buffer != cmd_buffer) continue;

    // find the queue from the pool
    uint32_t queue = 0;
    for (; queue < Q_COUNT; ++queue) {
      if (thread_data->temporary_command_pool[queue] == cbuf.pool) break;
    }

    cbuf.state = TemporaryCommandBuffer::InFlight;
    cbuf.fence = fence;

    VkSubmitInfo submit_info = {
        /* sType                */ VK_STRUCTURE_TYPE_SUBMIT_INFO,
        /* pNext                */ nullptr,
        /* waitSemaphoreCount   */ 0,
        /* pWaitSemaphores      */ nullptr,
        /* pWaitDstStageMask    */ nullptr,
        /* commandBufferCount   */ 1,
        /* pCommandBuffers      */ &cmd_buffer,
        /* signalSemaphoreCount */ 0,
        /* pSignalSemaphores    */ nullptr};

    return Result(vk->queueSubmit(vk->queue[queue].queue, 1, &submit_info,
                                  fence.data->fence));
  }

  // should not happen
  assert(false);

  return Result::kSuccess;
}

Result Vulkan::SubmitTemporaryCommandBuffers(EQueue queue,
                                             VkSemaphore &semaphore) {
  semaphore = VK_NULL_HANDLE;

  std::vector<VkCommandBuffer> command_buffers;
  Fence fence;

  {
    std::lock_guard<std::mutex> guard(vk->thread_data_mutex);

    bool have_allocated_fence = false;

    for (auto &&thread_data : vk->thread_data) {
      VkCommandPool pool = thread_data.second->temporary_command_pool[queue];

      for (auto &&cbuf : thread_data.second->temporary_command_buffers) {
        if (cbuf.pool != pool) continue;

        if (cbuf.state != TemporaryCommandBuffer::Recording) continue;

        if (!have_allocated_fence) {
          VK_RETURN_FAIL(AllocateFence(fence));
          have_allocated_fence = true;
        }

        VK_RETURN_FAIL(vk->endCommandBuffer(cbuf.buffer));

        cbuf.state = TemporaryCommandBuffer::InFlight;
        cbuf.fence = fence;

        command_buffers.push_back(cbuf.buffer);
      }
    }
  }

  if (command_buffers.size() == 0) return Result::kSuccess;

  VkSemaphoreCreateInfo create_info = {
      /* sType */ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      /* pNext */ nullptr,
      /* flags */ 0};
  VK_RETURN_FAIL(
      vk->createSemaphore(vk->device, &create_info, nullptr, &semaphore));

  VkSubmitInfo submit_info = {
      /* sType                */ VK_STRUCTURE_TYPE_SUBMIT_INFO,
      /* pNext                */ nullptr,
      /* waitSemaphoreCount   */ 0,
      /* pWaitSemaphores      */ nullptr,
      /* pWaitDstStageMask    */ nullptr,
      /* commandBufferCount   */ static_cast<uint32_t>(command_buffers.size()),
      /* pCommandBuffers      */ command_buffers.data(),
      /* signalSemaphoreCount */ 1,
      /* pSignalSemaphores    */ &semaphore};

  return Result(vk->queueSubmit(vk->queue[queue].queue, 1, &submit_info,
                                fence.data->fence));
}

// ============================================================================

// ============================================================================
Result Vulkan::InitEntry(const VulkanRequirements &requirements) {
  void *libVulkan = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
  if (!libVulkan) return Result::kSuccess;

#define LOAD_ENTRY(NAME, ID)                                        \
  do {                                                              \
    vk->ID = reinterpret_cast<PFN_##NAME>(dlsym(libVulkan, #NAME)); \
    if (vk->ID == nullptr) {                                        \
      return Result{VK_ERROR_INITIALIZATION_FAILED};                \
    }                                                               \
  } while (false)

  vk.reset(new Data());
  LOAD_ENTRY(vkCreateInstance, createInstance);
  LOAD_ENTRY(vkDestroyInstance, destroyInstance);
  LOAD_ENTRY(vkEnumeratePhysicalDevices, enumeratePhysicalDevices);
  LOAD_ENTRY(vkGetPhysicalDeviceFeatures, getPhysicalDeviceFeatures);
  LOAD_ENTRY(vkGetPhysicalDeviceFormatProperties,
             getPhysicalDeviceFormatProperties);
  LOAD_ENTRY(vkGetPhysicalDeviceImageFormatProperties,
             getPhysicalDeviceImageFormatProperties);
  LOAD_ENTRY(vkGetPhysicalDeviceProperties, getPhysicalDeviceProperties);
  LOAD_ENTRY(vkGetPhysicalDeviceQueueFamilyProperties,
             getPhysicalDeviceQueueFamilyProperties);
  LOAD_ENTRY(vkGetPhysicalDeviceMemoryProperties,
             getPhysicalDeviceMemoryProperties);
  LOAD_ENTRY(vkGetInstanceProcAddr, getInstanceProcAddr);
  LOAD_ENTRY(vkGetDeviceProcAddr, getDeviceProcAddr);
  LOAD_ENTRY(vkCreateDevice, createDevice);
  LOAD_ENTRY(vkDestroyDevice, destroyDevice);
  LOAD_ENTRY(vkEnumerateInstanceExtensionProperties,
             enumerateInstanceExtensionProperties);
  LOAD_ENTRY(vkEnumerateDeviceExtensionProperties,
             enumerateDeviceExtensionProperties);
  LOAD_ENTRY(vkEnumerateInstanceLayerProperties,
             enumerateInstanceLayerProperties);
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
  LOAD_ENTRY(vkGetImageSparseMemoryRequirements,
             getImageSparseMemoryRequirements);
  LOAD_ENTRY(vkGetPhysicalDeviceSparseImageFormatProperties,
             getPhysicalDeviceSparseImageFormatProperties);
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

  vk->use_swappy = requirements._use_swappy;

  return Result::kSuccess;
}

Result Vulkan::InitInstanceExtensions(const VulkanRequirements &requirements) {
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
  } while (res == VK_INCOMPLETE);
  for (const auto &layer : layers) {
    vk->available_instance_layers.insert(layer.layerName);
  }

#define LOAD_EXTENSIONS(layer)                                             \
  do {                                                                     \
    count = 0;                                                             \
    VK_RETURN_FAIL(                                                        \
        vk->enumerateInstanceExtensionProperties(layer, &count, nullptr)); \
    extensions.resize(count);                                              \
    VK_RETURN_FAIL(vk->enumerateInstanceExtensionProperties(               \
        layer, &count, extensions.data()));                                \
    for (const auto &extension : extensions) {                             \
      vk->available_instance_extensions.insert(extension.extensionName);   \
    }                                                                      \
  } while (false)

  LOAD_EXTENSIONS(nullptr);

  // get the layer extensions
  for (const auto &layer : vk->available_instance_layers) {
    LOAD_EXTENSIONS(layer.c_str());
  }

#undef LOAD_EXTENSIONS

  std::unordered_set<std::string> req_layers{requirements._instance_layers};
  std::unordered_set<std::string> req_extensions{
      requirements._instance_extensions};

  if (requirements._debug) {
    auto request_layer = [&](std::string name) -> bool {
      Log::I(TAG, "instance layer requested '%s' ...", name.c_str());
      for (auto &&layer : vk->available_instance_layers) {
        if (layer == name) {
          Log::I(TAG, "  found");
          req_layers.insert(name);
          return true;
        }
      }
      Log::I(TAG, "   not found");
      return false;
    };

    auto request_extension = [&](std::string name) -> bool {
      Log::I(TAG, "instance extension requested '%s' ...", name.c_str());
      for (auto &&extension : vk->available_instance_extensions) {
        if (extension == name) {
          Log::I(TAG, "  found");
          req_extensions.insert(name);
          return true;
        }
      }
      Log::I(TAG, "   not found");
      return false;
    };

    // this is a 'meta' layer that includes the later ones
    if (!request_layer("VK_LAYER_LUNARG_standard_validation")) {
      // on some devices I have encountered enabling all of them (with or
      // without the meta one) can cause issues. Included is the others in case
      // one causes issues. These also are order dependent and this is an
      // acceptable order.

      // request_layer("VK_LAYER_GOOGLE_threading");
      // request_layer("VK_LAYER_LUNARG_parameter_validation");
      // request_layer("VK_LAYER_LUNARG_device_limits");
      // request_layer("VK_LAYER_LUNARG_object_tracker");
      // request_layer("VK_LAYER_LUNARG_image");
      request_layer("VK_LAYER_LUNARG_core_validation");
      // request_layer("VK_LAYER_LUNARG_swapchain");
      // request_layer("VK_LAYER_GOOGLE_unique_objects");
    }

    if (!request_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
      request_extension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
      request_extension(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
    }
  }

  // make sure we have the required instance layers
  for (const auto &layer : req_layers) {
    if (vk->available_instance_layers.count(layer)) {
      Log::I(TAG, "required layer %s enabled", layer.c_str());
      vk->enabled_instance_layers.insert(layer);
    } else {
      Log::I(TAG, "missing layer %s", layer.c_str());
      return Result(VK_ERROR_LAYER_NOT_PRESENT);
    }
  }

  // make sure we have the required instance extensions
  for (const auto &extension : req_extensions) {
    if (vk->available_instance_extensions.count(extension)) {
      Log::I(TAG, "required extension %s enabled", extension.c_str());
      vk->enabled_instance_extensions.insert(extension);
    } else {
      Log::I(TAG, "missing extension %s", extension.c_str());
      return Result(VK_ERROR_EXTENSION_NOT_PRESENT);
    }
  }

  return Result::kSuccess;
}

Result Vulkan::InitInstance(const VulkanRequirements &requirements) {
  VK_RETURN_FAIL(InitInstanceExtensions(requirements));

  // map to c_str for VkInstanceCreateInfo
  std::vector<const char *> enabled_layers;
  for (const auto &layer : vk->enabled_instance_layers) {
    Log::D(TAG, "layer %s", layer.c_str());
    enabled_layers.emplace_back(layer.c_str());
  }

  // map to c_str for VkInstanceCreateInfo
  std::vector<const char *> enabled_extensions;
  for (const auto &extension : vk->enabled_instance_extensions) {
    Log::D(TAG, "extension %s", extension.c_str());
    enabled_extensions.emplace_back(extension.c_str());
  }

  VkApplicationInfo application_info = {
      /* sType              */ VK_STRUCTURE_TYPE_APPLICATION_INFO,
      /* pNext              */ nullptr,
      /* pApplicationName   */ "ancer",
      /* applicationVersion */ 1,
      /* pEngineName        */ "ancer",
      /* engineVersion      */ 1,
      /* apiVersion         */ VK_API_VERSION_1_0};
  VkInstanceCreateInfo create_info = {
      /* sType                   */ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      /* pNext                   */ nullptr,
      /* flags                   */ 0,
      /* pApplicationInfo        */ &application_info,
      /* enabledLayerCount       */
      static_cast<uint32_t>(enabled_layers.size()),
      /* ppEnabledLayerNames     */ enabled_layers.empty()
          ? nullptr
          : enabled_layers.data(),
      /* enabledExtensionCount   */
      static_cast<uint32_t>(enabled_extensions.size()),
      /* ppEnabledExtensionNames */ enabled_extensions.empty()
          ? nullptr
          : enabled_extensions.data()};
  VK_RETURN_FAIL(vk->createInstance(&create_info, nullptr, &vk->instance));

#define LOAD_ENTRY(NAME, ID)             \
  vk->ID = reinterpret_cast<PFN_##NAME>( \
      vk->getInstanceProcAddr(vk->instance, #NAME));

  if (HaveInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME)) {
    LOAD_ENTRY(vkDestroySurfaceKHR, destroySurfaceKHR);
    LOAD_ENTRY(vkGetPhysicalDeviceSurfaceSupportKHR,
               getPhysicalDeviceSurfaceSupportKHR);
    LOAD_ENTRY(vkGetPhysicalDeviceSurfaceCapabilitiesKHR,
               getPhysicalDeviceSurfaceCapabilitiesKHR);
    LOAD_ENTRY(vkGetPhysicalDeviceSurfaceFormatsKHR,
               getPhysicalDeviceSurfaceFormatsKHR);
    LOAD_ENTRY(vkGetPhysicalDeviceSurfacePresentModesKHR,
               getPhysicalDeviceSurfacePresentModesKHR);
  }

#ifdef VK_USE_PLATFORM_ANDROID_KHR
  if (HaveInstanceExtension(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME)) {
    LOAD_ENTRY(vkCreateAndroidSurfaceKHR, createAndroidSurfaceKHR);
  }
#endif

  if (HaveInstanceExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME)) {
    LOAD_ENTRY(vkCreateDebugReportCallbackEXT, createDebugReportCallbackEXT);
    LOAD_ENTRY(vkDestroyDebugReportCallbackEXT, destroyDebugReportCallbackEXT);
    LOAD_ENTRY(vkDebugReportMessageEXT, debugReportMessageEXT);
  }

  if (HaveInstanceExtension(VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
    LOAD_ENTRY(vkDebugMarkerSetObjectTagEXT, debugMarkerSetObjectTagEXT);
    LOAD_ENTRY(vkDebugMarkerSetObjectNameEXT, debugMarkerSetObjectNameEXT);
    LOAD_ENTRY(vkCmdDebugMarkerBeginEXT, cmdDebugMarkerBeginEXT);
    LOAD_ENTRY(vkCmdDebugMarkerEndEXT, cmdDebugMarkerEndEXT);
    LOAD_ENTRY(vkCmdDebugMarkerInsertEXT, cmdDebugMarkerInsertEXT);
  }

  if (HaveInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
    LOAD_ENTRY(vkSetDebugUtilsObjectNameEXT, setDebugUtilsObjectNameEXT);
    LOAD_ENTRY(vkSetDebugUtilsObjectTagEXT, setDebugUtilsObjectTagEXT);
    LOAD_ENTRY(vkQueueBeginDebugUtilsLabelEXT, queueBeginDebugUtilsLabelEXT);
    LOAD_ENTRY(vkQueueEndDebugUtilsLabelEXT, queueEndDebugUtilsLabelEXT);
    LOAD_ENTRY(vkQueueInsertDebugUtilsLabelEXT, queueInsertDebugUtilsLabelEXT);
    LOAD_ENTRY(vkCmdBeginDebugUtilsLabelEXT, cmdBeginDebugUtilsLabelEXT);
    LOAD_ENTRY(vkCmdEndDebugUtilsLabelEXT, cmdEndDebugUtilsLabelEXT);
    LOAD_ENTRY(vkCmdInsertDebugUtilsLabelEXT, cmdInsertDebugUtilsLabelEXT);
    LOAD_ENTRY(vkCreateDebugUtilsMessengerEXT, createDebugUtilsMessengerEXT);
    LOAD_ENTRY(vkDestroyDebugUtilsMessengerEXT, destroyDebugUtilsMessengerEXT);
    LOAD_ENTRY(vkSubmitDebugUtilsMessageEXT, submitDebugUtilsMessageEXT);
  }

#undef LOAD_ENTRY

  return Result::kSuccess;
}

struct Object {
  VkObjectType type;
  uint64_t handle;
  const char *name;
};

struct Label {
  const char *name;
  float color[4];
};

struct Message {
  VkDebugUtilsMessageSeverityFlagBitsEXT message_severity;
  VkDebugUtilsMessageTypeFlagsEXT message_types;
  const char *message_id_name;
  int32_t message_id_number;
  const char *message;
  std::vector<Label> queue_labels;
  std::vector<Label> command_buffer_labels;
  std::vector<Object> objects;
};

static VkBool32 DebugMessage(const Message &message) {
  Log::E(TAG, "%s", message.message);

  return VK_FALSE;
}

static VKAPI_ATTR VkBool32 DebugReportCallbackEXT(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
    uint64_t object, size_t location, int32_t messageCode,
    const char *pLayerPrefix, const char *pMessage, void *pUserData) {
  Message msg;

  // TODO(sarahburns@google.com) see if this has an official way to translate
  // from new to old (and back)
  if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
    msg.message_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    msg.message_types |= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
  }
  if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
    msg.message_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    msg.message_types |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
  }
  if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
    msg.message_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    msg.message_types |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  }
  if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
    msg.message_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    msg.message_types |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
  }
  if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
    msg.message_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    msg.message_types |= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
  }

  Object obj;

#define MATCH(FROM, TO)                          \
  case VK_DEBUG_REPORT_OBJECT_TYPE_##FROM##_EXT: \
    obj.type = VK_OBJECT_TYPE_##TO;              \
    break;
#define MATCH1(FROMTO) MATCH(FROMTO, FROMTO)
  switch (objectType) {
    default:
      MATCH1(UNKNOWN)
      MATCH1(INSTANCE)
      MATCH1(PHYSICAL_DEVICE)
      MATCH1(DEVICE)
      MATCH1(QUEUE)
      MATCH1(SEMAPHORE)
      MATCH1(COMMAND_BUFFER)
      MATCH1(FENCE)
      MATCH1(DEVICE_MEMORY)
      MATCH1(BUFFER)
      MATCH1(IMAGE)
      MATCH1(EVENT)
      MATCH1(QUERY_POOL)
      MATCH1(BUFFER_VIEW)
      MATCH1(IMAGE_VIEW)
      MATCH1(SHADER_MODULE)
      MATCH1(PIPELINE_CACHE)
      MATCH1(PIPELINE_LAYOUT)
      MATCH1(RENDER_PASS)
      MATCH1(PIPELINE)
      MATCH1(DESCRIPTOR_SET_LAYOUT)
      MATCH1(SAMPLER)
      MATCH1(DESCRIPTOR_POOL)
      MATCH1(DESCRIPTOR_SET)
      MATCH1(FRAMEBUFFER)
      MATCH1(SURFACE_KHR)
      MATCH1(SWAPCHAIN_KHR)
      MATCH1(DEBUG_REPORT_CALLBACK_EXT)
      MATCH1(DISPLAY_KHR)
      MATCH1(DISPLAY_MODE_KHR)
      MATCH1(OBJECT_TABLE_NVX)
      MATCH1(INDIRECT_COMMANDS_LAYOUT_NVX)
      MATCH1(VALIDATION_CACHE_EXT)
      MATCH1(SAMPLER_YCBCR_CONVERSION)
      MATCH1(DESCRIPTOR_UPDATE_TEMPLATE)
      MATCH1(ACCELERATION_STRUCTURE_NV)
  }
#undef MATCH1
#undef MATCH

  obj.handle = object;

  // TODO(sarahburns@google.com) get object name from VK_EXT_debug_marker
  // extension

  msg.objects.push_back(obj);

  msg.message_id_number = messageCode;

  msg.message = pMessage;

  return DebugMessage(msg);
}

static VKAPI_ATTR VkBool32 DebugUtilsMessengerCallbackEXT(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {
  Message msg;
  msg.message_severity = messageSeverity;
  msg.message_types = messageTypes;
  msg.message_id_name = pCallbackData->pMessageIdName;
  msg.message_id_number = pCallbackData->messageIdNumber;
  msg.message = pCallbackData->pMessage;

  for (uint32_t i = 0; i < pCallbackData->queueLabelCount; ++i) {
    Label lbl;
    lbl.name = pCallbackData->pQueueLabels[i].pLabelName;
    memcpy(lbl.color, pCallbackData->pQueueLabels[i].color, sizeof(lbl.color));
    msg.queue_labels.push_back(lbl);
  }

  for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; ++i) {
    Label lbl;
    lbl.name = pCallbackData->pCmdBufLabels[i].pLabelName;
    memcpy(lbl.color, pCallbackData->pCmdBufLabels[i].color, sizeof(lbl.color));
    msg.command_buffer_labels.push_back(lbl);
  }

  for (uint32_t i = 0; i < pCallbackData->objectCount; ++i) {
    Object obj;
    obj.type = pCallbackData->pObjects[i].objectType;
    obj.handle = pCallbackData->pObjects[i].objectHandle;
    obj.name = pCallbackData->pObjects[i].pObjectName;
    msg.objects.push_back(obj);
  }

  return DebugMessage(msg);
}

Result Vulkan::InitDebugReporting(const VulkanRequirements &requirements) {
  // TODO(sarahburns@google.com)

  if (HaveInstanceExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME)) {
    VkDebugReportCallbackCreateInfoEXT create_info = {
        /* sType       */
        VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
        /* pNext       */ nullptr,
        /* flags       */ VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
            VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT,
        /* pfnCallback */ DebugReportCallbackEXT,
        /* pUserData   */ this};
    VK_RETURN_FAIL(vk->createDebugReportCallbackEXT(
        vk->instance, &create_info, nullptr, &vk->debug_report_callback));

    Log::I(TAG, "Debug report callback created");
  }

  if (HaveInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
    VkDebugUtilsMessengerCreateInfoEXT create_info = {
        /* sType           */
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        /* pNext           */ nullptr,
        /* flags           */ 0,
        /* messageSeverity */ VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        /* messageType     */ VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        /* pfnUserCallback */ DebugUtilsMessengerCallbackEXT,
        /* pUserData       */ this};

    VK_RETURN_FAIL(vk->createDebugUtilsMessengerEXT(
        vk->instance, &create_info, nullptr, &vk->debug_utils_callback));

    Log::I(TAG, "Debug utils callback created");
  }

  return Result::kSuccess;
}

Result Vulkan::InitPhysicalDevice(const VulkanRequirements &requirements) {
  uint32_t count = 0;
  vk->enumeratePhysicalDevices(vk->instance, &count, nullptr);

  std::vector<VkPhysicalDevice> physical_devices(count);
  vk->enumeratePhysicalDevices(vk->instance, &count, physical_devices.data());

  // use the first physical device enumerated
  vk->physical_device = VK_NULL_HANDLE;

  for (const auto &physical_device : physical_devices) {
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

Result Vulkan::InitSurface(const VulkanRequirements &requirements) {
#ifdef VK_USE_PLATFORM_ANDROID_KHR
  AndroidHelper::WindowSize(vk->window_width, vk->window_height);

  VkAndroidSurfaceCreateInfoKHR create_info = {
      /* sType  */ VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
      /* pNext  */ nullptr,
      /* flags  */ 0,
      /* window */ AndroidHelper::Window()};
  VK_RETURN_FAIL(vk->createAndroidSurfaceKHR(vk->instance, &create_info,
                                             nullptr, &vk->surface));
#endif

  // by now, vk->window_width, vk->window_height, and vk->surface are valid

  VK_RETURN_FAIL(vk->getPhysicalDeviceSurfaceCapabilitiesKHR(
      vk->physical_device, vk->surface, &vk->surface_capabilities));

  uint32_t count = 0;
  VK_RETURN_FAIL(vk->getPhysicalDeviceSurfacePresentModesKHR(
      vk->physical_device, vk->surface, &count, nullptr));

  vk->surface_present_modes.resize(count);
  VK_RETURN_FAIL(vk->getPhysicalDeviceSurfacePresentModesKHR(
      vk->physical_device, vk->surface, &count,
      vk->surface_present_modes.data()));

  count = 0;
  VK_RETURN_FAIL(vk->getPhysicalDeviceSurfaceFormatsKHR(
      vk->physical_device, vk->surface, &count, nullptr));

  vk->surface_formats.resize(count);
  VK_RETURN_FAIL(vk->getPhysicalDeviceSurfaceFormatsKHR(
      vk->physical_device, vk->surface, &count, vk->surface_formats.data()));

  return Result::kSuccess;
}

Result Vulkan::InitDeviceExtensions(const VulkanRequirements &requirements) {
  uint32_t count;
  VkResult res;

  std::vector<VkLayerProperties> layers;
  std::vector<VkExtensionProperties> extensions;
  std::vector<VkExtensionProperties> all_extensions;

  vk->available_device_layers.clear();
  vk->available_device_extensions.clear();

  // get device layers
  count = 0;
  VK_RETURN_FAIL(
      vk->enumerateDeviceLayerProperties(vk->physical_device, &count, nullptr));
  layers.resize(count);
  VK_RETURN_FAIL(vk->enumerateDeviceLayerProperties(vk->physical_device, &count,
                                                    layers.data()));
  for (auto const &layer : layers)
    vk->available_device_layers.insert(layer.layerName);

    // get extensions for layers (and "")
#define LOAD_EXTENSIONS(layer)                                         \
  do {                                                                 \
    count = 0;                                                         \
    VK_RETURN_FAIL(vk->enumerateDeviceExtensionProperties(             \
        vk->physical_device, layer, &count, nullptr));                 \
    extensions.resize(count);                                          \
    VK_RETURN_FAIL(vk->enumerateDeviceExtensionProperties(             \
        vk->physical_device, layer, &count, extensions.data()));       \
    for (const auto &extension : extensions) {                         \
      all_extensions.push_back(extension);                             \
      vk->available_device_extensions.insert(extension.extensionName); \
    }                                                                  \
  } while (false)

  LOAD_EXTENSIONS(nullptr);

  for (const auto &layer : vk->available_device_layers) {
    LOAD_EXTENSIONS(layer.c_str());
  }

  if (vk->use_swappy) {
    count = 0;
    SwappyVk_determineDeviceExtensions(
        vk->physical_device, static_cast<uint32_t>(all_extensions.size()),
        all_extensions.data(), &count, nullptr);
    if (count > 0) {
      return Result(VK_ERROR_EXTENSION_NOT_PRESENT);
    }
  }

#undef LOAD_EXTENSIONS

  // make sure we have the required device extensions
  for (const auto &layer : requirements._device_layers) {
    if (vk->available_device_extensions.count(layer))
      vk->enabled_device_extensions.insert(layer);
    else
      return Result(VK_ERROR_LAYER_NOT_PRESENT);
  }

  // make sure we have the required device extensions
  for (const auto &extension : requirements._device_extensions) {
    if (vk->available_device_extensions.count(extension))
      vk->enabled_device_extensions.insert(extension);
    else
      return Result(VK_ERROR_EXTENSION_NOT_PRESENT);
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
  Vulkan &vulkan;

  QueueResolver(Vulkan &_vulkan,
                std::vector<VkQueueFamilyProperties> &&_families)
      : vulkan(_vulkan),
        families(std::move(_families)),
        used(families.size()),
        create_info() {}

  QueueAddress Allocate(VkQueueFlags required_flags) {
    // find the queue family with minimal diviation from required_flags
    uint32_t qfi = 0;

    bool best_new = false;
    uint32_t best_qfi = UINT32_MAX;
    uint32_t best_bits = UINT32_MAX;
    for (; qfi < families.size(); ++qfi) {
      uint32_t flags = families[qfi].queueFlags;

      if (vulkan.HaveInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME)) {
        VkBool32 present_support = VK_FALSE;
        VK_GOTO_FAIL(vulkan.vk->getPhysicalDeviceSurfaceSupportKHR(
            vulkan.vk->physical_device, qfi, vulkan.vk->surface,
            &present_support));
        if (present_support) {
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
      if ((flags & VK_QUEUE_GRAPHICS_BIT) || (flags & VK_QUEUE_COMPUTE_BIT)) {
        flags |= VK_QUEUE_TRANSFER_BIT;
      }

      if ((flags & required_flags) == 0) continue;

      bool is_new = used[qfi] < families[qfi].queueCount;

      const uint32_t num_bits = std::bitset<8>(flags).count();

      if (best_qfi == UINT32_MAX || num_bits < best_bits) {
        best_qfi = qfi;
        best_bits = num_bits;
        best_new = is_new;
      }
    }

    if (best_qfi == UINT32_MAX)
    fail:
      return QueueAddress{UINT32_MAX, UINT32_MAX};

    uint32_t index = used[best_qfi];
    if (best_new) {
      priorities.push_back(0.0f);
      ++used[best_qfi];
    } else {
      --index;
    }

    return QueueAddress{best_qfi, index};
  }

  void Finalize() {
    uint32_t priority_index = 0;

    for (uint32_t qfi = 0; qfi < families.size(); ++qfi) {
      if (used[qfi] == 0) continue;

      const float flat_priority = 1.0f / float(used[qfi]);

      for (uint32_t index = 0; index < used[qfi]; ++index) {
        priorities[priority_index + index] = flat_priority;
      }

      create_info.emplace_back(VkDeviceQueueCreateInfo{
          /* sType            */ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          /* pNext            */ nullptr,
          /* flags            */ 0,
          /* queueFamilyIndex */ qfi,
          /* queueCount       */ used[qfi],
          /* pQueuePriorities */ priorities.data() + priority_index});

      priority_index += used[qfi];
    }
  }
};
}  // namespace

Result Vulkan::InitDevice(const VulkanRequirements &requirements) {
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
  std::vector<const char *> enabled_layers(vk->enabled_device_layers.size());
  for (const auto &layer : vk->enabled_device_layers)
    enabled_layers.emplace_back(layer.c_str());

  // map to c_str for VkDeviceCreateInfo
  std::vector<const char *> enabled_extensions(
      vk->enabled_device_extensions.size());
  for (const auto &extension : vk->enabled_device_extensions)
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
      /* ppEnabledLayerNames     */ enabled_layers.empty()
          ? nullptr
          : enabled_layers.data(),
      /* enabledExtensionCount   */
      static_cast<uint32_t>(enabled_extensions.size()),
      /* ppEnabledExtensionNames */ enabled_extensions.empty()
          ? nullptr
          : enabled_extensions.data(),
      /* pEnabledFeatures        */ nullptr};

  VK_RETURN_FAIL(vk->createDevice(vk->physical_device, &create_info, nullptr,
                                  &vk->device));

#define RESOLVE(NAME)                                                     \
  do {                                                                    \
    if (NAME.queue_family_index != UINT32_MAX) {                          \
      vk->NAME.family_index = NAME.queue_family_index;                    \
      vk->getDeviceQueue(vk->device, NAME.queue_family_index, NAME.index, \
                         &vk->NAME.queue);                                \
    }                                                                     \
  } while (false)

  RESOLVE(graphics);
  RESOLVE(compute);
  RESOLVE(transfer);
  RESOLVE(present);

  if (vk->use_swappy) {
    SwappyVk_setQueueFamilyIndex(vk->device, vk->present.queue,
                                 vk->present.family_index);
  }

#define LOAD_ENTRY(NAME, ID) \
  vk->ID =                   \
      reinterpret_cast<PFN_##NAME>(vk->getDeviceProcAddr(vk->device, #NAME));

  if (HaveDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
    LOAD_ENTRY(vkCreateSwapchainKHR, createSwapchainKHR);
    LOAD_ENTRY(vkDestroySwapchainKHR, destroySwapchainKHR);
    LOAD_ENTRY(vkGetSwapchainImagesKHR, getSwapchainImagesKHR);
    LOAD_ENTRY(vkAcquireNextImageKHR, acquireNextImageKHR);
    LOAD_ENTRY(vkQueuePresentKHR, queuePresentKHR);
    LOAD_ENTRY(vkGetDeviceGroupPresentCapabilitiesKHR,
               getDeviceGroupPresentCapabilitiesKHR);
    LOAD_ENTRY(vkGetDeviceGroupSurfacePresentModesKHR,
               getDeviceGroupSurfacePresentModesKHR);
    LOAD_ENTRY(vkGetPhysicalDevicePresentRectanglesKHR,
               getPhysicalDevicePresentRectanglesKHR);
    LOAD_ENTRY(vkAcquireNextImage2KHR, acquireNextImage2KHR);
  }

#undef LOAD_ENTRY

  return Result::kSuccess;
}

Vulkan::ThreadData *Vulkan::GetThreadData() {
  std::lock_guard<std::mutex> guard(vk->thread_data_mutex);
  std::thread::id this_thread_id = std::this_thread::get_id();
  auto search = vk->thread_data.find(this_thread_id);
  if (search != vk->thread_data.end()) {
    return search->second.get();
  }
  auto data = std::make_unique<ThreadData>();
  auto data_p = data.get();
  vk->thread_data[this_thread_id] = std::move(data);
  return data_p;
}

Result Vulkan::DebugName(VkDebugReportObjectTypeEXT object_type,
                         uint64_t object, const char *format, va_list args) {
  if (vk->debug_enabled) {
    // TODO(sarahburns@google.com)
  }
  return Result::kSuccess;
}

void Vulkan::AddDestroy(DestroyEntry &destroy) {
  std::lock_guard<std::mutex> guard(vk->destroy_mutex);
  vk->destroy.push_back(destroy);
}

void Vulkan::DoDestroy(DestroyEntry &destroy) {
  switch (destroy.object_type) {
    case VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT:
    case VK_DEBUG_REPORT_OBJECT_TYPE_MAX_ENUM_EXT:
    case VK_DEBUG_REPORT_OBJECT_TYPE_RANGE_SIZE_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT:
      vk->destroySemaphore(vk->device, VkSemaphore(destroy.object), nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT:
      vk->freeMemory(vk->device, VkDeviceMemory(destroy.object), nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT:
      vk->destroyBuffer(vk->device, VkBuffer(destroy.object), nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT:
      vk->destroyImage(vk->device, VkImage(destroy.object), nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT:
      vk->destroyEvent(vk->device, VkEvent(destroy.object), nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT:
      vk->destroyQueryPool(vk->device, VkQueryPool(destroy.object), nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT:
      vk->destroyBufferView(vk->device, VkBufferView(destroy.object), nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT:
      vk->destroyImageView(vk->device, VkImageView(destroy.object), nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT:
      vk->destroyShaderModule(vk->device, VkShaderModule(destroy.object),
                              nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT:
      vk->destroyPipelineCache(vk->device, VkPipelineCache(destroy.object),
                               nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT:
      vk->destroyPipelineLayout(vk->device, VkPipelineLayout(destroy.object),
                                nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT:
      vk->destroyRenderPass(vk->device, VkRenderPass(destroy.object), nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT:
      vk->destroyPipeline(vk->device, VkPipeline(destroy.object), nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT:
      vk->destroyDescriptorSetLayout(
          vk->device, VkDescriptorSetLayout(destroy.object), nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT:
      vk->destroySampler(vk->device, VkSampler(destroy.object), nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT:
      vk->destroyDescriptorPool(vk->device, VkDescriptorPool(destroy.object),
                                nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT:
      vk->destroyFramebuffer(vk->device, VkFramebuffer(destroy.object),
                             nullptr);
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_KHR_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_MODE_KHR_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_OBJECT_TABLE_NVX_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT:
      // handled elsewhere
      break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_VALIDATION_CACHE_EXT:
      // handled elsewhere
      break;
  }
}

void Vulkan::AdvanceFrame(uint32_t frame) {
  if (frame < vk->frame) {
    Log::W(TAG, "Tried to advance with an old frame number");
    return;
  }

  GetResourcesStore().Cleanup(true).ok();

  // free all unused framebuffers
  {
    std::lock_guard<std::mutex> guard(vk->framebuffer_mutex);
    auto i = vk->framebuffers.begin();
    while (i != vk->framebuffers.end()) {
      if (i->second.frame <= frame) {
        vk->destroyFramebuffer(vk->device, i->second.framebuffer, nullptr);
        i = vk->framebuffers.erase(i);
      } else
        ++i;
    }
  }

  {
    std::lock_guard<std::mutex> guard(vk->destroy_mutex);
    auto end = vk->destroy.end();
    for (auto i = vk->destroy.begin(); i != end; /*_*/) {
      if (i->frame <= frame) {
        DoDestroy(*i);
        i = vk->destroy.erase(i);
        end = vk->destroy.end();
      } else
        ++i;
    }
  }

  ++vk->frame;
}

}  // namespace vulkan
}  // namespace ancer
