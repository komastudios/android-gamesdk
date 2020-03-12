#ifndef AGDC_ANCER_IMAGE_H_
#define AGDC_ANCER_IMAGE_H_

#include <cstdio>

#include "vulkan_base.h"

namespace ancer {
namespace vulkan {

class Image;

struct [[nodiscard]] ImageResource {
 public:
  inline ImageResource &Level(uint32_t level) {
    _level_min = level;
    _level_max = level + 1;
    return *this;
  }

  inline ImageResource &Levels(uint32_t min, uint32_t max) {
    _level_min = min;
    _level_max = max;
    return *this;
  }

  inline ImageResource &Layer(uint32_t layer) {
    _layer_min = layer;
    _layer_max = layer + 1;
    return *this;
  }

  inline ImageResource &Layers(uint32_t min, uint32_t max) {
    _layer_min = min;
    _layer_max = max;
    return *this;
  }

  inline ImageResource &Offset(uint32_t x, uint32_t y, uint32_t z) {
    _offset.x = x;
    _offset.y = y;
    _offset.z = z;
    return *this;
  }

  inline ImageResource &Extent(uint32_t width, uint32_t height, uint32_t depth) {
    _extent.width = width;
    _extent.height = height;
    _extent.depth = depth;
    return *this;
  }

  inline ImageResource &Layout(VkImageLayout layout) {
    _layout = layout;
    return *this;
  }

  VkImage ImageHandle() const;

  inline VkOffset3D Offset() const {
    return _offset;
  }

  inline VkExtent3D Extent() const {
    return _extent;
  }

  inline VkImageLayout Layout() const {
    return _layout;
  }

  inline VkImageSubresourceLayers SubresourceLayers() const {
    VkImageSubresourceLayers res = {
      _aspects,
      _level_min,
      _layer_min,
      _layer_max - _layer_min
    };
    return res;
  }

  inline VkImageSubresourceRange SubresourceRange() const {
    VkImageSubresourceRange res = {
      _aspects,
      _level_min,
      _level_max - _level_min,
      _layer_min,
      _layer_max - _layer_min
    };
    return res;
  }

 private:
  friend Image;

  ImageResource(const Image &image);

  const Image &_image;
  VkImageAspectFlags _aspects;
  uint32_t _level_min;
  uint32_t _level_max;
  uint32_t _layer_min;
  uint32_t _layer_max;
  VkOffset3D _offset;
  VkExtent3D _extent;
  VkImageLayout _layout;
};

namespace image {

class [[nodiscard]] Builder {
 public:
  Builder &Format(VkFormat format);
  Builder &Extents(uint32_t width, uint32_t height = 1, uint32_t depth = 1);
  Builder &Levels(uint32_t levels);
  Builder &Layers(uint32_t layers);
  Builder &Samples(VkSampleCountFlagBits samples);
  Builder &Linear(bool value = true);
  Builder &Usage(VkImageUsageFlags flags, bool add = true);
  Builder &Layout(VkImageLayout layout);
  Builder &ResourceUse(EResourceUse ruse);
  Builder &Queue(EQueue q, bool add = true);
  Builder &Data(void * data);

  Result Build(Vulkan &vk);

 private:
  friend Image;

  Builder(Image &image);

  EResourceUse _ruse;
  VkImageCreateInfo _create_info;
  void * _data = nullptr;
  uint8_t _queues = 0;
  Image &_image;
};

}

/**
 * A Vulkan Image, Images are memory allocated
 */
class Image {
 public:
  image::Builder Builder();

  Result Initialize(Vulkan &vk,
                    EResourceUse ruse, VkImageUsageFlags usage,
                    uint32_t width, uint32_t height, uint32_t depth,
                    uint32_t levels, uint32_t layers,
                    VkFormat format, VkImageLayout layout, void * data);

  void Shutdown();

  inline ImageResource Resource() const {
    return ImageResource(*this);
  }

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
  friend struct ImageResource;
  friend class image::Builder;

  Vulkan _vk;
  VkImage _image;
  VkImageView _view;
  VkSampler _sampler;

  uint32_t _width;
  uint32_t _height;
  uint32_t _depth;
  uint32_t _levels;
  uint32_t _layers;
  VkImageLayout _layout;
  
  MemoryAllocation _memory;
};

}
}

#endif
