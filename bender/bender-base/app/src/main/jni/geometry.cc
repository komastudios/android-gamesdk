//
// Created by mattkw on 10/25/2019.
//

#include "geometry.h"
#include "polyhedron.h"
#include <vector>

#include "bender_helpers.h"

using namespace benderhelpers;

benderkit::Device *Geometry::device_ = nullptr;
VkBuffer Geometry::vertex_buf_ = VK_NULL_HANDLE;
VkDeviceMemory Geometry::vertex_buffer_device_memory_ = VK_NULL_HANDLE;
VkBuffer Geometry::index_buf_ = VK_NULL_HANDLE;
VkDeviceMemory Geometry::index_buffer_device_memory_ = VK_NULL_HANDLE;

Geometry::Geometry(benderkit::Device *device,
                   int index_count,
                   int vertex_offset,
                   int index_offset,
                   BoundingBox &box,
                   std::function<void(std::vector<float> &, std::vector<uint16_t> &)> generator)
    : vertex_offset_(vertex_offset),
      index_count_(index_count),
      index_offset_(index_offset),
      bounding_box_(box),
      generator_(generator) {
  Geometry::device_ = device;
}

Geometry::~Geometry() {
  Cleanup();
}

void Geometry::Cleanup() {
  vkDeviceWaitIdle(Geometry::device_->GetDevice());
  vkDestroyBuffer(Geometry::device_->GetDevice(), Geometry::vertex_buf_, nullptr);
  vkFreeMemory(Geometry::device_->GetDevice(), Geometry::vertex_buffer_device_memory_, nullptr);
  vkDestroyBuffer(Geometry::device_->GetDevice(), Geometry::index_buf_, nullptr);
  vkFreeMemory(Geometry::device_->GetDevice(), Geometry::index_buffer_device_memory_, nullptr);
}

void Geometry::OnResume(benderkit::Device *device, const std::vector<float> &vertex_data, const std::vector<uint16_t> &index_data) {
  Geometry::device_ = device;
  if (generator_ != nullptr){
    std::vector<float> generated_vertices;
    std::vector<uint16_t> generated_indices;
    generator_(generated_vertices, generated_indices);
    CreateVertexBuffer(generated_vertices, generated_indices);
  }
  else {
    CreateVertexBuffer(vertex_data, index_data);
  }
}

void Geometry::CreateVertexBuffer(const std::vector<float>& vertex_data, const std::vector<uint16_t>& index_data) {

  VkDeviceSize buffer_size_vertex = sizeof(vertex_data[0]) * vertex_data.size();
  Geometry::device_->CreateBuffer(buffer_size_vertex, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
               vertex_buf_, vertex_buffer_device_memory_);

  VkDeviceSize buffer_size_index = sizeof(index_data[0]) * index_data.size();
  Geometry::device_->CreateBuffer(buffer_size_index, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
               Geometry::index_buf_, index_buffer_device_memory_);

  void *data;
  vkMapMemory(Geometry::device_->GetDevice(), vertex_buffer_device_memory_, 0, buffer_size_vertex, 0, &data);
  memcpy(data, vertex_data.data(), buffer_size_vertex);
  vkUnmapMemory(Geometry::device_->GetDevice(), vertex_buffer_device_memory_);

  vkMapMemory(Geometry::device_->GetDevice(), index_buffer_device_memory_, 0, buffer_size_index, 0, &data);
  memcpy(data, index_data.data(), buffer_size_index);
  vkUnmapMemory(Geometry::device_->GetDevice(), index_buffer_device_memory_);
}

void Geometry::Bind(VkCommandBuffer cmd_buffer)  {
  VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &vertex_buf_, &offset);
  vkCmdBindIndexBuffer(cmd_buffer, Geometry::index_buf_, offset, VK_INDEX_TYPE_UINT16);
}