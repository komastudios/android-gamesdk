//
// Created by Matt Wang on 2019-11-08.
//

#ifndef BENDER_BASE_MATERIAL_H
#define BENDER_BASE_MATERIAL_H

#include "vulkan_wrapper.h"
#include "renderer.h"
#include "shader_state.h"
#include "texture.h"

struct alignas(16) MaterialAttributes {
  alignas(16) glm::vec3 ambient = {1.0f, 1.0f, 1.0f};
  alignas(16) glm::vec3 diffuse = {1.0f, 1.0f, 1.0f};
  alignas(16) glm::vec3 specular = {1.0f, 1.0f, 1.0f};
  alignas(16) float specularExponent = 128;
};

class Material {
public:
  Material(Renderer& renderer, std::shared_ptr<ShaderState> shaders, std::shared_ptr<Texture> texture,
          const MaterialAttributes &attr);
  ~Material();

  void fillPipelineInfo(VkGraphicsPipelineCreateInfo *pipeline_info);

  VkDescriptorSetLayout getMaterialDescriptorSetLayout() const { return material_descriptors_layout_; }
  VkDescriptorSet getMaterialDescriptorSet(uint_t frame_index) const {return material_descriptor_sets_[frame_index]; }

  static void cleanup() { default_texture_.reset(); }

private:
  static std::shared_ptr<Texture> default_texture_;

  Renderer& renderer_;

  std::shared_ptr<ShaderState> shaders_;
  std::shared_ptr<Texture> texture_;
  MaterialAttributes material_attributes_;
  VkSampler sampler_;

  std::unique_ptr<UniformBufferObject<MaterialAttributes>> material_buffer_;

  VkDescriptorSetLayout material_descriptors_layout_;
  std::vector<VkDescriptorSet> material_descriptor_sets_;

  std::shared_ptr<ShaderState> getShaders() const { return shaders_; }

  void createDefaultTexture(Renderer& renderer);
  void createSampler();
  void createMaterialDescriptorSetLayout();
  void createMaterialDescriptorSets();
};

#endif //BENDER_BASE_MATERIAL_H
