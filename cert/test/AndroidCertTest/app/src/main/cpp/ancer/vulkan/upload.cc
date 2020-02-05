#include "upload.h"

namespace ancer {
namespace vulkan {

Result Uploader::Initialize(Vulkan &vk, uint32_t concurrent,
                            VkDeviceSize buffer_size) {
  _vk = vk;

  VK_RETURN_FAIL(_buffer.Initialize(vk, ResourceUse::CPUToGPU,
                                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    buffer_size, nullptr));

  _pending.resize(concurrent);

  return Result::kSuccess;
}

Result Uploader::Shutdown() {
  return Result::kSuccess;
}

Result Uploader::Start(VkBuffer buffer, VkDeviceSize size,
                       UploadBuffer &context) {
  VK_RETURN_FAIL(GetSlot(size, context.index, context.offset));

  context.buffer = buffer;
  context.regions.clear();

  return Result::kSuccess;
}

void Uploader::Copy(UploadBuffer &context, VkDeviceSize dst_offset,
                    VkDeviceSize size, const void * data) {
  memcpy(
    reinterpret_cast<uint8_t *>(_buffer.Allocation().Map()) + context.offset,
    data, size);

  context.regions.push_back(VkBufferCopy {
      /* srcOffset */ context.offset,
      /* dstOffset */ dst_offset,
      /* size      */ size
  });
  context.offset += size;
}

Result Uploader::Finish(UploadBuffer &context) {
  VkCommandBuffer cmd_buffer;

  VK_RETURN_FAIL(_vk.AcquireTemporaryCommandBuffer(Q_TRANSFER, cmd_buffer));

  _vk->cmdCopyBuffer(cmd_buffer, _buffer.Handle(), context.buffer,
                     static_cast<uint32_t>(context.regions.size()),
                     context.regions.data());

  VK_RETURN_FAIL(_vk.QueueTemporaryCommandBuffer(cmd_buffer,
                                               _pending[context.index].fence));

  _pending[context.index].in_flight.store(true);

  return Result::kSuccess;
}

Result Uploader::Pump(bool force) {
  bool complete = false;

  // resolve some
  for(uint32_t index = _index_head; index < _index_tail;
      ++index) {
    if(!_pending[index].in_flight.load())
      break;

    VK_RETURN_FAIL(_vk.WaitForFence(_pending[index].fence, complete, force));

    // since we resolve in order, if we cant get one, we cant resolve the
    // remaining
    if(!complete)
      break;

    // update the buffer space used
    _index_head = index;
    _space_head = _pending[index].end;

    // this would be the last, reset
    if(_space_head == _space_tail) {
      _space_head = 0;
      _space_tail = 0;
      _index_head = 0;
      _index_tail = 0;
    }
  }

  return Result::kSuccess;
}

Result Uploader::GetSlot(VkDeviceSize size, uint32_t &index,
                         VkDeviceSize &start) {
  std::lock_guard<std::mutex> guard(_mutex);

  VK_RETURN_FAIL(Pump(false));

  // if we need room, pump all of the remaining entries
  // - because this does a vkWaitForFences with a timeout, we are waiting on the
  // GPU
  while(_buffer.Allocation().Size() - _space_tail < size)
    VK_RETURN_FAIL(Pump(true));

  index = _index_tail;
  start = _space_tail;

  _index_tail += 1;
  _space_tail += size;

  return Result::kSuccess;
}

}
}
