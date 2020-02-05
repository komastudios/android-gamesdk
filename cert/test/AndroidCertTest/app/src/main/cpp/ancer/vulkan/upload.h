#ifndef AGDC_ANCER_UPLOAD_H_
#define AGDC_ANCER_UPLOAD_H_

#include <cstdio>

#include "vulkan_base.h"
#include "buffer.h"

namespace ancer {
namespace vulkan {

/**
 * A uploading context for copying data to a staging buffer into a buffer
 */
struct UploadBuffer {
  uint32_t index;
  VkDeviceSize offset;
  VkBuffer buffer;
  std::vector<VkBufferCopy> regions;
};

/**
 * This class uses a single staging buffer that can have up to N concurrent
 * uploads to it and copy to Image or Buffer data. If the there is limited space
 * or uploads it waits on the current ones before if continues.
 *
 * designed for a multi-threaded environment
 */
class Uploader {
 public:
  Result Initialize(Vulkan &vk, uint32_t concurrent, VkDeviceSize buffer_size);

  Result Shutdown();

  Result Start(VkBuffer buffer, VkDeviceSize size, UploadBuffer &context);

  void Copy(UploadBuffer &context, VkDeviceSize dst_offset, VkDeviceSize size,
            const void * data);

  Result Finish(UploadBuffer &context);

 private:
  struct Pending {
    Pending() : end(), fence(), in_flight(false) { }

    Pending(const Pending & other) : end(other.end), fence(other.fence) {
      in_flight.store(other.in_flight.load());
    }

    VkDeviceSize end;
    Fence fence;
    std::atomic<bool> in_flight;
  };

  Result Pump(bool force);

  Result GetSlot(VkDeviceSize size, uint32_t &index,
                 VkDeviceSize &start);

  Vulkan _vk;

  Buffer _buffer;

  // TODO(sarahburns@google): do this without a mutex
  std::mutex _mutex;
  VkDeviceSize _space_head;
  VkDeviceSize _space_tail;
  uint32_t _index_head;
  uint32_t _index_tail;
  std::vector<Pending> _pending;
};

}
}

#endif
