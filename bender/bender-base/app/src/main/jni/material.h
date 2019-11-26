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
    glm::vec3 color;
};

class Material {
public:
  Material(Renderer& renderer, std::shared_ptr<ShaderState> shaders, std::shared_ptr<Texture> texture,
          const glm::vec3 color = glm::vec3(1.0, 1.0, 1.0));
  ~Material();

  void fillPipelineInfo(VkGraphicsPipelineCreateInfo *pipeline_info);

  VkDescriptorSetLayout getMaterialDescriptorSetLayout() const { return material_descriptors_layout_; }
  VkDescriptorSet getMaterialDescriptorSet(uint_t frame_index) const {return material_descriptor_sets_[frame_index]; }

private:
  static std::shared_ptr<Texture> default_texture_;

  Renderer& renderer_;

  std::shared_ptr<ShaderState> shaders_;
  std::shared_ptr<Texture> texture_;
  glm::vec3 color_;
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
