#include "swapchain.h"

namespace ancer {
namespace vulkan {

Result Swapchain::Initialize(Vulkan & _vulkan, VkImageUsageFlags usage_flags) {
  vulkan = _vulkan;
  auto & vk = vulkan.vk;

  VkExtent2D extent;

  if(vk->surface_capabilities.currentExtent.width == 0xFFFFFFFF ||
     vk->surface_capabilities.currentExtent.height == 0xFFFFFFFF) {
    extent.width = vk->window_width;
    extent.width = std::max(extent.width,
                            vk->surface_capabilities.minImageExtent.width);
    extent.width = std::min(extent.width,
                            vk->surface_capabilities.maxImageExtent.width);

    extent.height = vk->window_height;
    extent.height = std::max(extent.height,
                             vk->surface_capabilities.minImageExtent.height);
    extent.height = std::min(extent.height,
                             vk->surface_capabilities.maxImageExtent.height);
  } else
    extent = vk->surface_capabilities.currentExtent;

  uint32_t image_count = 2;
  image_count = std::max(image_count, vk->surface_capabilities.minImageCount);
  image_count = std::min(image_count, vk->surface_capabilities.maxImageCount);

  if(vk->surface_formats.size() == 1 &&
     vk->surface_formats[0].format == VK_FORMAT_UNDEFINED) {
    format = VK_FORMAT_B8G8R8A8_UNORM;
  } else {
    if(vk->surface_formats.size() == 0)
      return Result(VK_ERROR_INITIALIZATION_FAILED);
    format = vk->surface_formats[0].format;
  }

  uint32_t qfi[2] = {
    vk->graphics.family_index,
    vk->present.family_index
  };
  uint32_t qfi_count = qfi[0] != qfi[1] ? 2 : 1;

  VkSurfaceTransformFlagBitsKHR pre_transform;
  if(vk->surface_capabilities.supportedTransforms &
     VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  else
    pre_transform = vk->surface_capabilities.currentTransform;

  VkCompositeAlphaFlagBitsKHR composite_alpha =
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  const VkCompositeAlphaFlagBitsKHR composite_alpha_flags[4] = {
    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
    VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
    VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
  };
  for(const auto & flags : composite_alpha_flags) {
    if(vk->surface_capabilities.supportedCompositeAlpha & flags) {
      composite_alpha = flags;
      break;
    }
  }

  VkSwapchainCreateInfoKHR create_info = {
    /* sType                 */ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    /* pNext                 */ nullptr,
    /* flags                 */ 0,
    /* surface               */ vk->surface,
    /* minImageCount         */ image_count,
    /* imageFormat           */ format,
    /* imageColorSpace       */ VK_COLORSPACE_SRGB_NONLINEAR_KHR,
    /* imageExtent           */ extent,
    /* imageArrayLayers      */ 1,
    /* imageUsage            */ usage_flags,
    /* imageSharingMode      */ qfi_count > 1 ? VK_SHARING_MODE_CONCURRENT :
                                                VK_SHARING_MODE_EXCLUSIVE,
    /* queueFamilyIndexCount */ qfi_count,
    /* pQueueFamilyIndices   */ qfi,
    /* preTransform          */ pre_transform,
    /* compositeAlpha        */ composite_alpha,
    /* presentMode           */ VK_PRESENT_MODE_FIFO_KHR,
    /* clipped               */ VK_FALSE,
    /* oldSwapchain          */ VK_NULL_HANDLE
  };

  VK_RETURN_FAIL(vk->createSwapchainKHR(vk->device, &create_info, nullptr,
                                        &swapchain));

  uint32_t count = 0;
  VK_RETURN_FAIL(vk->getSwapchainImagesKHR(vk->device, swapchain, &count,
                                           nullptr));

  images.resize(count);
  VK_RETURN_FAIL(vk->getSwapchainImagesKHR(vk->device, swapchain, &count,
                                           images.data()));

  for(auto & image : images) {
    VkImageViewCreateInfo view_create_info = {
      /* sType            */ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      /* pNext            */ nullptr,
      /* flags            */ 0,
      /* image            */ image,
      /* viewType         */ VK_IMAGE_VIEW_TYPE_2D,
      /* format           */ format,
      /* components       */ {
        /* r */ VK_COMPONENT_SWIZZLE_R,
        /* g */ VK_COMPONENT_SWIZZLE_G,
        /* b */ VK_COMPONENT_SWIZZLE_B,
        /* a */ VK_COMPONENT_SWIZZLE_A,
      },
      /* subresourceRange */ {
        /* aspectMask     */ VK_IMAGE_ASPECT_COLOR_BIT,
        /* baseMipLevel   */ 0,
        /* levelCount     */ 1,
        /* baseArrayLayer */ 0,
        /* layerCount     */ 1
      }
    };

    VkImageView image_view;
    VK_RETURN_FAIL(vk->createImageView(vk->device, &view_create_info,
                                       nullptr, &image_view));

    image_views.push_back(image_view);

    used_semaphores.push_back(VK_NULL_HANDLE);
  }

  width = extent.width;
  height = extent.height;

  return Result::kSuccess;
}

void Swapchain::Shutdown() {
}

Result Swapchain::Acquire(SwapchainImage & image) {
  auto & vk = vulkan.vk;

  VkSemaphoreCreateInfo semaphore_create_info = {
    /* sType */ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    /* pNext */ nullptr,
    /* flags */ 0
  };
  VK_RETURN_FAIL(vk->createSemaphore(vk->device, &semaphore_create_info,
                                     nullptr, &image.semaphore));

  VK_RETURN_FAIL(vk->acquireNextImageKHR(vk->device, swapchain, UINT64_MAX,
                                         image.semaphore, VK_NULL_HANDLE,
                                         &image.index));

  image.image = images[image.index];
  image.image_view = image_views[image.index];

  return Result::kSuccess;
}

Result Swapchain::Present(SwapchainImage & image,
                          VkSemaphore render_semaphore) {
  auto & vk = vulkan.vk;

  // cleanup of semaphores. since we wait on the CPU to acquire, the previous
  // semaphore will have been waited on
  if(used_semaphores[image.index] != VK_NULL_HANDLE) {
    vk->destroySemaphore(vk->device, used_semaphores[image.index], nullptr);
  }
  used_semaphores[image.index] = image.semaphore;

  // do the presentation
  VkPresentInfoKHR present_info = {
    /* sType              */ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    /* pNext              */ nullptr,
    /* waitSemaphoreCount */ render_semaphore == VK_NULL_HANDLE ? 0u : 1u,
    /* pWaitSemaphores    */ &render_semaphore,
    /* swapchainCount     */ 1,
    /* pSwapchains        */ &swapchain,
    /* pImageIndices      */ &image.index,
    /* pResults           */ nullptr
  };

  return Result(vk->queuePresentKHR(vk->present.queue, &present_info));
}

}
}
