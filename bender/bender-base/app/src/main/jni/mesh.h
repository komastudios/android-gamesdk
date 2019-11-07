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
#import "material.h"
#import "shader_state.h"
#import "renderer.h"
#import "uniform_buffer.h"

struct ModelViewProjection {
    alignas(16) glm::mat4 mvp;
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 invTranspose;
};

class Mesh {
public:
  Mesh(android_app *app, BenderKit::Device *device, Renderer *renderer,
          const std::vector<float>& vertexData, const std::vector<uint16_t>& indexData,
          std::shared_ptr<ShaderState> shaders);

  ~Mesh();

  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptor_set_layout_; }

  void updatePipeline(VkRenderPass renderPass);
  void submitDraw() const;

  void translate(glm::vec3 offset);
  void rotate(glm::vec3 axis, float angle);
  void scale(glm::vec3 scaling);

  void setPosition(glm::vec3 position);
  void setRotation(glm::vec3 axis, float angle);
  void setScale(glm::vec3 scale);

  glm::vec3 getPosition() const;
  glm::quat getRotation() const;
  glm::vec3 getScale() const;

  void updateMvpBuffer(glm::vec3 camera);

private:
  BenderKit::Device *device_;
  Renderer *renderer_;
  std::shared_ptr<Geometry> geometry_;
  std::shared_ptr<Material> material_;
  std::shared_ptr<ShaderState> shaders_;

  UniformBufferObject<ModelViewProjection> *mvp_buffer_;

  glm::vec3 position_;
  glm::quat rotation_;
  glm::vec3 scale_;

  VkDescriptorSetLayout descriptor_set_layout_;
  std::vector<VkDescriptorSet> descriptor_sets_;
  VkPipelineLayout layout_;
  VkPipelineCache cache_;
  VkPipeline pipeline_ = VK_NULL_HANDLE;

  void createDescriptorSetLayout();
  void createDescriptorSets();
  void createMeshPipeline(VkRenderPass renderPass);
};

#endif //BENDER_BASE_MESH_H
