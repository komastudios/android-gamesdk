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
  alignas(16) glm::mat4 inv_transpose;
};

struct InstanceStuff {
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

//  std::unique_ptr<UniformBufferObject<ModelViewProjection>> mesh_buffer_;

//  const std::shared_ptr<Material> material_;

//  glm::vec3 position_;
//  glm::quat rotation_;
//  glm::vec3 scale_;

//  void Translate(glm::vec3 offset);
//  void Rotate(glm::vec3 axis, float angle);
//  void Scale(glm::vec3 scaling);
//
//  // TODO: consolidate camera, view, proj into a camera object
//  void Update(uint_t frame_index, glm::vec3 camera, glm::mat4 view, glm::mat4 proj, glm::mat4 prerotation);
//
//  glm::mat4 GetTransform() const;
//
//  BoundingBox GetBoundingBoxWorldSpace() const;
//
//  int GetTrianglesCount() const;
};

class MeshRenderer {
public:
  MeshRenderer(Renderer *renderer, std::shared_ptr<Material> material, std::shared_ptr<Geometry> geometry_data);

  MeshRenderer(Renderer *renderer, std::shared_ptr<Material> material, const std::vector<float>& vertex_data,
          const std::vector<uint16_t>& index_data);

  ~MeshRenderer();

  void Cleanup();

  void OnResume(Renderer *renderer);

  void UpdatePipeline(VkRenderPass render_pass, std::shared_ptr<Material> material_template);

  void SubmitDraw(VkCommandBuffer cmd_buffer, uint_t frame_index, std::shared_ptr<Geometry> geometry,
          std::vector<ModelViewProjection> instances) const;

private:

  Renderer *renderer_;

  VkDescriptorSetLayout mesh_descriptors_layout_;

  VkPipelineLayout layout_;
  VkPipeline pipeline_ = VK_NULL_HANDLE;

  std::vector<VkDescriptorSet> mesh_descriptor_sets_;

  void CreateMeshPipeline(VkRenderPass render_pass, std::shared_ptr<Material> material_template);

  void CreateMeshDescriptorSetLayout();
  void CreateMeshDescriptors();
};

#endif //BENDER_BASE_MESH_H
