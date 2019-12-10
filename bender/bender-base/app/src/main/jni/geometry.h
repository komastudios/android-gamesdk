//
// Created by mattkw on 10/25/2019.
//

#ifndef BENDER_BASE_GEOMETRY_H_
#define BENDER_BASE_GEOMETRY_H_

#include "vulkan_wrapper.h"
#include "bender_kit.h"

class Geometry {
 public:
  Geometry(BenderKit::Device &device,
           const std::vector<float> &vertexData,
           const std::vector<uint16_t> &indexData,
           std::function<void(std::vector<float> &, std::vector<uint16_t> &)> generator = nullptr);
  ~Geometry();

  void cleanup();

  void onResume(BenderKit::Device &device);

  int getVertexCount() const { return vertexCount_; }
  int getIndexCount() const { return indexCount_; }

  void bind(VkCommandBuffer commandBuffer) const;

 private:
  BenderKit::Device &device_;

  int vertexCount_;
  VkBuffer vertexBuf_;
  VkDeviceMemory vertexBufferDeviceMemory_;

  int indexCount_;
  VkBuffer indexBuf_;
  VkDeviceMemory indexBufferDeviceMemory_;

  std::function<void(std::vector<float> &, std::vector<uint16_t> &)> generator_ = nullptr;

  void createVertexBuffer(const std::vector<float> &vertexData,
                          const std::vector<uint16_t> &indexData);
};

#endif //BENDER_BASE_GEOMETRY_H
