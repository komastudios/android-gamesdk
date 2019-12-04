//
// Created by mattkw on 10/25/2019.
//

#include "geometry.h"
#include "polyhedron.h"
#include <vector>

#include "bender_helpers.h"

using namespace BenderHelpers;

Geometry::Geometry(BenderKit::Device &device,
                   const std::vector<float> &vertexData,
                   const std::vector<uint32_t> &indexData,
                   std::function<void(std::vector<float>&, std::vector<uint32_t>&)> generator)
    : device_(device), generator_(generator) {
  createVertexBuffer(vertexData, indexData);
}

Geometry::~Geometry() {
  cleanup();
}

void Geometry::cleanup() {
  vkDeviceWaitIdle(device_.getDevice());
  vkDestroyBuffer(device_.getDevice(), vertexBuf_, nullptr);
  vkFreeMemory(device_.getDevice(), vertexBufferDeviceMemory_, nullptr);
  vkDestroyBuffer(device_.getDevice(), indexBuf_, nullptr);
  vkFreeMemory(device_.getDevice(), indexBufferDeviceMemory_, nullptr);
}

void Geometry::onResume(BenderKit::Device &device) {
  device_ = device;
  if (generator_ != nullptr){
    std::vector<float> vertex_data;
    std::vector<uint32_t> index_data;
    generator_(vertex_data, index_data);
    createVertexBuffer(vertex_data, index_data);
  }
}

void Geometry::createVertexBuffer(const std::vector<float>& vertexData, const std::vector<uint32_t>& indexData) {
  vertexCount_ = vertexData.size();
  indexCount_ = indexData.size();

  VkDeviceSize bufferSizeVertex = sizeof(vertexData[0]) * vertexData.size();
  device_.createBuffer(bufferSizeVertex, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
               vertexBuf_, vertexBufferDeviceMemory_);

  VkDeviceSize bufferSizeIndex = sizeof(indexData[0]) * indexData.size();
  device_.createBuffer(bufferSizeIndex, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
               indexBuf_, indexBufferDeviceMemory_);

  void *data;
  vkMapMemory(device_.getDevice(), vertexBufferDeviceMemory_, 0, bufferSizeVertex, 0, &data);
  memcpy(data, vertexData.data(), bufferSizeVertex);
  vkUnmapMemory(device_.getDevice(), vertexBufferDeviceMemory_);

  vkMapMemory(device_.getDevice(), indexBufferDeviceMemory_, 0, bufferSizeIndex, 0, &data);
  memcpy(data, indexData.data(), bufferSizeIndex);
  vkUnmapMemory(device_.getDevice(), indexBufferDeviceMemory_);
}

void Geometry::bind(VkCommandBuffer commandBuffer) const {
  VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuf_, &offset);
  vkCmdBindIndexBuffer(commandBuffer, indexBuf_, offset, VK_INDEX_TYPE_UINT32);
}