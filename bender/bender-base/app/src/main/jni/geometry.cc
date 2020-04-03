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

void Geometry::CreateVertexBuffer(benderkit::Device &device, const std::vector<packed_vertex>& vertex_data, const std::vector<uint16_t>& index_data) {
  VkDeviceSize buffer_size_vertex = sizeof(vertex_data[0]) * vertex_data.size();
  device.CreateBuffer(buffer_size_vertex, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
               vertex_buf_, vertex_buffer_device_memory_);

  VkDeviceSize buffer_size_index = sizeof(index_data[0]) * index_data.size();
  device.CreateBuffer(buffer_size_index, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
               index_buf_, index_buffer_device_memory_);

  void *data;
  vkMapMemory(device.GetDevice(), vertex_buffer_device_memory_, 0, buffer_size_vertex, 0, &data);
  memcpy(data, vertex_data.data(), buffer_size_vertex);
  vkUnmapMemory(device.GetDevice(), vertex_buffer_device_memory_);

  vkMapMemory(device.GetDevice(), index_buffer_device_memory_, 0, buffer_size_index, 0, &data);
  memcpy(data, index_data.data(), buffer_size_index);
  vkUnmapMemory(device.GetDevice(), index_buffer_device_memory_);
}

void Geometry::Bind(VkCommandBuffer cmd_buffer) const {
  vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &vertex_buf_, &vertex_offset_);
  vkCmdBindIndexBuffer(cmd_buffer, index_buf_, index_offset_, VK_INDEX_TYPE_UINT16);
}