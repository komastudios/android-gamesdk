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
