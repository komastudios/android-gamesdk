//
// Created by mattkw on 10/25/2019.
//

#include "uniform_buffer.h"


UniformBuffer::UniformBuffer(benderkit::Device& device, size_t bufferSize) :
        device_(device) {
  buffers_.resize(device_.GetSwapchainLength());
  bufferMemory_.resize(device_.GetSwapchainLength());

  for (size_t i = 0; i < device_.GetSwapchainLength(); i++) {
    device_.CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         buffers_[i], bufferMemory_[i]);
  }
}

UniformBuffer::~UniformBuffer() {
  for (int i = 0; i < device_.GetSwapchainLength(); ++i) {
    vkDestroyBuffer(device_.GetDevice(), buffers_[i], nullptr);
    vkFreeMemory(device_.GetDevice(), bufferMemory_[i], nullptr);
  }
}

void* UniformBuffer::map(int frameIndex, size_t offset, size_t size) {
  void* data;
  vkMapMemory(device_.GetDevice(), bufferMemory_[frameIndex], offset, size, 0, &data);
  return data;
}

void UniformBuffer::unmap(int frameIndex) {
  vkUnmapMemory(device_.GetDevice(), bufferMemory_[frameIndex]);
}

