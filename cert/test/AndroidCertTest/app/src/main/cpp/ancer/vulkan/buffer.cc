#include "buffer.h"

#include "upload.h"

namespace ancer {
namespace vulkan {

Buffer::Buffer() : _buffer(VK_NULL_HANDLE), _view(VK_NULL_HANDLE), _memory() {}

Result Buffer::Initialize(Vulkan &vk, EResourceUse ruse,
                          VkBufferUsageFlags usage, VkDeviceSize size,
                          void *data) {
  _vk = vk;

  VkMemoryPropertyFlags memory_flags = 0;

  if (data != nullptr) {
    usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  }

  VkBufferCreateInfo create_info = {
      /* sType                 */ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      /* pNext                 */ nullptr,
      /* flags                 */ 0,
      /* size                  */ size,
      /* usage                 */ usage,
      /* sharingMode           */ VK_SHARING_MODE_EXCLUSIVE,
      /* queueFamilyIndexCount */ 0,
      /* pQueueFamilyIndices   */ nullptr};

  VK_RETURN_FAIL(
      _vk->createBuffer(_vk->device, &create_info, nullptr, &_buffer));
  _range = size;

  VkMemoryRequirements memory_requirements;
  _vk->getBufferMemoryRequirements(_vk->device, _buffer, &memory_requirements);

  switch (ruse) {
    case EResourceUse::GPU:
    case EResourceUse::TransientGPU:
      memory_flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
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

  VK_GOTO_FAIL(
      res = _vk.AllocateMemory(memory_requirements, memory_flags, _memory));

  VK_GOTO_FAIL(res = _vk->bindBufferMemory(_vk->device, _buffer,
                                           _memory.Memory(), _memory.Offset()));

  if (data != nullptr) {
    auto &uploader = _vk.GetUploader();

    UploadBuffer upload;

    VK_GOTO_FAIL(res =
                     uploader.Start(_buffer, memory_requirements.size, upload));

    uploader.Copy(upload, 0, size, data);

    VK_GOTO_FAIL(uploader.Finish(upload));
  }

  if (false) {
  fail:
    if (_buffer != VK_NULL_HANDLE)
      _vk->destroyBuffer(_vk->device, _buffer, nullptr);
    _vk.Free(_memory);
    return res;
  }

  return Result::kSuccess;
}

void Buffer::Shutdown() {
  _vk.Destroy(_buffer);
  _vk.Free(_memory);
  _buffer = VK_NULL_HANDLE;
}

void *Buffer::MapVoid() { return _memory.Map(); }

void Buffer::Unmap() {
  // nothing
}

}  // namespace vulkan
}  // namespace ancer
