#include "image.h"

#include "upload.h"
#include "format.h"

namespace ancer {
namespace vulkan {

Result Image::Initialize(Vulkan &vk, VkResourceUse ruse,
                         VkImageUsageFlags usage, uint32_t width,
                         uint32_t height, uint32_t depth, uint32_t levels,
                         uint32_t layers, VkFormat format,
                         VkImageLayout layout, void * data) {
  _vk = vk;

  VkImageType image_type = VK_IMAGE_TYPE_2D;
  VkImageViewType image_view_type = VK_IMAGE_VIEW_TYPE_2D;

  if(height == 1 && depth == 1) {
    image_type = VK_IMAGE_TYPE_1D;
    if(layers > 1)
      image_view_type = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
  } else if(depth == 1) {
    image_type = VK_IMAGE_TYPE_3D;
    image_view_type = VK_IMAGE_VIEW_TYPE_3D;
  } else if(layers > 1)
    image_view_type = VK_IMAGE_VIEW_TYPE_2D_ARRAY;

  if(data != nullptr)
    usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

  VkImageCreateInfo create_info = {
    /* sType                 */ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    /* pNext                 */ nullptr,
    /* flags                 */ 0,
    /* imageType             */ image_type,
    /* format                */ format,
    /* extent                */ {
      /* width  */ width,
      /* height */ height,
      /* depth  */ depth
    },
    /* mipLevels             */ levels,
    /* arrayLayers           */ layers,
    /* samples               */ VK_SAMPLE_COUNT_1_BIT,
    /* tiling                */ VK_IMAGE_TILING_OPTIMAL,
    /* usage                 */ usage,
    /* sharingMode           */ VK_SHARING_MODE_EXCLUSIVE,
    /* queueFamilyIndexCount */ 0,
    /* pQueueFamilyIndices   */ nullptr,
    /* initialLayout         */ layout
  };

  VK_RETURN_FAIL(_vk->createImage(_vk->device, &create_info, nullptr,
                                  &_image));

  VkMemoryRequirements requirements;
  _vk->getImageMemoryRequirements(_vk->device, _image, &requirements);

  VkMemoryPropertyFlags memory_flags = 0;

  switch(ruse) {
    case VK_RESOURCE_GPU:
      memory_flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      break;
    case VK_RESOURCE_TRANSIENT_GPU:
      memory_flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
                      VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
      break;
    case VK_RESOURCE_CPU_TO_GPU:
      memory_flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                      VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
      break;
    case VK_RESOURCE_GPU_TO_CPU:
      memory_flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                      VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
      break;
  }

  Result res;

  VK_GOTO_FAIL(res = _vk.AllocateMemory(requirements, memory_flags, _memory));

  VK_GOTO_FAIL(res = _vk->bindImageMemory(_vk->device, _image,
                                          _memory.Memory(),
                                          _memory.Offset()));

  // TODO(sarahburns@google.com)
#if 0
  if(data != nullptr) {
    auto &uploader = _vk.GetUploader();

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

    auto format_properties = GetFormatProperties(_vk, format);

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
  }
#endif

  if(false) {
fail:
    _vk.Free(_memory);
    if(_image != VK_NULL_HANDLE)
      _vk.Destroy(_image, true);
    return res;
  }

  return Result::kSuccess;
}

void Image::Shutdown() {
  _vk.Free(_memory);
  _vk.Destroy(_image, true);
  _image = VK_NULL_HANDLE;
}

}
}
