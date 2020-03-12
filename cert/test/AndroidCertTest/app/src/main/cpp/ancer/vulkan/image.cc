#include "image.h"

#include "upload.h"
#include "format.h"
#include <ancer/util/Log.hpp>
#include <ancer/vulkan/vk_debug_print.h>

namespace ancer {
namespace vulkan {

namespace {
static Log::Tag TAG{"VulkanImage"};
}

VkImage ImageResource::ImageHandle() const {
  return _image.Handle();
}

ImageResource::ImageResource(const Image &image) : _image(image) {
  _level_min = 0;
  _level_max = image._levels;
  _layer_min = 0;
  _layer_max = image._layers;
  _offset.x = 0;
  _offset.y = 0;
  _offset.z = 0;
  _extent.width = image._width;
  _extent.height = image._height;
  _extent.depth = image._depth;
}

namespace image {

Builder &Builder::Format(VkFormat format) {
  _create_info.format = format;
  return *this;
}

Builder &Builder::Extents(uint32_t width, uint32_t height, uint32_t depth) {
  _create_info.extent.width = width;
  _create_info.extent.height = height;
  _create_info.extent.depth = depth;
  return *this;
}

Builder &Builder::Levels(uint32_t levels) {
  _create_info.mipLevels = levels;
  return *this;
}

Builder &Builder::Layers(uint32_t layers) {
  _create_info.arrayLayers = layers;
  return *this;
}

Builder &Builder::Samples(VkSampleCountFlagBits samples) {
  _create_info.samples = samples;
  return *this;
}

Builder &Builder::Linear(bool value) {
  if(value) {
    _create_info.tiling = VK_IMAGE_TILING_LINEAR;
  } else {
    _create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  }
  return *this;
}

Builder &Builder::Usage(VkImageUsageFlags flags, bool add) {
  if(add) {
    _create_info.usage |= flags;
  } else {
    _create_info.usage &= ~flags;
  }
  return *this;
}

Builder &Builder::Layout(VkImageLayout layout) {
  _create_info.initialLayout = layout;
  return *this;
}

Builder &Builder::ResourceUse(EResourceUse ruse) {
  _ruse = ruse;
  return *this;
}

Builder &Builder::Queue(EQueue q, bool add) {
  if(add) {
    _queues |= 1 << q;
  } else {
    _queues &= ~(1 << q);
  }
  return *this;
}

Builder &Builder::Data(void * data) {
  _data = data;
  return *this;
}

Result Builder::Build(Vulkan &vk) {
  _image._vk = vk;

  VkImageType image_type = VK_IMAGE_TYPE_2D;
  VkImageViewType image_view_type = VK_IMAGE_VIEW_TYPE_2D;

  if(_create_info.extent.height == 1 && _create_info.extent.depth == 1) {
    image_type = VK_IMAGE_TYPE_1D;
    if(_create_info.arrayLayers > 1)
      image_view_type = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
  } else if(_create_info.extent.depth > 1) {
    image_type = VK_IMAGE_TYPE_3D;
    image_view_type = VK_IMAGE_VIEW_TYPE_3D;
  } else if(_create_info.arrayLayers > 1)
    image_view_type = VK_IMAGE_VIEW_TYPE_2D_ARRAY;

  if(_data != nullptr) {
    (void)Queue(Q_TRANSFER);
    (void)Usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT);
  }

  _image._width = _create_info.extent.width;
  _image._height = _create_info.extent.height;
  _image._depth = _create_info.extent.depth;
  _image._levels = _create_info.mipLevels;
  _image._layers = _create_info.arrayLayers;
  _image._layout = _create_info.initialLayout;

  _create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  _create_info.pNext = nullptr;
  _create_info.flags = 0;
  _create_info.imageType = image_type;
  _create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  _create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  _create_info.queueFamilyIndexCount = 0;
  _create_info.pQueueFamilyIndices = nullptr;

  uint32_t qbits = 0;
  uint32_t qfi[3] = { };

#define ADD_QUEUE(Q, I) \
  do { \
    if (_queues & (1 << Q) && (qbits & (1 << I)) == 0) { \
      qbits |= (1 << I); \
      qfi[_create_info.queueFamilyIndexCount++] = I; \
      _create_info.pQueueFamilyIndices = qfi; \
    } \
  } while(0)

