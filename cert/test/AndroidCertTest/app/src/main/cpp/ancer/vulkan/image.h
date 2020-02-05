#ifndef AGDC_ANCER_IMAGE_H_
#define AGDC_ANCER_IMAGE_H_

#include <cstdio>

#include "vulkan_base.h"

namespace ancer {
namespace vulkan {

/**
 * A Vulkan Image, Images are memory allocated
 */
class Image {
 public:
  Result Initialize(Vulkan &vk,
                    VkResourceUse ruse, VkImageUsageFlags usage,
                    uint32_t width, uint32_t height, uint32_t depth,
                    uint32_t levels, uint32_t layers,
                    VkFormat format, VkImageLayout layout, void * data);

  void Shutdown();

  inline VkImage Handle() const {
    return _image;
  }

  inline VkImageView View() const {
    return _view;
  }

  inline VkSampler Sampler() const {
    return _sampler;
  }

  inline VkImageLayout Layout() const {
    return _layout;
  }

  inline const MemoryAllocation &Allocation() const {
    return _memory;
  }

 private:
  Vulkan _vk;
  VkImage _image;
  VkImageView _view;
  VkSampler _sampler;
  VkImageLayout _layout;
  MemoryAllocation _memory;
};

}
}

#endif
