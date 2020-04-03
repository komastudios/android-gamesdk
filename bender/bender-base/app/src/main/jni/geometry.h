//
// Created by mattkw on 10/25/2019.
//

#ifndef BENDER_BASE_GEOMETRY_H_
#define BENDER_BASE_GEOMETRY_H_

#include "vulkan_wrapper.h"
#include "bender_kit.h"
#include <functional>
#include <glm/glm.hpp>
#include <packed_types.h>
#include <mesh_helpers.h>

struct BoundingBox {
    BoundingBox(glm::vec3 min, glm::vec3 max) :
            min(min), max(max) {
        center = (max + min) * .5f;
    }

    BoundingBox() :
            min({MAXFLOAT, MAXFLOAT, MAXFLOAT}), max({-MAXFLOAT, -MAXFLOAT, -MAXFLOAT}),
            center({0, 0, 0}) {}

    glm::vec3 min, max, center;
};

class Geometry {
 public:
  Geometry(int vertex_count,
           int index_count,
           int vertex_offset,
           int index_offset,
           BoundingBox &bounding_box,
           glm::vec3 &scale_factor);

  int GetVertexCount() const { return vertex_count_; }
  int GetIndexCount() const { return index_count_; }
  glm::vec3 GetScaleFactor() const { return scale_factor_; }
  BoundingBox GetBoundingBox() const { return bounding_box_; }

  void Bind(VkCommandBuffer cmd_buffer) const;

  static packed_vertex *LockVertexBuffer(benderkit::Device &device, int vertex_count);
  static void UnlockVertexBuffer(benderkit::Device &device);
  static uint16_t *LockIndexBuffer(benderkit::Device &device, int index_count);
  static void UnlockIndexBuffer(benderkit::Device &device);

  static void CleanupStatic(benderkit::Device &device);

 private:
  int vertex_count_;
  int index_count_;
  VkDeviceSize vertex_offset_;
  VkDeviceSize index_offset_;
  BoundingBox bounding_box_;
  glm::vec3 scale_factor_;

  static VkBuffer vertex_buf_;
  static VkDeviceMemory vertex_buffer_device_memory_;
  static VkBuffer index_buf_;
  static VkDeviceMemory index_buffer_device_memory_;
};

#endif //BENDER_BASE_GEOMETRY_H
