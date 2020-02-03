//
// Created by Matt Wang on 2019-11-08.
//

#include "material.h"
#include "shader_bindings.h"
#include "constants.h"

std::shared_ptr<Texture> Material::default_texture_ = nullptr;

void Material::DefaultTextureGenerator(uint8_t *data) {
  unsigned char img_data[4] = {255, 255, 255, 0};
  memcpy(data, img_data, 4);
}

void Material::CreateDefaultTexture(Renderer &renderer) {
  if (default_texture_ != nullptr) { return; }
  unsigned char img_data[4] = {255, 255, 255, 0};
  default_texture_ = std::make_shared<Texture>(renderer.GetDevice(), img_data, 1, 1,
                                               VK_FORMAT_R8G8B8A8_SRGB, DefaultTextureGenerator);
}

Material::Material(Renderer *renderer, std::shared_ptr<ShaderState> shaders,
                   std::vector<std::shared_ptr<Texture>> &texture, const MaterialAttributes &attrs) :
    renderer_(renderer) {
  shaders_ = shaders;
  for (auto currTexture : texture) {
    if (currTexture == nullptr){
      CreateDefaultTexture(*renderer);
      texture_.push_back(default_texture_);
    }
    else{
      texture_.push_back(currTexture);
    }
  }
  material_attributes_ = attrs;
  material_buffer_ = std::make_unique<UniformBufferObject<MaterialAttributes>>(renderer_->GetDevice());

  CreateSampler();
  CreateMaterialDescriptorSetLayout();
  CreateMaterialDescriptorSets();
}

Material::~Material() {
  Cleanup();
}

void Material::Cleanup() {
  vkDeviceWaitIdle(renderer_->GetVulkanDevice());
  vkDestroySampler(renderer_->GetVulkanDevice(), sampler_, nullptr);
  vkDestroyDescriptorSetLayout(renderer_->GetVulkanDevice(), material_descriptors_layout_, nullptr);
  material_buffer_.reset();
}

void Material::OnResume(Renderer *renderer) {
  renderer_ = renderer;
  material_buffer_ = std::make_unique<UniformBufferObject<MaterialAttributes>>(renderer_->GetDevice());
  CreateSampler();
  CreateMaterialDescriptorSetLayout();
  CreateMaterialDescriptorSets();
}

void Material::CreateSampler() {
  const VkSamplerCreateInfo sampler_Create_info = {
    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    .pNext = nullptr,
    .magFilter = VK_FILTER_LINEAR,
    .minFilter = VK_FILTER_LINEAR,
    .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
    .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .mipLodBias = 0.0f,
    .maxAnisotropy = 1,
    .compareOp = VK_COMPARE_OP_NEVER,
    .minLod = 0.0f,
    .maxLod = 0.0f,
    .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
    .unnormalizedCoordinates = VK_FALSE,
  };
  CALL_VK(vkCreateSampler(renderer_->GetVulkanDevice(), &sampler_Create_info, nullptr, &sampler_));
}

void Material::FillPipelineInfo(VkGraphicsPipelineCreateInfo *pipeline_info) {
  shaders_->FillPipelineInfo(pipeline_info);
}

void Material::CreateMaterialDescriptorSetLayout() {
  std::vector<VkDescriptorSetLayoutBinding>
      bindings(texture_.size() + FRAGMENT_BINDING_MATERIAL_COUNT);

  VkDescriptorSetLayoutBinding attributes_layout_binding = {
      .binding = FRAGMENT_BINDING_MATERIAL_ATTRIBUTES,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .pImmutableSamplers = nullptr,
      .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
  };
  bindings[0] = attributes_layout_binding;

  for (uint32_t x = 0; x < texture_.size(); x++) {
    VkDescriptorSetLayoutBinding currSampler = {
        .binding = x + FRAGMENT_BINDING_MATERIAL_COUNT,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImmutableSamplers = nullptr,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    };
    bindings[x + FRAGMENT_BINDING_MATERIAL_COUNT] = currSampler;
  }

  VkDescriptorSetLayoutCreateInfo layout_info = {
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
          .bindingCount = static_cast<uint32_t>(bindings.size()),
          .pBindings = bindings.data(),
  };

  CALL_VK(vkCreateDescriptorSetLayout(renderer_->GetVulkanDevice(), &layout_info, nullptr,
                                        &material_descriptors_layout_));
}

void Material::CreateMaterialDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(renderer_->GetDevice().GetDisplayImages().size(),
                                           material_descriptors_layout_);

  VkDescriptorSetAllocateInfo alloc_info = {
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
          .descriptorPool = renderer_->GetDescriptorPool(),
          .descriptorSetCount = static_cast<uint32_t>(renderer_->GetDevice().GetDisplayImages().size()),
          .pSetLayouts = layouts.data(),
  };

  material_descriptor_sets_.resize(renderer_->GetDevice().GetDisplayImages().size());
  CALL_VK(vkAllocateDescriptorSets(renderer_->GetVulkanDevice(), &alloc_info, material_descriptor_sets_.data()));

  MaterialAttributes attrs = material_attributes_;

  for (size_t i = 0; i < renderer_->GetDevice().GetDisplayImages().size(); i++) {
    std::vector<VkWriteDescriptorSet>
        descriptor_writes(texture_.size() + FRAGMENT_BINDING_MATERIAL_COUNT);

    material_buffer_->Update(i, [&attrs](auto &ubo) {
      ubo.ambient = attrs.ambient;
      ubo.specular = attrs.specular;
      ubo.diffuse = attrs.diffuse;
    });

    VkDescriptorBufferInfo material_buffer_info = {
        .buffer = material_buffer_->GetBuffer(i),
        .offset = 0,
        .range = sizeof(MaterialAttributes),
    };

    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = material_descriptor_sets_[i];
    descriptor_writes[0].dstBinding = FRAGMENT_BINDING_MATERIAL_ATTRIBUTES;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &material_buffer_info;

    std::vector<VkDescriptorImageInfo> image_info(texture_.size());
    for (int x = 0; x < texture_.size(); x++){
      image_info[x].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      image_info[x].imageView = texture_[x]->GetImageView(),
      image_info[x].sampler = sampler_;

      int arrayIdx = x + FRAGMENT_BINDING_MATERIAL_COUNT;
      descriptor_writes[arrayIdx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptor_writes[arrayIdx].dstSet = material_descriptor_sets_[i];
      descriptor_writes[arrayIdx].dstBinding = arrayIdx;
      descriptor_writes[arrayIdx].dstArrayElement = 0;
      descriptor_writes[arrayIdx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptor_writes[arrayIdx].descriptorCount = 1;
      descriptor_writes[arrayIdx].pImageInfo = &image_info[x];
    }

    vkUpdateDescriptorSets(renderer_->GetVulkanDevice(), descriptor_writes.size(), descriptor_writes.data(),
                           0, nullptr);
  }
}
