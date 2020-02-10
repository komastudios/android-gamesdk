#ifndef AGDC_ANCER_BUFFER_H_
#define AGDC_ANCER_BUFFER_H_

#include <cstdio>

#include "vulkan_base.h"

namespace ancer {
namespace vulkan {

/**
 * A Vulkan Buffer, has a memory allocation
 */
class Buffer {
 public:
  Result Initialize(Vulkan &vk, ResourceUse ruse, VkBufferUsageFlags usage,
                    VkDeviceSize size, void * data);

  void Shutdown();

  inline VkBuffer Handle() const {
    return _buffer;
  }

  inline VkBufferView View() const {
    return _view;
  }

  inline const MemoryAllocation &Allocation() const {
    return _memory;
  }

  inline VkDeviceSize Offset() const {
    return 0;
  }

  inline VkDeviceSize Range() const {
    return _memory.Size();
  }

 private:
  Vulkan _vk;
  VkBuffer _buffer;
  VkBufferView _view;
  MemoryAllocation _memory;
};

class IndexBuffer : public Buffer {
 public:
  inline Result Initialize(Vulkan &vk, ResourceUse ruse, VkDeviceSize size,
                           void * data) {
    return Buffer::Initialize(vk, ruse, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                              size, data);
  }
};

class VertexBuffer : public Buffer {
 public:
  inline Result Initialize(Vulkan &vk, ResourceUse ruse, VkDeviceSize size,
                           void * data) {
    return Buffer::Initialize(vk, ruse, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                              size, data);
  }
};

}
}

#endif
