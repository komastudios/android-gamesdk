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

class Mesh {
public:
  Mesh(Renderer *renderer, std::shared_ptr<Geometry> geometry_data);

  Mesh(Renderer *renderer, const std::vector<float>& vertex_data,
          const std::vector<uint16_t>& index_data);

  Mesh(const Mesh &other, std::shared_ptr<Geometry> geometry);

  ~Mesh();

  void Cleanup();

  void OnResume(Renderer *renderer);

  void UpdatePipeline(VkRenderPass render_pass, std::shared_ptr<Material> material);

  void SetupDraw(VkCommandBuffer cmd_buffer, uint_t frame_index);

  void SubmitDraw(VkDescriptorSet mesh_descriptor_set, VkDescriptorSet material_descriptor_set) const;

  VkDescriptorSetLayout GetMeshDescriptorSetLayout() { return mesh_descriptors_layout_; }

  BoundingBox GetBoundingBox() const { return geometry_->GetBoundingBox(); }

  int GetTrianglesCount() const { return geometry_->GetIndexCount() / 3; }

private:
  Renderer *renderer_;
  const std::shared_ptr<Geometry> geometry_;

  VkDescriptorSetLayout mesh_descriptors_layout_;

  VkPipelineLayout layout_;
  VkPipeline pipeline_ = VK_NULL_HANDLE;

  VkCommandBuffer cmd_buffer_ = VK_NULL_HANDLE;

  void CreateMeshPipeline(VkRenderPass render_pass, std::shared_ptr<Material> material);

  void CreateMeshDescriptorSetLayout();
};

#endif //BENDER_BASE_MESH_H
