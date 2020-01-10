//
// Created by mattkw on 10/31/2019.
//

#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"
#include "material.h"
#include "shader_bindings.h"

Mesh::Mesh(Renderer *renderer,
           std::shared_ptr<Geometry> geometry) :
    renderer_(renderer), geometry_(geometry) {
  CreateMeshDescriptorSetLayout();
}

Mesh::Mesh(Renderer *renderer,
           const std::vector<float> &vertex_data,
           const std::vector<uint16_t> &index_data) :
    Mesh(renderer,
         std::make_shared<Geometry>(renderer->GetDevice(), vertex_data, index_data)) {}

Mesh::Mesh(const Mesh &other, std::shared_ptr<Geometry> geometry) :
    renderer_(other.renderer_),
    geometry_(geometry) {
  CreateMeshDescriptorSetLayout();
}

Mesh::~Mesh() {
  Cleanup();
}

void Mesh::Cleanup() {
  vkDeviceWaitIdle(renderer_->GetVulkanDevice());
  vkDestroyPipeline(renderer_->GetVulkanDevice(), pipeline_, nullptr);
  vkDestroyPipelineLayout(renderer_->GetVulkanDevice(), layout_, nullptr);
  vkDestroyDescriptorSetLayout(renderer_->GetVulkanDevice(), mesh_descriptors_layout_, nullptr);
  pipeline_ = VK_NULL_HANDLE;
}

void Mesh::OnResume(Renderer *renderer) {
  renderer_ = renderer;
  CreateMeshDescriptorSetLayout();
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

void Mesh::CreateMeshDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding ubo_layout_binding = {
          .binding = VERTEX_BINDING_MODEL_VIEW_PROJECTION,
          .descriptorCount = 1,
          .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          .pImmutableSamplers = nullptr,
          .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
  };

  std::array<VkDescriptorSetLayoutBinding, 1> bindings = {ubo_layout_binding};
  VkDescriptorSetLayoutCreateInfo layout_info = {
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
          .bindingCount = bindings.size(),
          .pBindings = bindings.data(),
  };

  CALL_VK(vkCreateDescriptorSetLayout(renderer_->GetVulkanDevice(), &layout_info, nullptr,
                                      &mesh_descriptors_layout_));
}

void Mesh::UpdatePipeline(VkRenderPass render_pass, std::shared_ptr<Material> material) {
  if (pipeline_ != VK_NULL_HANDLE)
    return;

  CreateMeshPipeline(render_pass, material);
}

void Mesh::SetupDraw(VkCommandBuffer cmd_buffer, uint_t frame_index) {
  cmd_buffer_ = cmd_buffer;
  vkCmdBindPipeline(cmd_buffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline_);

  geometry_->Bind(cmd_buffer);

  VkDescriptorSet lights_descriptor_set = renderer_->GetLightsDescriptorSet(frame_index);

  vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          layout_, BINDING_SET_LIGHTS, 1, &lights_descriptor_set, 0, nullptr);
}

void Mesh::SubmitDraw(VkDescriptorSet mesh_descriptor_set, VkDescriptorSet material_descriptor_set) const {
  vkCmdBindDescriptorSets(cmd_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_,
                          BINDING_SET_MESH, 1, &mesh_descriptor_set, 0,
                          nullptr);

  vkCmdBindDescriptorSets(cmd_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          layout_, BINDING_SET_MATERIAL, 1, &material_descriptor_set, 0,
                          nullptr);

  vkCmdDrawIndexed(cmd_buffer_, geometry_->GetIndexCount(), 1, 0, 0, 0);
}