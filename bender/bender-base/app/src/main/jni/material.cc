//
// Created by Matt Wang on 2019-11-08.
//

#include "material.h"
#include "shader_bindings.h"

Material::Material(Renderer& renderer, std::shared_ptr<ShaderState> shaders, Texture *texture, glm::vec3 *color) :
    renderer_(renderer) {
  shaders_ = shaders;
  texture_ = texture;
  color_ = color;
  material_buffer_ = new UniformBufferObject<MaterialAttributes>(renderer_.getDevice());

  if (texture_ != nullptr) {
      createSampler();
  }
  createMaterialDescriptorSetLayout();
  createMaterialDescriptorSets();
}

Material::~Material() {
  if (texture_ != nullptr) {
    vkDestroySampler(renderer_.getDevice().getDevice(), sampler_, nullptr);
  }
  vkDestroyDescriptorSetLayout(renderer_.getDevice().getDevice(), material_descriptors_layout_, nullptr);
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
  std::array<VkDescriptorSetLayoutBinding, 2> bindings = {};
  uint32_t index = 0;

  if (texture_ != nullptr) {
    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = FRAGMENT_BINDING_SAMPLER;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    bindings[index++] = samplerLayoutBinding;
  }

  if (color_ != nullptr) {
    VkDescriptorSetLayoutBinding attributesLayoutBinding = {};
    attributesLayoutBinding.binding = FRAGMENT_BINDING_MATERIAL_ATTRIBUTES;
    attributesLayoutBinding.descriptorCount = 1;
    attributesLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    attributesLayoutBinding.pImmutableSamplers = nullptr;
    attributesLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    bindings[index++] = attributesLayoutBinding;
  }

  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = index;
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
    std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
    uint32_t index = 0;

    if (texture_ != nullptr) {
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture_->getImageView();
        imageInfo.sampler = sampler_;

        descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[index].dstSet = material_descriptor_sets_[i];
        descriptorWrites[index].dstBinding = FRAGMENT_BINDING_SAMPLER;
        descriptorWrites[index].dstArrayElement = 0;
        descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[index].descriptorCount = 1;
        descriptorWrites[index].pImageInfo = &imageInfo;

        index++;
    }

    if (color_ != nullptr) {
        glm::vec3 color = *color_;
        material_buffer_->update(i, [&color](auto& ubo) {
            ubo.color = color;
        });

        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = material_buffer_->getBuffer(i);
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(MaterialAttributes);

        descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[index].dstSet = material_descriptor_sets_[i];
        descriptorWrites[index].dstBinding = FRAGMENT_BINDING_MATERIAL_ATTRIBUTES;
        descriptorWrites[index].dstArrayElement = 0;
        descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[index].descriptorCount = 1;
        descriptorWrites[index].pBufferInfo = &bufferInfo;

        index++;
    }

    vkUpdateDescriptorSets(renderer_.getDevice().getDevice(), index, descriptorWrites.data(),
                           0, nullptr);
  }
}
