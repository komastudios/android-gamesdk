//
// Created by mattkw on 10/25/2019.
//

#pragma once

#ifndef BENDER_BASE_UNIFORM_BUFFER_H
#define BENDER_BASE_UNIFORM_BUFFER_H

#include "vulkan_wrapper.h"
#include "bender_kit.h"

#include <vector>

class UniformBuffer {
public:
  UniformBuffer(BenderKit::Device& device, size_t bufferSize);
  ~UniformBuffer();

  void* map(int frameIndex, size_t offset, size_t size);
  void unmap(int frameIndex);

  VkBuffer getBuffer(int frameIndex) const { return buffers_[frameIndex]; }

private:
  BenderKit::Device& device_;

  std::vector<VkBuffer> buffers_;
  std::vector<VkDeviceMemory> bufferMemory_;
};

template <typename T> class UniformBufferObject : private UniformBuffer {
public:
  UniformBufferObject(BenderKit::Device& device) : UniformBuffer(device, sizeof(T)) { }

  void update(int frameIndex, std::function<void(T& data)> updater) {
    void *data = UniformBuffer::map(frameIndex, 0, sizeof(T));
    updater(*reinterpret_cast<T*>(data));
    unmap(frameIndex);
  }

  T& map(int frameIndex) {
    return *reinterpret_cast<T*>(UniformBuffer::map(frameIndex, 0, sizeof(T)));
  }

  VkBuffer getBuffer(int frameIndex) const { return UniformBuffer::getBuffer(frameIndex); }
};

#endif //BENDER_BASE_UNIFORM_BUFFER_H
