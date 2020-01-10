#ifndef AGDC_ANCER_GRAPHICSCONTEXT_H_
#define AGDC_ANCER_GRAPHICSCONTEXT_H_

#include "vulkan_base.h"

#include <cstdarg>

namespace ancer {
namespace vulkan {

/**
 * Base class for a *Context. Manages the buffers and submit info stuff to
 * control timing on the GPU.
 */
class Context {
 public:
  void Shutdown();

  inline VkCommandBuffer CommandBuffer() const {
    return command_buffers[current_buffer];
  }

  /**
   * We are done with this command buffer, go to the next and prepare it.
   */
  Result Begin();

  /**
   * End recording to the command buffer.
   */
  Result End();

  /**
   * Fill a VkSubmitInfo struct for submitting this command buffer to a queue
   */
  void SubmitInfo(VkSubmitInfo & submit_info) const;

  /**
   * Add a semaphore and stage to wait on to the wait list, meaning synchronize
   * a previously submitted command buffer on a different queue at a particular
   * stage.
   */
  void Wait(VkSemaphore semaphore, VkPipelineStageFlags stages);

  /**
   * Create or get a semaphore that signals the completion of this command
   * buffer on different queues.
   */
  Result CompletedSignal(VkSemaphore & semaphore);

  /**
   * Add this semaphore to the list to signal when completed.
   */
  void Signal(VkSemaphore semaphore);

 protected:
  Result Initialize(Vulkan & vk, uint32_t num_buffers,
                    uint32_t qfi, const char * name);

  Vulkan vk;

 private:
  uint32_t qfi;
  std::vector<VkCommandPool> pools;
  std::vector<VkCommandBuffer> command_buffers;
  std::vector<Fence> command_buffer_fence;
  uint32_t current_buffer;

  bool begin;
  bool end;
  std::vector<VkSemaphore> wait_semaphores;
  std::vector<VkPipelineStageFlags> wait_stage_masks;
  std::vector<VkSemaphore> signal_semaphores;
  VkSemaphore completed_signal;
};

/**
 * A Context for Graphics operations. This will hold all functions unqiue to
 * Graphics queues. This may seem excessive but it will allow us to record
 * timing information on the GPU for each command
 */
class GraphicsContext : public Context {
 public:
  inline Result Initialize(Vulkan & vk, uint32_t num_buffers,
                           const char * name) {
    return Context::Initialize(vk, num_buffers,
                               vk.vk->graphics_queue_family_index, name);
  }
};

/**
 * A Context for Compute operations. This will hold all functions unqiue to
 * Compute queues. This may seem excessive but it will allow us to record
 * timing information on the GPU for each command
 */
class ComputeContext : public Context {
 public:
  inline Result Initialize(Vulkan & vk, uint32_t num_buffers,
                           const char * name) {
    return Context::Initialize(vk, num_buffers,
                               vk.vk->compute_queue_family_index, name);
  }
};

/**
 * A Context for Transfer operations. This will hold all functions unqiue to
 * Transfer queues. This may seem excessive but it will allow us to record
 * timing information on the GPU for each command
 */
class TransferContext : public Context {
 public:
  inline Result Initialize(Vulkan & vk, uint32_t num_buffers,
                           const char * name) {
    return Context::Initialize(vk, num_buffers,
                               vk.vk->transfer_queue_family_index, name);
  }
};

}
}

#endif
