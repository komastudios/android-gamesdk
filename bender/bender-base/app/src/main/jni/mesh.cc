//
// Created by mattkw on 10/31/2019.
//

#include "mesh.h"
#include "material.h"
#include "shader_bindings.h"

#include <glm/gtc/matrix_transform.hpp>


Mesh::Mesh(Renderer *renderer,
           std::shared_ptr<Geometry> geometry) :
    renderer_(renderer), geometry_(geometry) {}

Mesh::Mesh(Renderer *renderer,
           const std::vector<float> &vertex_data,
           const std::vector<uint16_t> &index_data) :
    Mesh(renderer,
         std::make_shared<Geometry>(renderer->GetDevice(), vertex_data, index_data)) {}

Mesh::Mesh(const Mesh &other, std::shared_ptr<Geometry> geometry) :
    renderer_(other.renderer_),
    geometry_(geometry) {}

Mesh::~Mesh() {
  Cleanup();
}

void Mesh::Cleanup() {
  vkDeviceWaitIdle(renderer_->GetVulkanDevice());
  vkDestroyPipeline(renderer_->GetVulkanDevice(), pipeline_, nullptr);
  vkDestroyPipelineLayout(renderer_->GetVulkanDevice(), layout_, nullptr);
  pipeline_ = VK_NULL_HANDLE;
}

void Mesh::OnResume(Renderer *renderer) {
  renderer_ = renderer;
}

void Mesh::CreateMeshPipeline(VkRenderPass render_pass, std::shared_ptr<Material> material) {
  std::array<VkDescriptorSetLayout, 3> layouts;

  layouts[BINDING_SET_MESH] = mesh_descriptors_layout_;
  layouts[BINDING_SET_MATERIAL] = material->GetMaterialDescriptorSetLayout();
  layouts[BINDING_SET_LIGHTS] = renderer_->GetLightsDescriptorSetLayout();

  VkPipelineLayoutCreateInfo pipeline_layout_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .setLayoutCount = layouts.size(),
      .pSetLayouts = layouts.data(),
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = nullptr,
  };

  CALL_VK(vkCreatePipelineLayout(renderer_->GetVulkanDevice(), &pipeline_layout_info, nullptr,
                                 &layout_))

  VkGraphicsPipelineCreateInfo pipeline_info = renderer_->GetDefaultPipelineInfo(
      layout_,
      render_pass);

  material->FillPipelineInfo(&pipeline_info);

  CALL_VK(vkCreateGraphicsPipelines(
      renderer_->GetVulkanDevice(),
      renderer_->GetPipelineCache(), 1,
      &pipeline_info,
      nullptr, &pipeline_));
}

void Mesh::UpdatePipeline(VkRenderPass render_pass, std::shared_ptr<Material> material) {
  if (pipeline_ != VK_NULL_HANDLE)
    return;

  CreateMeshPipeline(render_pass, material);
}

void Mesh::SubmitDraw(VkCommandBuffer cmd_buffer, uint_t frame_index, std::vector<MeshInstance> instances) const {
    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);

    geometry_->Bind(cmd_buffer);

    VkDescriptorSet lights_descriptor_set = renderer_->GetLightsDescriptorSet(frame_index);

    vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout_, BINDING_SET_LIGHTS, 1, &lights_descriptor_set, 0, nullptr);

    for (uint16_t i = 0; i < instances.size(); i++) {
        VkDescriptorSet mesh_descriptor_set = instances[i].GetMeshDescriptorSet(frame_index);

        vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_,
                                BINDING_SET_MESH, 1, &mesh_descriptor_set,
                                0, nullptr);

        VkDescriptorSet material_descriptor_set = instances[i].GetMaterialDescriptorSet(frame_index);

        vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout_, BINDING_SET_MATERIAL, 1, &material_descriptor_set, 0,
                                nullptr);

        vkCmdDrawIndexed(cmd_buffer,
                         geometry_->GetIndexCount(),
                         1, 0, 0, 0);
    }
}