#include "context.h"

namespace ancer {
namespace vulkan {

Result Context::Initialize(Vulkan & vulkan, uint32_t num_buffers,
                           uint32_t _qfi, const char * name) {
  Result result;

  Shutdown();

  qfi = _qfi;
  vk = vulkan;

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

    VK_GOTO_FAIL(result = vulkan.DebugName(command_pool, "Command Pool %s %i",
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

    VK_GOTO_FAIL(result = vulkan.DebugName(command_buffer,
                                           "Command Buffer %s %i",
                                           name, i));

    pools.push_back(command_pool);
    command_buffers.push_back(command_buffer);
    command_buffer_fence.push_back(Fence());
  }

  if(false) {
fail:
    Shutdown();
    return result;
  }

  return Result::kSuccess;
}

void Context::Shutdown() {
  for(uint32_t i = 0; i < command_buffers.size(); ++i)
    vk->freeCommandBuffers(vk->device, pools[i], 1, &command_buffers[i]);

  for(uint32_t i = 0; i < pools.size(); ++i)
    vk->destroyCommandPool(vk->device, pools[i], nullptr);

  current_buffer = 0;
}

Result Context::Begin() {
  current_buffer = (current_buffer + 1) % command_buffers.size();

  bool complete;
  VK_RETURN_FAIL(vk.WaitForFence(command_buffer_fence[current_buffer],
                                     complete));

  if(!complete)
    return Result(VK_TIMEOUT);

  VkCommandBufferBeginInfo begin_info = {
    /* sType            */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    /* pNext            */ nullptr,
    /* flags            */ 0,
    /* pInheritanceInfo */ nullptr
  };

  VK_RETURN_FAIL(vk->beginCommandBuffer(CommandBuffer(), &begin_info));

  end = false;
  wait_semaphores.clear();
  wait_stage_masks.clear();
  signal_semaphores.clear();
  completed_signal = VK_NULL_HANDLE;

  return Result::kSuccess;
}

Result Context::End() {
  if(!end) {
    VK_RETURN_FAIL(vk->endCommandBuffer(CommandBuffer()));
    end = true;
  }

  return Result::kSuccess;
}

void Context::SubmitInfo(VkSubmitInfo & si) const {
  si.waitSemaphoreCount = static_cast<uint32_t>(wait_semaphores.size());
  si.pWaitSemaphores = wait_semaphores.empty() ? nullptr
                                               : wait_semaphores.data();
  si.pWaitDstStageMask = wait_semaphores.empty() ? nullptr
                                                 : wait_stage_masks.data();
  si.commandBufferCount = 1;
  si.pCommandBuffers = &command_buffers[current_buffer];
  si.signalSemaphoreCount = static_cast<uint32_t>(signal_semaphores.size());
  si.pSignalSemaphores = signal_semaphores.empty() ? nullptr
                                                   : signal_semaphores.data();
}

void Context::Wait(VkSemaphore semaphore, VkPipelineStageFlags stages) {
  wait_semaphores.push_back(semaphore);
  wait_stage_masks.push_back(stages);
}

Result Context::CompletedSignal(VkSemaphore & semaphore) {
  if(completed_signal == VK_NULL_HANDLE) {
    VkSemaphoreCreateInfo semaphore_create_info = {
      /* sType */ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      /* pNext */ nullptr,
      /* flags */ 0
    };
    VK_RETURN_FAIL(vk->createSemaphore(vk->device, &semaphore_create_info,
                                       nullptr, &completed_signal));

    Signal(completed_signal);
  }

  semaphore = completed_signal;

  return Result::kSuccess;
}

void Context::Signal(VkSemaphore semaphore) {
  signal_semaphores.push_back(semaphore);
}

}
}
