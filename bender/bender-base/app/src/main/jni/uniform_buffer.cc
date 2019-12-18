//
// Created by mattkw on 10/25/2019.
//

#include "uniform_buffer.h"

UniformBuffer::UniformBuffer(benderkit::Device& device, size_t buffer_size) :
        device_(device) {
  buffers_.resize(device_.GetSwapchainLength());
  buffer_memory_.resize(device_.GetSwapchainLength());

  for (size_t i = 0; i < device_.GetSwapchainLength(); i++) {
    device_.CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         buffers_[i], buffer_memory_[i]);
  }
}

UniformBuffer::~UniformBuffer() {
  for (int i = 0; i < device_.GetSwapchainLength(); i++) {
    vkDestroyBuffer(device_.GetDevice(), buffers_[i], nullptr);
    vkFreeMemory(device_.GetDevice(), buffer_memory_[i], nullptr);
  }
}

void* UniformBuffer::Map(int frame_index, size_t offset, size_t size) {
  void* data;
  vkMapMemory(device_.GetDevice(), buffer_memory_[frame_index], offset, size, 0, &data);
  return data;
}

void UniformBuffer::Unmap(int frame_index) {
  vkUnmapMemory(device_.GetDevice(), buffer_memory_[frame_index]);
}

