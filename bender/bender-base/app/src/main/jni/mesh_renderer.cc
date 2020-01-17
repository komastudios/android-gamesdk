//
// Created by mattkw on 10/31/2019.
//

#include <glm/gtc/matrix_transform.hpp>

#include "mesh_renderer.h"
#include "material.h"
#include "shader_bindings.h"

MeshRenderer::MeshRenderer(Renderer *renderer,
           std::shared_ptr<Material> material,
           std::shared_ptr<Geometry> geometry) :
    renderer_(renderer) {

//  mesh_buffer_ = std::make_unique<UniformBufferObject<ModelViewProjection>>(renderer_->GetDevice());
  CreateMeshDescriptorSetLayout();
  CreateMeshDescriptors();
}

MeshRenderer::MeshRenderer(Renderer *renderer,
           std::shared_ptr<Material> material,
           const std::vector<float> &vertex_data,
           const std::vector<uint16_t> &index_data) :
    MeshRenderer(renderer,
         material,
         std::make_shared<Geometry>(renderer->GetDevice(), vertex_data, index_data)) {}

MeshRenderer::~MeshRenderer() {
  Cleanup();
}

void MeshRenderer::Cleanup() {
  vkDeviceWaitIdle(renderer_->GetVulkanDevice());
  vkDestroyPipeline(renderer_->GetVulkanDevice(), pipeline_, nullptr);
  vkDestroyPipelineLayout(renderer_->GetVulkanDevice(), layout_, nullptr);
  vkDestroyDescriptorSetLayout(renderer_->GetVulkanDevice(), mesh_descriptors_layout_, nullptr);
//  mesh_buffer_.reset();
  pipeline_ = VK_NULL_HANDLE;
}

void MeshRenderer::OnResume(Renderer *renderer) {
  renderer_ = renderer;
//  mesh_buffer_ = std::make_unique<UniformBufferObject<ModelViewProjection>>(renderer_->GetDevice());
  CreateMeshDescriptorSetLayout();
  CreateMeshDescriptors();
}

void MeshRenderer::CreateMeshPipeline(VkRenderPass render_pass, std::shared_ptr<Material> material_template) {
  std::array<VkDescriptorSetLayout, 3> layouts;

  layouts[BINDING_SET_MESH] = mesh_descriptors_layout_;
  layouts[BINDING_SET_MATERIAL] = material_template->GetMaterialDescriptorSetLayout();
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

  material_template->FillPipelineInfo(&pipeline_info);

  CALL_VK(vkCreateGraphicsPipelines(
      renderer_->GetVulkanDevice(),
      renderer_->GetPipelineCache(), 1,
      &pipeline_info,
      nullptr, &pipeline_));
}

void MeshRenderer::UpdatePipeline(VkRenderPass render_pass, std::shared_ptr<Material> material_template) {
  if (pipeline_ != VK_NULL_HANDLE)
    return;

  CreateMeshPipeline(render_pass, material_template);
}

void MeshRenderer::CreateMeshDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding instance_layout_binding = {
          .binding = VERTEX_BINDING_MODEL_VIEW_PROJECTION,
          .descriptorCount = 1,
          .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          .pImmutableSamplers = nullptr,
          .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
  };

  std::array<VkDescriptorSetLayoutBinding, 1> bindings = {instance_layout_binding};
  VkDescriptorSetLayoutCreateInfo layout_info = {
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
          .bindingCount = bindings.size(),
          .pBindings = bindings.data(),
  };

  CALL_VK(vkCreateDescriptorSetLayout(renderer_->GetVulkanDevice(), &layout_info, nullptr,
                                      &mesh_descriptors_layout_));
}

void MeshRenderer::CreateMeshDescriptors() {
  std::vector<VkDescriptorSetLayout> layouts(renderer_->GetDevice().GetDisplayImages().size(),
                                             mesh_descriptors_layout_);

  VkDescriptorSetAllocateInfo alloc_info = {
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
          .descriptorPool = renderer_->GetDescriptorPool(),
          .descriptorSetCount = static_cast<uint32_t>(renderer_->GetDevice().GetDisplayImages().size()),
          .pSetLayouts = layouts.data(),
  };

  mesh_descriptor_sets_.resize(renderer_->GetDevice().GetDisplayImages().size());
  CALL_VK(vkAllocateDescriptorSets(renderer_->GetVulkanDevice(),
                                   &alloc_info,
                                   mesh_descriptor_sets_.data()));

  for (size_t i = 0; i < renderer_->GetDevice().GetDisplayImages().size(); i++) {
    VkDescriptorBufferInfo buffer_info = {
            .buffer = mesh_buffer_->GetBuffer(i),
            .offset = 0,
            .range = sizeof(ModelViewProjection),
    };

    std::array<VkWriteDescriptorSet, 1> descriptor_writes = {};

    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = mesh_descriptor_sets_[i];
    descriptor_writes[0].dstBinding = VERTEX_BINDING_MODEL_VIEW_PROJECTION;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(renderer_->GetVulkanDevice(),
                           descriptor_writes.size(),
                           descriptor_writes.data(),
                           0,
                           nullptr);

  }
}

void MeshRenderer::SetupDraw(VkCommandBuffer cmd_buffer, uint32_t frame_index,
                             std::shared_ptr<Geometry> geometry) const {
  vkCmdBindPipeline(cmd_buffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline_);

  geometry->Bind(cmd_buffer);

  VkDescriptorSet lights_descriptor_set = renderer_->GetLightsDescriptorSet(frame_index);

  vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          layout_, BINDING_SET_LIGHTS, 1, &lights_descriptor_set, 0, nullptr);
}

void MeshRenderer::SubmitDraw(VkCommandBuffer cmd_buffer, uint_t frame_index, uint32_t index_count,
        std::vector<std::shared_ptr<Material>>& materials, std::vector<VkDescriptorSet>& mesh_descriptor_sets) const {
  assert(materials.size() == mesh_descriptor_sets.size());

  for (uint32_t i = 0; i < materials.size(); i++) {
    vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_, BINDING_SET_MESH,
                            1, &mesh_descriptor_sets[i], 0, nullptr);

    VkDescriptorSet material_descriptor_set = materials[i]->GetMaterialDescriptorSet(frame_index);

    vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout_, BINDING_SET_MATERIAL, 1, &material_descriptor_set, 0, nullptr);

    vkCmdDrawIndexed(cmd_buffer, index_count, 1, 0, 0, 0);
  }
}

//void MeshRenderer::Translate(glm::vec3 offset) {
//  position_ += offset;
//}
//
//void MeshRenderer::Rotate(glm::vec3 axis, float angle) {
//  rotation_ *= glm::angleAxis(glm::radians(angle), axis);
//  rotation_ = glm::normalize(rotation_);
//}
//
//void MeshRenderer::Scale(glm::vec3 scaling) {
//  scale_ *= scaling;
//}
//
//void MeshRenderer::SetPosition(glm::vec3 position) {
//  position_ = position;
//}
//
//void MeshRenderer::SetRotation(glm::vec3 axis, float angle) {
//  rotation_ = glm::angleAxis(glm::radians(angle), axis);
//}
//
//void MeshRenderer::SetScale(glm::vec3 scale) {
//  scale_ = scale;
//}
//
//glm::vec3 MeshRenderer::GetPosition() const {
//  return position_;
//}
//
//glm::quat MeshRenderer::GetRotation() const {
//  return rotation_;
//}
//
//glm::vec3 MeshRenderer::GetScale() const {
//  return scale_;
//}
//
//int MeshRenderer::GetTrianglesCount() const {
//  return geometry_->GetIndexCount() / 3;
//}