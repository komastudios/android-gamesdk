//
// Created by mattkw on 10/25/2019.
//

#include "geometry.h"
#include "polyhedron.h"
#include <vector>
#include <packed_types.h>

#include "bender_helpers.h"

using namespace benderhelpers;

Geometry::Geometry(benderkit::Device &device,
                   const std::vector<MeshVertex> &vertex_data,
                   const std::vector<uint16_t> &index_data)
    : device_(device) {
  for (int x = 0; x < vertex_data.size(); x++){
    glm::vec3 pos = vertex_data[x].pos;

    if (pos.x > bounding_box_.max.x) bounding_box_.max.x = pos.x;
    if (pos.x < bounding_box_.min.x) bounding_box_.min.x = pos.x;
    if (pos.y > bounding_box_.max.y) bounding_box_.max.y = pos.y;
    if (pos.y < bounding_box_.min.y) bounding_box_.min.y = pos.y;
    if (pos.z > bounding_box_.max.z) bounding_box_.max.z = pos.z;
    if (pos.z < bounding_box_.min.z) bounding_box_.min.z = pos.z;
  }
  bounding_box_.center = (bounding_box_.max + bounding_box_.min) * .5f;

  std::vector<packed_vertex> real_vertex_data(vertex_data.size());
  float max_x = std::max(std::abs(bounding_box_.min.x), std::abs(bounding_box_.max.x));
  float max_y = std::max(std::abs(bounding_box_.min.y), std::abs(bounding_box_.max.y));
  float max_z = std::max(std::abs(bounding_box_.min.z), std::abs(bounding_box_.max.z));
  scale_factor_ = {max_x, max_y, max_z};

  for (int x = 0; x < vertex_data.size(); x++){
    real_vertex_data[x].position.Set(vertex_data[x].pos / scale_factor_, 1.0f);

    glm::vec3 normal = vertex_data[x].normal;
    glm::vec3 avg_tangent = vertex_data[x].tangent;
    glm::vec3 avg_bitangent = vertex_data[x].bitangent;

    normal = glm::normalize(normal);
    avg_tangent = glm::normalize(avg_tangent);
    glm::vec3 tangent = glm::normalize(avg_tangent - (normal * (glm::dot(normal, avg_tangent))));
    glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

    glm::mat3 tbn = {normal, tangent, bitangent};
    glm::quat q_tangent(tbn);
    if (glm::any(glm::isnan(q_tangent))){
        if (glm::any(glm::isnan(glm::normalize(normal)))){
            normal = {1.0, 0.0, 0.0};
        }
        tangent = glm::normalize(normal + glm::vec3(0.5f, 1.5f, 0.5f));
        tangent = glm::normalize(tangent - (normal * (glm::dot(normal, tangent))));
        bitangent = glm::cross(normal, tangent);
        tbn = {normal, tangent, bitangent};
        q_tangent = tbn;
    }
    q_tangent = glm::normalize(q_tangent);

    if( q_tangent.w < 0.0f )
      q_tangent = -q_tangent;

    const float bias = 1.0f / SNORM_MAX;
    const float norm_factor = std::sqrt(1 - bias * bias);

    if (q_tangent.w < bias){
      q_tangent.w = bias;
      q_tangent.x *= norm_factor;
      q_tangent.y *= norm_factor;
      q_tangent.z *= norm_factor;
    }

    if (glm::dot(bitangent, avg_bitangent) <= 0.0f)
      q_tangent = -q_tangent;

    real_vertex_data[x].q_tangent.Set(q_tangent);
    real_vertex_data[x].tex_coord.Set(vertex_data[x].tex_coord);
  }

  CreateVertexBuffer(real_vertex_data, index_data);
}

Geometry::~Geometry() {
  vkDeviceWaitIdle(device_.GetDevice());
  vkDestroyBuffer(device_.GetDevice(), vertex_buf_, nullptr);
  vkFreeMemory(device_.GetDevice(), vertex_buffer_device_memory_, nullptr);
  vkDestroyBuffer(device_.GetDevice(), index_buf_, nullptr);
  vkFreeMemory(device_.GetDevice(), index_buffer_device_memory_, nullptr);
}

void Geometry::CreateVertexBuffer(const std::vector<packed_vertex>& vertex_data, const std::vector<uint16_t>& index_data) {
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