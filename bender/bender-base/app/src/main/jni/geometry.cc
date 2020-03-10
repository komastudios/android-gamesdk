//
// Created by mattkw on 10/25/2019.
//

#include "geometry.h"
#include "polyhedron.h"
#include <vector>
#include <math.h>
#include <glm/gtc/random.hpp>

#include "bender_helpers.h"

using namespace benderhelpers;

Geometry::Geometry(benderkit::Device &device,
                   const std::vector<float> &vertex_data,
                   const std::vector<uint16_t> &index_data)
    : device_(device) {
  for (int x = 0; x < vertex_data.size() / 14; x++){
    float x_coord = vertex_data[x * 14];
    float y_coord = vertex_data[x * 14 + 1];
    float z_coord = vertex_data[x * 14 + 2];

    if (x_coord > bounding_box_.max.x) bounding_box_.max.x = x_coord;
    if (x_coord < bounding_box_.min.x) bounding_box_.min.x = x_coord;
    if (y_coord > bounding_box_.max.y) bounding_box_.max.y = y_coord;
    if (y_coord < bounding_box_.min.y) bounding_box_.min.y = y_coord;
    if (z_coord > bounding_box_.max.z) bounding_box_.max.z = z_coord;
    if (z_coord < bounding_box_.min.z) bounding_box_.min.z = z_coord;
  }
  bounding_box_.center = (bounding_box_.max + bounding_box_.min) * .5f;

  std::vector<uint16_t> real_vertex_data(vertex_data.size() / 14 * 10);
  float max_x = std::max(std::abs(bounding_box_.min.x), std::abs(bounding_box_.max.x));
  float max_y = std::max(std::abs(bounding_box_.min.y), std::abs(bounding_box_.max.y));
  float max_z = std::max(std::abs(bounding_box_.min.z), std::abs(bounding_box_.max.z));
  scale_factor_ = {max_x, max_y, max_z};

  for (int x = 0; x < vertex_data.size() / 14; x++){
    float x_coord = vertex_data[x * 14];
    float y_coord = vertex_data[x * 14 + 1];
    float z_coord = vertex_data[x * 14 + 2];

    real_vertex_data[x * 10] = FloatToSnorm16(x_coord / max_x);
    real_vertex_data[x * 10 + 1] = FloatToSnorm16(y_coord / max_y);
    real_vertex_data[x * 10 + 2] = FloatToSnorm16(z_coord / max_z);
    real_vertex_data[x * 10 + 3] = SNORM_MAX;

    glm::vec3 normal = {vertex_data[x * 14 + 3], vertex_data[x * 14 + 4], vertex_data[x * 14 + 5]};
    glm::vec3 avg_tangent = {vertex_data[x * 14 + 6], vertex_data[x * 14 + 7], vertex_data[x * 14 + 8]};
    glm::vec3 avg_bitangent = {vertex_data[x * 14 + 9], vertex_data[x * 14 + 10], vertex_data[x * 14 + 11]};

    if (isnan(glm::dot(glm::normalize(avg_tangent), glm::normalize(avg_tangent)))) {
      avg_tangent = glm::vec3(1.0, 0.0, 0.0);
    }
    if (isnan(glm::dot(glm::normalize(avg_bitangent), glm::normalize(avg_bitangent)))) {
      avg_bitangent = glm::vec3(1.0, 0.0, 0.0);
    }

    normal = glm::normalize(normal);
    avg_tangent = glm::normalize(avg_tangent);
    glm::vec3 tangent = glm::normalize(avg_tangent - (normal * (glm::dot(normal, avg_tangent))));
    glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

    glm::mat3 tbn = {normal, tangent, bitangent};
    glm::quat q_tangent(tbn);
    q_tangent = glm::normalize(q_tangent);

    if( q_tangent.w < 0 )
      q_tangent = -q_tangent;

    const float bias = 1.0f / SNORM_MAX;
    const float norm_factor = std::sqrt(1 - bias * bias);

    if (q_tangent.w < bias){
      q_tangent.w = bias;
      q_tangent.x *= norm_factor;
      q_tangent.y *= norm_factor;
      q_tangent.z *= norm_factor;
    }

    if (glm::dot(bitangent, avg_bitangent) <= 0)
      q_tangent = -q_tangent;

    real_vertex_data[x * 10 + 4] = FloatToSnorm16(q_tangent.x);
    real_vertex_data[x * 10 + 5] = FloatToSnorm16(q_tangent.y);
    real_vertex_data[x * 10 + 6] = FloatToSnorm16(q_tangent.z);
    real_vertex_data[x * 10 + 7] = FloatToSnorm16(q_tangent.w);

    real_vertex_data[x * 10 + 8] = FloatToUnorm16(vertex_data[x * 14 + 12]);
    real_vertex_data[x * 10 + 9] = FloatToUnorm16(vertex_data[x * 14 + 13]);
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

void Geometry::CreateVertexBuffer(const std::vector<uint16_t>& vertex_data, const std::vector<uint16_t>& index_data) {
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