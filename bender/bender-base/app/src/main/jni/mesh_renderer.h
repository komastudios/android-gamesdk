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

class MeshRenderer {
public:
  MeshRenderer(Renderer *renderer, std::shared_ptr<Material> material, std::shared_ptr<Geometry> geometry_data);

  MeshRenderer(Renderer *renderer, std::shared_ptr<Material> material, const std::vector<float>& vertex_data,
          const std::vector<uint16_t>& index_data);

  ~MeshRenderer();

  void Cleanup();

  void OnResume(Renderer *renderer);

  void UpdatePipeline(VkRenderPass render_pass, std::shared_ptr<Material> material_template);

  void SetupDraw(VkCommandBuffer cmd_buffer, uint32_t frame_index,
                 std::shared_ptr<Geometry> geometry) const;

  void SubmitDraw(VkCommandBuffer cmd_buffer, uint_t frame_index, uint32_t index_count,
                  std::vector<std::shared_ptr<Material>>& materials, std::vector<VkDescriptorSet>& mesh_descriptor_sets) const;

private:
  Renderer *renderer_;

  VkDescriptorSetLayout mesh_descriptors_layout_;

  VkPipelineLayout layout_;
  VkPipeline pipeline_ = VK_NULL_HANDLE;

  // std::vector<VkDescriptorSet> mesh_descriptor_sets_;

  void CreateMeshPipeline(VkRenderPass render_pass, std::shared_ptr<Material> material_template);

  void CreateMeshDescriptorSetLayout();
  // void CreateMeshDescriptors();
};

#endif //BENDER_BASE_MESH_H
