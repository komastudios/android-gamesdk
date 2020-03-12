#ifndef AGDC_ANCER_GRAPHICSCONTEXT_H_
#define AGDC_ANCER_GRAPHICSCONTEXT_H_

#include "vulkan_base.h"
#include "render_pass.h"
#include "resources.h"
#include "image.h"

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
    return _command_buffers[_current_buffer];
  }

  /**
   * We are done with this command buffer, go to the next and prepare it.
   */
  Result Begin();

  /**
   *
   */
  Result BindResources(VkPipelineBindPoint bind_point, VkPipelineLayout layout,
                       uint32_t first,
                       std::initializer_list<Resources> resources);

  /**
   * End recording to the command buffer.
   */
  Result End();

  /**
   * Set the fence used for the current command buffer
   */
  Result SetFence(Fence & fence);

  /**
   * Get the fence used for the current command buffer
   */
  Result GetFence(Fence & fence, bool create_advancing_fence = false);

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

  /// utility functions, replacement for vkCmd*
 public:
  static void ChangeImageLayout(Vulkan &vk, VkCommandBuffer cbuf,
                                ImageResource image, VkImageLayout oldLayout);

  void ChangeImageLayout(ImageResource image, VkImageLayout oldLayout);

  void CopyImage(ImageResource src, ImageResource dst);

  void BlitImage(ImageResource src, ImageResource dst,
                 VkFilter filter = VK_FILTER_LINEAR);

 protected:
  Result Initialize(Vulkan & vk, uint32_t num_buffers,
                    uint32_t qfi, const char * name);

  Vulkan _vk;

 private:
  Result ClearFence();

  uint32_t _qfi;
  std::vector<VkCommandPool> _pools;
  std::vector<VkCommandBuffer> _command_buffers;
  std::vector<Fence> _command_buffer_fence;
  uint32_t _current_buffer;

  bool _begin;
  bool _end;
  std::vector<VkSemaphore> _wait_semaphores;
  std::vector<VkPipelineStageFlags> _wait_stage_masks;
  std::vector<VkSemaphore> _signal_semaphores;
  VkSemaphore _completed_signal;
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
    return Context::Initialize(vk, num_buffers, vk->graphics.family_index,
                               name);
  }

  /**
   * Gets or creates a compatible VkFramebuffer and starts the RenderPass with
   * it
   */
  Result BeginRenderPass(RenderPass &render_pass,
                         uint32_t width, uint32_t height,
                         uint32_t layers,
                         std::initializer_list<VkImageView> image_views,
                         std::initializer_list<VkClearValue> clear_values);
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
    return Context::Initialize(vk, num_buffers, vk->compute.family_index,
                               name);
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
    return Context::Initialize(vk, num_buffers, vk->transfer.family_index,
                               name);
  }
};

}
}

#endif
