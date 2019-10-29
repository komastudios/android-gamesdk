//
// Created by mattkw on 10/25/2019.
//

#ifndef BENDER_BASE_GEOMETRY_H_
#define BENDER_BASE_GEOMETRY_H_

#include "vulkan_wrapper.h"
#include "bender_kit.h"

class Geometry {
 public:
  Geometry(BenderKit::Device *device,
           std::vector<float> vertexData,
           std::vector<uint16_t> indexData);
  ~Geometry();

  int getVertexCount() const { return vertexCount_; }
  int getIndexCount() const { return indexCount_; }

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                      VkBuffer &buffer, VkDeviceMemory &bufferMemory);

  void bind(VkCommandBuffer commandBuffer) const;

 private:
  BenderKit::Device *device_;

  int vertexCount_;
  VkBuffer vertexBuf_;
  VkDeviceMemory vertexBufferDeviceMemory_;

  int indexCount_;
  VkBuffer indexBuf_;
  VkDeviceMemory indexBufferDeviceMemory_;

  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties,
                          VkPhysicalDevice gpuDevice) const;

  void createVertexBuffer(std::vector<float> vertexData, std::vector<uint16_t> indexData);
};

#endif //BENDER_BASE_GEOMETRY_H
