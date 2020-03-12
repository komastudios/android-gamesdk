#include "context.h"

namespace ancer {
namespace vulkan {

Result Context::Initialize(Vulkan & vk, uint32_t num_buffers,
                           uint32_t qfi, const char * name) {
  Result result;

  Shutdown();

  _qfi = qfi;
  _vk = vk;

  for(uint32_t i = 0; i < num_buffers; ++i) {
    VkCommandPoolCreateInfo create_info = {
      /* sType            */ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      /* pNext            */ nullptr,
      /* flags            */ 0,
      /* queueFamilyIndex */ qfi
    };

    VkCommandPool command_pool;
    VK_GOTO_FAIL(result = vk->createCommandPool(vk->device, &create_info,
                                                nullptr, &command_pool));

    VK_GOTO_FAIL(result = _vk.DebugName(command_pool, "Command Pool %s %i",
                                           name, i));

    VkCommandBufferAllocateInfo allocate_info = {
      /* sType              */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      /* pNext              */ nullptr,
      /* commandPool        */ command_pool,
      /* level              */ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      /* commandBufferCount */ 1
    };

    VkCommandBuffer command_buffer;
    VK_GOTO_FAIL(result = vk->allocateCommandBuffers(vk->device,
                                                     &allocate_info,
                                                     &command_buffer));

    VK_GOTO_FAIL(result = _vk.DebugName(command_buffer,
                                           "Command Buffer %s %i",
                                           name, i));

    _pools.push_back(command_pool);
    _command_buffers.push_back(command_buffer);
    _command_buffer_fence.push_back(Fence());
  }

  if(false) {
fail:
    Shutdown();
    return result;
  }

  return Result::kSuccess;
}

void Context::Shutdown() {
  for(uint32_t i = 0; i < _command_buffers.size(); ++i)
    _vk->freeCommandBuffers(_vk->device, _pools[i], 1, &_command_buffers[i]);

  for(uint32_t i = 0; i < _pools.size(); ++i)
    _vk->destroyCommandPool(_vk->device, _pools[i], nullptr);

  _current_buffer = 0;
}

Result Context::Begin() {
  _current_buffer = (_current_buffer + 1) % _command_buffers.size();

  VK_RETURN_FAIL(ClearFence());

  VkCommandBufferBeginInfo begin_info = {
    /* sType            */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    /* pNext            */ nullptr,
    /* flags            */ 0,
    /* pInheritanceInfo */ nullptr
  };

  VK_RETURN_FAIL(_vk->beginCommandBuffer(CommandBuffer(), &begin_info));

  _end = false;
  _wait_semaphores.clear();
  _wait_stage_masks.clear();
  _signal_semaphores.clear();
  _completed_signal = VK_NULL_HANDLE;

  return Result::kSuccess;
}

Result Context::BindResources(VkPipelineBindPoint bind_point,
                              VkPipelineLayout layout, uint32_t first,
                              std::initializer_list<Resources> resources) {
  auto &resolver = _vk.GetResourcesStore();
  VkDescriptorSet sets[resources.size()];
  uint32_t i = 0;
  for(auto resource : resources) {
    VK_RETURN_FAIL(resolver.Resolve(resource, sets[i]));
    ++i;
  }
  _vk->cmdBindDescriptorSets(CommandBuffer(), bind_point, layout, first,
                             static_cast<uint32_t>(resources.size()), sets,
                             0, nullptr);
  return Result::kSuccess;
}

Result Context::End() {
  if(!_end) {
    _end = true;
    VK_RETURN_FAIL(_vk->endCommandBuffer(CommandBuffer()));
  }

  return Result::kSuccess;
}

Result Context::SetFence(Fence & fence) {
  VK_RETURN_FAIL(ClearFence());

  _command_buffer_fence[_current_buffer] = fence;

  return Result::kSuccess;
}

Result Context::GetFence(Fence & fence, bool create_advancing_fence) {
  if(create_advancing_fence) {
    VK_RETURN_FAIL(_vk.AllocateFence(fence, true));
    _command_buffer_fence[_current_buffer] = fence;
  } else {
    fence = _command_buffer_fence[_current_buffer];
  }
  return Result::kSuccess;
}

void Context::SubmitInfo(VkSubmitInfo & si) const {
  si.waitSemaphoreCount = static_cast<uint32_t>(_wait_semaphores.size());
  si.pWaitSemaphores = _wait_semaphores.empty() ? nullptr
                                                : _wait_semaphores.data();
  si.pWaitDstStageMask = _wait_semaphores.empty() ? nullptr
                                                  : _wait_stage_masks.data();
  si.commandBufferCount = 1;
  si.pCommandBuffers = &_command_buffers[_current_buffer];
  si.signalSemaphoreCount = static_cast<uint32_t>(_signal_semaphores.size());
  si.pSignalSemaphores = _signal_semaphores.empty() ? nullptr
                                                    : _signal_semaphores.data();
}

void Context::Wait(VkSemaphore semaphore, VkPipelineStageFlags stages) {
  _wait_semaphores.push_back(semaphore);
  _wait_stage_masks.push_back(stages);
}

Result Context::CompletedSignal(VkSemaphore & semaphore) {
  if(_completed_signal == VK_NULL_HANDLE) {
    VkSemaphoreCreateInfo semaphore_create_info = {
      /* sType */ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      /* pNext */ nullptr,
      /* flags */ 0
    };
    VK_RETURN_FAIL(_vk->createSemaphore(_vk->device, &semaphore_create_info,
                                        nullptr, &_completed_signal));

    Signal(_completed_signal);
  }

  semaphore = _completed_signal;

  return Result::kSuccess;
}

void Context::Signal(VkSemaphore semaphore) {
  _signal_semaphores.push_back(semaphore);
}

Result Context::ClearFence() {
  if(_command_buffer_fence[_current_buffer].Valid()) {
    bool complete;
    VK_RETURN_FAIL(_vk.WaitForFence(_command_buffer_fence[_current_buffer],
                                    complete));
    if(!complete)
      return Result(VK_TIMEOUT);
  }

  return Result::kSuccess;
}

/*
    VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT = 0x00000002,
    VK_PIPELINE_STAGE_VERTEX_INPUT_BIT = 0x00000004,
    VK_PIPELINE_STAGE_VERTEX_SHADER_BIT = 0x00000008,
    VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT = 0x00000010,
    VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT = 0x00000020,
    VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT = 0x00000040,
    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT = 0x00000080,
    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT = 0x00000100,
    VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT = 0x00000200,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = 0x00000400,
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT = 0x00000800,
    VK_PIPELINE_STAGE_TRANSFER_BIT = 0x00001000,
    VK_PIPELINE_STAGE_HOST_BIT = 0x00004000,

    VK_ACCESS_INDIRECT_COMMAND_READ_BIT = 0x00000001,
    VK_ACCESS_INDEX_READ_BIT = 0x00000002,
    VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT = 0x00000004,
    VK_ACCESS_UNIFORM_READ_BIT = 0x00000008,
    VK_ACCESS_INPUT_ATTACHMENT_READ_BIT = 0x00000010,
    VK_ACCESS_SHADER_READ_BIT = 0x00000020,
    VK_ACCESS_SHADER_WRITE_BIT = 0x00000040,
    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT = 0x00000080,
    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT = 0x00000100,
    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT = 0x00000200,
    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT = 0x00000400,
    VK_ACCESS_TRANSFER_READ_BIT = 0x00000800,
    VK_ACCESS_TRANSFER_WRITE_BIT = 0x00001000,
    VK_ACCESS_HOST_READ_BIT = 0x00002000,
    VK_ACCESS_HOST_WRITE_BIT = 0x00004000,
    VK_ACCESS_MEMORY_READ_BIT = 0x00008000,
    VK_ACCESS_MEMORY_WRITE_BIT = 0x00010000,
*/

void Context::ChangeImageLayout(ImageResource image, VkImageLayout oldLayout) {
  VkImageLayout newLayout = image.Layout();
  if(newLayout == oldLayout) {
    return;
  }
  VkPipelineStageFlags srcStages = 0;
  VkPipelineStageFlags dstStages = 0;
  VkAccessFlags srcAccessMask = 0;
  VkAccessFlags dstAccessMask = 0;
  switch(oldLayout) {
  case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
    srcStages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    break;
  case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    srcStages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    break;
  case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
    srcStages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    break;
  case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    srcStages |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
                 VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    srcAccessMask |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    break;
  case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    srcStages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    srcAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
    break;
  case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
    srcStages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    srcAccessMask |= VK_ACCESS_TRANSFER_WRITE_BIT;
    break;
  default:
    break;
  }
  switch(newLayout) {
  case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
    dstStages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dstAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    break;
  case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    dstStages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    break;
  case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
    dstStages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    break;
  case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    dstStages |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
                 VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dstAccessMask |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    break;
  case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    dstStages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    dstAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
    break;
  case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
    dstStages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    dstAccessMask |= VK_ACCESS_TRANSFER_WRITE_BIT;
    break;
  default:
    break;
  }
  VkImageMemoryBarrier barrier = {
    /* sType               */ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    /* pNext               */ nullptr,
    /* srcAccessMask       */ srcAccessMask,
    /* dstAccessMask       */ dstAccessMask,
    /* oldLayout           */ oldLayout,
    /* newLayout           */ image.Layout(),
    /* srcQueueFamilyIndex */ 0,
    /* dstQueueFamilyIndex */ 0,
    /* image               */ image.ImageHandle(),
    /* subresourceRange    */ image.SubresourceRange()
  };
  _vk->cmdPipelineBarrier(CommandBuffer(), srcStages, dstStages, 0, 0, nullptr,
                          0, nullptr, 1, &barrier);
}

void Context::CopyImage(ImageResource src, ImageResource dst) {
  VkImageCopy region = {
    /* srcSubresource */ src.SubresourceLayers(),
    /* srcOffset      */ src.Offset(),
    /* dstSubresource */ dst.SubresourceLayers(),
    /* dstOffset      */ dst.Offset(),
    /* extent         */ dst.Extent()
  };
  _vk->cmdCopyImage(CommandBuffer(), src.ImageHandle(), src.Layout(),
                    dst.ImageHandle(), dst.Layout(), 1, &region);
}

Result GraphicsContext::BeginRenderPass(RenderPass &render_pass,
                                      uint32_t width, uint32_t height,
                                      uint32_t layers,
                     std::initializer_list<VkImageView> image_views,
                     std::initializer_list<VkClearValue> clear_values) {
  VkFramebuffer framebuffer;
  VK_RETURN_FAIL(_vk.GetFramebuffer(render_pass.Handle(), width, height,
                                    layers, image_views, framebuffer));

  VkRenderPassBeginInfo begin_info = {
    /* sType           */ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    /* pNext           */ nullptr,
    /* renderPass      */ render_pass.Handle(),
    /* framebuffer     */ framebuffer,
    /* renderArea      */ {
      /* offset */ {
        /* x */ 0,
        /* y */ 0
      },
      /* extent */ {
        /* width  */ width,
        /* height */ height
      }
    },
    /* clearValueCount */ static_cast<uint32_t>(clear_values.size()),
    /* pClearValues    */ clear_values.begin()
  };

  _vk->cmdBeginRenderPass(CommandBuffer(), &begin_info,
                          VK_SUBPASS_CONTENTS_INLINE);

  return Result::kSuccess;
}

}
}
