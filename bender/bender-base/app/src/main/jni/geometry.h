//
// Created by mattkw on 10/25/2019.
//

#ifndef BENDER_BASE_GEOMETRY_H_
#define BENDER_BASE_GEOMETRY_H_

#include "vulkan_wrapper.h"
#include "bender_kit.h"
#include <functional>

class Geometry {
 public:
  Geometry(benderkit::Device &device,
           const std::vector<float> &vertex_data,
           const std::vector<uint16_t> &index_data,
           std::function<void(std::vector<float> &, std::vector<uint16_t> &)> generator = nullptr);
  ~Geometry();

  void Cleanup();

  void OnResume(benderkit::Device &device);

  int GetVertexCount() const { return vertex_count_; }
  int GetIndexCount() const { return index_count_; }

  void Bind(VkCommandBuffer cmd_buffer) const;

 private:
  benderkit::Device &device_;

  int vertex_count_;
  VkBuffer vertex_buf_;
  VkDeviceMemory vertex_buffer_device_memory_;

  int index_count_;
  VkBuffer index_buf_;
  VkDeviceMemory index_buffer_device_memory_;

  std::function<void(std::vector<float> &, std::vector<uint16_t> &)> generator_ = nullptr;

  void CreateVertexBuffer(const std::vector<float> &vertex_data,
                          const std::vector<uint16_t> &index_data);
};

#endif //BENDER_BASE_GEOMETRY_H
