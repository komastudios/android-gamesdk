//
// Created by mattkw on 10/31/2019.
//

#ifndef BENDER_BASE_MESH_H
#define BENDER_BASE_MESH_H

#include <vector>
#include <memory>

#include "vulkan_wrapper.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "bender_kit.h"
#include "geometry.h"
#include "shader_state.h"
#include "texture.h"
#include "renderer.h"
#include "material.h"

#include "uniform_buffer.h"

struct ModelViewProjection {
  alignas(16) glm::mat4 mvp;
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 invTranspose;
};

class Mesh {
public:
  Mesh(Renderer& renderer, Material& material, std::shared_ptr<Geometry> geometryData);

  Mesh(Renderer& renderer, Material& material, const std::vector<float>& vertexData,
          const std::vector<uint16_t>& indexData);

  ~Mesh();

  void updatePipeline(VkRenderPass renderPass);

  // TODO: consolidate camera, view, proj into a camera object
  void update(uint_t frame_index, glm::vec3 camera, glm::mat4 view, glm::mat4 proj);
  void submitDraw(VkCommandBuffer commandBuffer, uint_t frame_index) const;

  void translate(glm::vec3 offset);
  void rotate(glm::vec3 axis, float angle);
  void scale(glm::vec3 scaling);

  void setPosition(glm::vec3 position);
  void setRotation(glm::vec3 axis, float angle);
  void setScale(glm::vec3 scale);

  glm::vec3 getPosition() const;
  glm::quat getRotation() const;
  glm::vec3 getScale() const;

  glm::mat4 getTransform() const;

private:
  std::unique_ptr<UniformBufferObject<ModelViewProjection>> mesh_buffer_;

  Renderer& renderer_;
  Material& material_;

  std::shared_ptr<Geometry> geometry_;

  glm::vec3 position_;
  glm::quat rotation_;
  glm::vec3 scale_;

  VkDescriptorSetLayout mesh_descriptors_layout_;

  VkPipelineLayout layout_;
  VkPipelineCache cache_;
  VkPipeline pipeline_ = VK_NULL_HANDLE;

  std::vector<VkDescriptorSet> mesh_descriptor_sets_;

  void createMeshPipeline(VkRenderPass renderPass);

  void createMeshDescriptorSetLayout();
  void createMeshDescriptors();
};

#endif //BENDER_BASE_MESH_H
