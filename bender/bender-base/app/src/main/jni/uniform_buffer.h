//
// Created by mattkw on 10/25/2019.
//

#pragma once

#ifndef BENDER_BASE_UNIFORM_BUFFER_H
#define BENDER_BASE_UNIFORM_BUFFER_H

#include "vulkan_wrapper.h"
#include "bender_kit.h"
#include "functional"

#include <vector>

class UniformBuffer {
public:
  UniformBuffer(benderkit::Device& device, size_t buffer_size);
  ~UniformBuffer();

  void* Map(int frame_index, size_t offset, size_t size);
  void Unmap(int frame_index);

  VkBuffer GetBuffer(int frame_index) const { return buffers_[frame_index]; }

private:
  benderkit::Device& device_;

  std::vector<VkBuffer> buffers_;
  std::vector<VkDeviceMemory> buffer_memory_;
};

template <typename T> class UniformBufferObject : private UniformBuffer {
public:
  UniformBufferObject(benderkit::Device& device) : UniformBuffer(device, sizeof(T)) { }

  void Update(int frame_index, std::function<void(T& data)> updater) {
    void *data = UniformBuffer::Map(frame_index, 0, sizeof(T));
    updater(*reinterpret_cast<T*>(data));
    Unmap(frame_index);
  }

  T& Map(int frame_index) {
    return *reinterpret_cast<T*>(UniformBuffer::Map(frame_index, 0, sizeof(T)));
  }

  VkBuffer GetBuffer(int frame_index) const { return UniformBuffer::GetBuffer(frame_index); }
};

#endif //BENDER_BASE_UNIFORM_BUFFER_H
