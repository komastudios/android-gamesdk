#ifndef AGDC_ANCER_SWAPCHAIN_H_
#define AGDC_ANCER_SWAPCHAIN_H_

#include "vulkan_base.h"

namespace ancer {
namespace vulkan {

/**
 * An image acquired from the swapchain. Contains the image, image view, index
 * (usefull for pre-processing based on the images from the swapchain) and a
 * semaphore for syncing the image acuire operation to be before writing to the
 * image.
 */
struct SwapchainImage {
  uint32_t index;
  VkImage image;
  VkImageView image_view;
  VkSemaphore semaphore;
};

/**
 * This is a useful wrapper for a VkSwapchain. It handles Creation of the
 * swapchain and the VkImageViews needed for rendering, Acquiring, and
 * Presenting.
 */
class Swapchain {
 public:
  Result Initialize(Vulkan & vulkan, VkImageUsageFlags usage_flags);
  void Shutdown();

  inline VkFormat Format() const {
    return format;
  }

  inline uint32_t NumImages() const {
    return images.size();
  }

  inline VkImage Image(uint32_t index) const {
    return images[index];
  }

  inline VkImageView ImageView(uint32_t index) const {
    return image_views[index];
  }

  /**
   * Acquire an image from the VkSwapchain, filling a SwapchainImage struct
   */
  Result Acquire(SwapchainImage & image);

  /**
   * Schedule a SwapchainImage to be shown on the display. The optional
   * parameter render_semaphore can be used to sync the end of the rendering to
   * the presentation to the display.
   */
  Result Present(SwapchainImage & image,
                 VkSemaphore render_semaphore = VK_NULL_HANDLE);

 private:
  Vulkan vulkan;

  VkFormat format;

  VkSwapchainKHR swapchain;
  std::vector<VkImage> images;
  std::vector<VkImageView> image_views;
  std::vector<VkSemaphore> used_semaphores;
};

}
}

#endif
