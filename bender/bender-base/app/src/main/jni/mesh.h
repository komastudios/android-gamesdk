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
#import "texture.h"
#import "renderer.h"

#import "uniform_buffer.h"

struct ModelViewProjection {
  alignas(16) glm::mat4 mvp;
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 invTranspose;
};

class Mesh {
public:
  static void createPools(BenderKit::Device& device);
  static void destroyPools(BenderKit::Device& device);

  static VkDescriptorPool getMeshDescriptorPool() { return mesh_descriptor_pool_; };
  static VkDescriptorPool getMaterialDescriptorPool() { return material_descriptor_pool_; };

  Mesh(Renderer *renderer, const std::vector<float>& vertexData,
          const std::vector<uint16_t>& indexData, std::shared_ptr<ShaderState> shaders);

  ~Mesh();

  // TODO: Move texture to the Material class
  void createDescriptors(Texture* texture);

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
  static VkDescriptorPool mesh_descriptor_pool_;
  static VkDescriptorPool material_descriptor_pool_;

  UniformBufferObject<ModelViewProjection> *meshBuffer;

  BenderKit::Device &device_;
  Renderer *renderer_;

  std::shared_ptr<Geometry> geometry_;
  std::shared_ptr<ShaderState> shaders_;

  glm::vec3 position_;
  glm::quat rotation_;
  glm::vec3 scale_;

  VkDescriptorSetLayout material_descriptors_layout_;
  VkDescriptorSetLayout mesh_descriptors_layout_;

  VkPipelineLayout layout_;
  VkPipelineCache cache_;
  VkPipeline pipeline_ = VK_NULL_HANDLE;

  std::vector<VkDescriptorSet> material_descriptor_sets_;
  std::vector<VkDescriptorSet> model_view_projection_descriptor_sets_;

  void createDescriptorSetLayout();
  void createMeshPipeline(VkRenderPass renderPass);
};

#endif //BENDER_BASE_MESH_H
