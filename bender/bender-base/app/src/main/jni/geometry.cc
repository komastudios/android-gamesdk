//
// Created by mattkw on 10/25/2019.
//

#include "geometry.h"
#include "polyhedron.h"
#include <vector>

#include "bender_helpers.h"

using namespace benderhelpers;

Geometry::Geometry(benderkit::Device &device,
                   const std::vector<float> &vertex_data,
                   const std::vector<uint16_t> &index_data,
                   std::function<void(std::vector<float>&, std::vector<uint16_t>&)> generator)
    : device_(device), generator_(generator) {
  CreateVertexBuffer(vertex_data, index_data);

  for (int x = 0; x < vertex_data.size() / 8; x++){
    float xCoord = vertex_data[x * 8];
    float yCoord = vertex_data[x * 8 + 1];
    float zCoord = vertex_data[x * 8 + 2];

    if (xCoord > bounding_box_.max.x) bounding_box_.max.x = xCoord;
    if (xCoord < bounding_box_.min.x) bounding_box_.min.x = xCoord;
    if (yCoord > bounding_box_.max.y) bounding_box_.max.y = yCoord;
    if (yCoord < bounding_box_.min.y) bounding_box_.min.y = yCoord;
    if (zCoord > bounding_box_.max.z) bounding_box_.max.z = zCoord;
    if (zCoord < bounding_box_.min.z) bounding_box_.min.z = zCoord;
  }
}

Geometry::~Geometry() {
  Cleanup();
}

void Geometry::Cleanup() {
  vkDeviceWaitIdle(device_.GetDevice());
  vkDestroyBuffer(device_.GetDevice(), vertex_buf_, nullptr);
  vkFreeMemory(device_.GetDevice(), vertex_buffer_device_memory_, nullptr);
  vkDestroyBuffer(device_.GetDevice(), index_buf_, nullptr);
  vkFreeMemory(device_.GetDevice(), index_buffer_device_memory_, nullptr);
}

void Geometry::OnResume(benderkit::Device &device) {
  device_ = device;
  if (generator_ != nullptr){
    std::vector<float> vertex_data;
    std::vector<uint16_t> index_data;
    generator_(vertex_data, index_data);
    CreateVertexBuffer(vertex_data, index_data);
  }
}

void Geometry::CreateVertexBuffer(const std::vector<float>& vertex_data, const std::vector<uint16_t>& index_data) {
  vertex_count_ = vertex_data.size();
  index_count_ = index_data.size();

  VkDeviceSize buffer_size_vertex = sizeof(vertex_data[0]) * vertex_data.size();
  device_.CreateBuffer(buffer_size_vertex, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
               vertex_buf_, vertex_buffer_device_memory_);

  VkDeviceSize buffer_size_index = sizeof(index_data[0]) * index_data.size();
  device_.CreateBuffer(buffer_size_index, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
               index_buf_, index_buffer_device_memory_);

  void *data;
  vkMapMemory(device_.GetDevice(), vertex_buffer_device_memory_, 0, buffer_size_vertex, 0, &data);
  memcpy(data, vertex_data.data(), buffer_size_vertex);
  vkUnmapMemory(device_.GetDevice(), vertex_buffer_device_memory_);

  vkMapMemory(device_.GetDevice(), index_buffer_device_memory_, 0, buffer_size_index, 0, &data);
  memcpy(data, index_data.data(), buffer_size_index);
  vkUnmapMemory(device_.GetDevice(), index_buffer_device_memory_);
}

void Geometry::Bind(VkCommandBuffer cmd_buffer) const {
  VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &vertex_buf_, &offset);
  vkCmdBindIndexBuffer(cmd_buffer, index_buf_, offset, VK_INDEX_TYPE_UINT16);
}