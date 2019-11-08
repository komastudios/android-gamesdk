//
// Created by Matt Wang on 2019-11-08.
//

#include "material.h"
#include "shader_bindings.h"

Material::Material(Renderer& renderer, Texture& texture) : renderer_(renderer), texture_(texture) {

}

Material::~Material() {

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
  CALL_VK(vkCreateSampler(renderer_.getDevice().getDevice(), &sampler_create_info, nullptr, &sampler_));
}

void Material::createMaterialDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
  samplerLayoutBinding.binding = FRAGMENT_BINDING_SAMPLER;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 1> bindings = {samplerLayoutBinding};

  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  CALL_VK(vkCreateDescriptorSetLayout(renderer_.getDevice().getDevice(), &layoutInfo, nullptr,
                                        &material_descriptors_layout_));
}

void Material::createMaterialDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(renderer_.getDevice().getDisplayImagesSize(),
                                           material_descriptors_layout_);

  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = renderer_.getDescriptorPool();
  allocInfo.descriptorSetCount = static_cast<uint32_t>(renderer_.getDevice().getDisplayImagesSize());
  allocInfo.pSetLayouts = layouts.data();

  material_descriptor_sets_.resize(renderer_.getDevice().getDisplayImagesSize());
  CALL_VK(vkAllocateDescriptorSets(renderer_.getDevice().getDevice(), &allocInfo, material_descriptor_sets_.data()));

  for (size_t i = 0; i < renderer_.getDevice().getDisplayImagesSize(); i++) {
    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = texture_.getImageView();
    imageInfo.sampler = sampler_;

    std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = material_descriptor_sets_[i];
    descriptorWrites[0].dstBinding = FRAGMENT_BINDING_SAMPLER;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(renderer_.getDevice().getDevice(), descriptorWrites.size(), descriptorWrites.data(),
                           0, nullptr);
  }
}