  ADD_QUEUE(Q_GRAPHICS, vk->graphics.family_index);
  ADD_QUEUE(Q_COMPUTE, vk->compute.family_index);
  ADD_QUEUE(Q_TRANSFER, vk->transfer.family_index);

#undef ADD_QUEUE

  VkImageCreateInfo_debug_print("create_info", 0, _create_info);

  VK_RETURN_FAIL(vk->createImage(vk->device, &_create_info, nullptr,
                                 &_image._image));

  VkMemoryRequirements requirements;
  vk->getImageMemoryRequirements(vk->device, _image._image, &requirements);

  VkMemoryPropertyFlags memory_flags = 0;

  switch(_ruse) {
    case EResourceUse::GPU:
      memory_flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      break;
    case EResourceUse::TransientGPU:
      memory_flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
                      VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
      break;
    case EResourceUse::CPUToGPU:
      memory_flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                      VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
      break;
    case EResourceUse::GPUToCPU:
      memory_flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                      VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
      break;
  }

  Result res;

  VK_GOTO_FAIL(res = vk.AllocateMemory(requirements, memory_flags,
                                       _image._memory));

  VK_GOTO_FAIL(res = vk->bindImageMemory(vk->device, _image._image,
                                         _image._memory.Memory(),
                                         _image._memory.Offset()));

  // TODO(sarahburns@google.com)
#if 0
  if(data != nullptr) {
    auto &uploader = vk.GetUploader();

    UploadImage upload;

    VK_GOTO_FAIL(res = uploader.Start(_image, requirements.size, upload));

    uint8_t * image_data = reinterpret_cast<uint8_t*>(data);

    VkImageSubresourceLayers subresource_layers = {
      /* aspectMask     */ VK_IMAGE_ASPECT_COLOR_BIT,
      /* mipLevel       */ 0,
      /* baseArrayLayer */ 0,
      /* layerCount     */ layers
    };

    VkOffset3D offset = {};
    VkExtent3D extent = create_info.extent;

    auto format_properties = GetFormatProperties(vk, format);

    uint32_t &level = subresource_layers.mipLevel;
    for(; level < levels; ++level) {
      uploader.Copy(upload, subresource_layers, offset, extent);

      image_data += format_properties.size_in_bytes * extent.width *
                    extent.height * extent.depth;

      extent.width = std::max(1u, extent.width >> 1);
      extent.height = std::max(1u, extent.height >> 1);
      extent.depth = std::max(1u, extent.depth >> 1);
    }

    uploader.Copy(upload, 0, size, data);

    VK_GOTO_FAIL(uploader.Finish(upload));
  } else
#endif
  {
    VkCommandBuffer buf;
    VK_GOTO_FAIL(res = vk.AcquireTemporaryCommandBuffer(Q_TRANSFER, buf));
  }

  if(false) {
fail:
    vk.Free(_image._memory);
    if(_image._image != VK_NULL_HANDLE)
      vk.Destroy(_image._image, true);
    return res;
  }

  return Result::kSuccess;
}

Builder::Builder(Image &image) : _image(image) {
}

}

image::Builder Image::Builder() {
  return image::Builder(*this);
}

Result Image::Initialize(Vulkan &vk, EResourceUse ruse,
                         VkImageUsageFlags usage, uint32_t width,
                         uint32_t height, uint32_t depth, uint32_t levels,
                         uint32_t layers, VkFormat format,
                         VkImageLayout layout, void * data) {
  return Builder()
      .Format(format)
      .Extents(width, height, depth)
      .Levels(levels)
      .Layers(layers)
      .Samples(VK_SAMPLE_COUNT_1_BIT)
      .Linear(false)
      .Usage(usage)
      .Layout(layout)
      .ResourceUse(ruse)
      .Data(data)
      .Build(vk);
}

void Image::Shutdown() {
  _vk.Free(_memory);
  _vk.Destroy(_image, true);
  _image = VK_NULL_HANDLE;
}

}
}
