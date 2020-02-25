//
// Created by mattkw on 10/25/2019.
//

#ifndef BENDER_BASE_GEOMETRY_H_
#define BENDER_BASE_GEOMETRY_H_

#include "vulkan_wrapper.h"
#include "bender_kit.h"
#include <functional>
#include <glm/glm.hpp>

struct BoundingBox {
  glm::vec3 min {MAXFLOAT, MAXFLOAT, MAXFLOAT};
  glm::vec3 max {-MAXFLOAT, -MAXFLOAT, -MAXFLOAT};
  glm::vec3 center {0, 0, 0};
};

class Geometry {
 public:
  Geometry(benderkit::Device *device,
           int index_count,
           int vertex_offset,
           int index_offset,
           BoundingBox &box,
           std::function<void(std::vector<float> &, std::vector<uint16_t> &)> generator = nullptr
  );
  ~Geometry();

  void Cleanup();

  void OnResume(benderkit::Device *device, const std::vector<float> &vertex_data = {}, const std::vector<uint16_t> &index_data = {});

  int GetVertexCount() const { return vertex_count_; }
  int GetIndexCount() const { return index_count_; }
  int GetFirstIndex() const { return index_offset_; }
  int GetVertexOffset() const { return vertex_offset_; }
  BoundingBox GetBoundingBox() const { return bounding_box_; }

  static void Bind(VkCommandBuffer cmd_buffer);
  static void CreateVertexBuffer(const std::vector<float> &vertex_data,
                                 const std::vector<uint16_t> &index_data);

 private:
  int vertex_count_;
  int vertex_offset_;
  int index_count_;
  int index_offset_;
  BoundingBox bounding_box_;

  static benderkit::Device *device_;
  static VkBuffer vertex_buf_;
  static VkDeviceMemory vertex_buffer_device_memory_;
  static VkBuffer index_buf_;
  static VkDeviceMemory index_buffer_device_memory_;


  std::function<void(std::vector<float> &, std::vector<uint16_t> &)> generator_ = nullptr;
};

#endif //BENDER_BASE_GEOMETRY_H
