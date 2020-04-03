//
// Created by mattkw on 10/25/2019.
//

#include "geometry.h"
#include "polyhedron.h"
#include <vector>
#include <packed_types.h>

#include "bender_helpers.h"

using namespace benderhelpers;

VkBuffer Geometry::vertex_buf_ = VK_NULL_HANDLE;
VkDeviceMemory Geometry::vertex_buffer_device_memory_ = VK_NULL_HANDLE;
VkBuffer Geometry::index_buf_ = VK_NULL_HANDLE;
VkDeviceMemory Geometry::index_buffer_device_memory_ = VK_NULL_HANDLE;

Geometry::Geometry(int vertex_count,
                   int index_count,
                   int vertex_offset,
                   int index_offset,
                   BoundingBox &bounding_box,
                   glm::vec3 &scale_factor)
        : vertex_count_(vertex_count), index_count_(index_count),
          vertex_offset_(vertex_offset), index_offset_(index_offset),
          bounding_box_(bounding_box), scale_factor_(scale_factor) {}

void Geometry::CleanupStatic(benderkit::Device &device) {
  vkDeviceWaitIdle(device.GetDevice());
  vkDestroyBuffer(device.GetDevice(), vertex_buf_, nullptr);
  vkFreeMemory(device.GetDevice(), vertex_buffer_device_memory_, nullptr);
  vkDestroyBuffer(device.GetDevice(), index_buf_, nullptr);
  vkFreeMemory(device.GetDevice(), index_buffer_device_memory_, nullptr);
}

packed_vertex *Geometry::LockVertexBuffer(benderkit::Device &device, int vertex_count) {
  VkDeviceSize buffer_size = sizeof(packed_vertex) * vertex_count;
  device.CreateBuffer(buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      vertex_buf_, vertex_buffer_device_memory_);

  void *data;
  vkMapMemory(device.GetDevice(), vertex_buffer_device_memory_, 0, buffer_size, 0, &data);
  return static_cast<packed_vertex*>(data);
}

uint16_t *Geometry::LockIndexBuffer(benderkit::Device &device, int index_count) {
  VkDeviceSize buffer_size = sizeof(packed_vertex) * index_count;
  device.CreateBuffer(buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                      index_buf_, index_buffer_device_memory_);

  void *data;
  vkMapMemory(device.GetDevice(), index_buffer_device_memory_, 0, buffer_size, 0, &data);
  return static_cast<uint16_t *>(data);
}

void Geometry::UnlockVertexBuffer(benderkit::Device &device) {
  vkUnmapMemory(device.GetDevice(), vertex_buffer_device_memory_);
}

void Geometry::UnlockIndexBuffer(benderkit::Device &device) {
  vkUnmapMemory(device.GetDevice(), index_buffer_device_memory_);
}

void Geometry::Bind(VkCommandBuffer cmd_buffer) const {
  vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &vertex_buf_, &vertex_offset_);
  vkCmdBindIndexBuffer(cmd_buffer, index_buf_, index_offset_, VK_INDEX_TYPE_UINT16);
}