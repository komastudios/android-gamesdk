//
// Created by mattkw on 10/31/2019.
//

#ifndef BENDER_BASE_MESH_H
#define BENDER_BASE_MESH_H

#import <vector>
#import <memory>

#import "vulkan_wrapper.h"
#import "glm/glm.hpp"
#import "glm/gtc/quaternion.hpp"
#import "bender_kit.h"
#import "geometry.h"
#import "shader_state.h"

class Mesh {
public:
  Mesh(BenderKit::Device *device,
          const std::vector<float>& vertexData,
          const std::vector<uint16_t>& indexData,
          std::shared_ptr<ShaderState> shaders);

  ~Mesh();

  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptor_set_layout_; }

  void updatePipeline(VkRenderPass renderPass);
  void submitDraw(VkCommandBuffer commandBuffer, VkDescriptorSet& descriptorSet) const;

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
  BenderKit::Device *device_;

  std::shared_ptr<Geometry> geometry_;
  std::shared_ptr<ShaderState> shaders_;

  glm::vec3 position_;
  glm::quat rotation_;
  glm::vec3 scale_;

  VkDescriptorSetLayout descriptor_set_layout_;
  VkPipelineLayout layout_;
  VkPipelineCache cache_;
  VkPipeline pipeline_ = VK_NULL_HANDLE;

  void createDescriptorSetLayout();
  void createMeshPipeline(VkRenderPass renderPass);
};

#endif //BENDER_BASE_MESH_H
