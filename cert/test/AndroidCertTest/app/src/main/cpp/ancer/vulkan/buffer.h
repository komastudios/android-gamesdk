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
  Buffer();

  Result Initialize(Vulkan &vk, EResourceUse ruse, VkBufferUsageFlags usage,
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
    return _range;
  }

  void *MapVoid();

  void Unmap();

  template<typename T>
  T *Map() {
    return reinterpret_cast<T *>(MapVoid());
  }

 private:
  Vulkan _vk;
  VkBuffer _buffer;
  VkBufferView _view;
  VkDeviceSize _range;
  MemoryAllocation _memory;
};

class IndexBuffer : public Buffer {
 public:
  inline Result Initialize(Vulkan &vk, EResourceUse ruse,
                           VkIndexType index_type, VkDeviceSize size,
                           void * data) {
    _index_type = index_type;
    return Buffer::Initialize(vk, ruse, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                              size, data);
  }

  inline VkIndexType IndexType() const {
    return _index_type;
  }

 private:
  VkIndexType _index_type;
};

class VertexBuffer : public Buffer {
 public:
  inline Result Initialize(Vulkan &vk, EResourceUse ruse, VkDeviceSize size,
                           void * data) {
    return Buffer::Initialize(vk, ruse, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                              size, data);
  }
};

class UniformBuffer : public Buffer {
 public:
  inline Result Initialize(Vulkan &vk, EResourceUse ruse, VkDeviceSize size,
                           void * data) {
    return Buffer::Initialize(vk, ruse, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                              size, data);
  }
};

class StorageBuffer : public Buffer {
 public:
  inline Result Initialize(Vulkan &vk, EResourceUse ruse, VkDeviceSize size,
                           void * data) {
    return Buffer::Initialize(vk, ruse, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                              size, data);
  }
};

}
}

#endif
