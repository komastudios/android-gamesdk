//
// Created by Matt Wang on 2019-11-08.
//

#include "material.h"
#include "shader_bindings.h"
#include "constants.h"

std::shared_ptr<Texture> Material::default_texture_ = nullptr;

void Material::createDefaultTexture(Renderer& renderer) {
  if (default_texture_ != nullptr) { return; }
  unsigned char imgData[4] = {255, 255, 255, 0};
  default_texture_ = std::make_shared<Texture>(renderer.getDevice(), imgData, 1, 1, VK_FORMAT_R8G8B8A8_SRGB);
}

Material::Material(Renderer& renderer, std::shared_ptr<ShaderState> shaders, std::shared_ptr<Texture> texture, const glm::vec3 color) :
    renderer_(renderer) {
  shaders_ = shaders;
  if (texture == nullptr) {
    createDefaultTexture(renderer);
    texture_ = default_texture_;
  } else {
    texture_ = texture;
  }
  color_ = color;
  material_buffer_ = std::make_unique<UniformBufferObject<MaterialAttributes>>(renderer_.getDevice());

  createSampler();
  createMaterialDescriptorSetLayout();
  createMaterialDescriptorSets();
}

Material::~Material() {
  vkDestroySampler(renderer_.getVulkanDevice(), sampler_, nullptr);
  vkDestroyDescriptorSetLayout(renderer_.getVulkanDevice(), material_descriptors_layout_, nullptr);
}

void Material::createSampler() {
  const VkSamplerCreateInfo sampler_create_info = {
    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    .pNext = nullptr,
    .magFilter = VK_FILTER_NEAREST,
    .minFilter = VK_FILTER_NEAREST,
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
  CALL_VK(vkCreateSampler(renderer_.getVulkanDevice(), &sampler_create_info, nullptr, &sampler_));
}

void Material::fillPipelineInfo(VkGraphicsPipelineCreateInfo *pipeline_info) {
  shaders_->fillPipelineInfo(pipeline_info);
}

void Material::createMaterialDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
  samplerLayoutBinding.binding = FRAGMENT_BINDING_SAMPLER;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding attributesLayoutBinding = {};
  attributesLayoutBinding.binding = FRAGMENT_BINDING_MATERIAL_ATTRIBUTES;
  attributesLayoutBinding.descriptorCount = 1;
  attributesLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  attributesLayoutBinding.pImmutableSamplers = nullptr;
  attributesLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, MATERIAL_DESCRIPTOR_LAYOUT_SIZE> bindings = {samplerLayoutBinding, attributesLayoutBinding};

  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  CALL_VK(vkCreateDescriptorSetLayout(renderer_.getVulkanDevice(), &layoutInfo, nullptr,
                                        &material_descriptors_layout_));
}

void Material::createMaterialDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(renderer_.getDevice().getDisplayImages().size(),
                                           material_descriptors_layout_);

  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = renderer_.getDescriptorPool();
  allocInfo.descriptorSetCount = static_cast<uint32_t>(renderer_.getDevice().getDisplayImages().size());
  allocInfo.pSetLayouts = layouts.data();

  material_descriptor_sets_.resize(renderer_.getDevice().getDisplayImages().size());
  CALL_VK(vkAllocateDescriptorSets(renderer_.getVulkanDevice(), &allocInfo, material_descriptor_sets_.data()));

  glm::vec3 color = color_;

  for (size_t i = 0; i < renderer_.getDevice().getDisplayImages().size(); i++) {
    std::array<VkWriteDescriptorSet, MATERIAL_DESCRIPTOR_LAYOUT_SIZE> descriptorWrites = {};

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = texture_->getImageView();
    imageInfo.sampler = sampler_;

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = material_descriptor_sets_[i];
    descriptorWrites[0].dstBinding = FRAGMENT_BINDING_SAMPLER;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pImageInfo = &imageInfo;

    material_buffer_->update(i, [&color](auto& ubo) {
        ubo.color = color;
    });

    VkDescriptorBufferInfo materialBufferInfo = {};
    materialBufferInfo.buffer = material_buffer_->getBuffer(i);
    materialBufferInfo.offset = 0;
    materialBufferInfo.range = sizeof(MaterialAttributes);

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = material_descriptor_sets_[i];
    descriptorWrites[1].dstBinding = FRAGMENT_BINDING_MATERIAL_ATTRIBUTES;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pBufferInfo = &materialBufferInfo;

    vkUpdateDescriptorSets(renderer_.getVulkanDevice(), descriptorWrites.size(), descriptorWrites.data(),
                           0, nullptr);
  }
}
