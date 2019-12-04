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
  MaterialAttributes() : ambient(1.0f, 1.0f, 1.0f),
                         diffuse(1.0f, 1.0f, 1.0f),
                         specular(1.0f, 1.0f, 1.0f),
                         specularExponent(128) {}
  alignas(16) glm::vec3 ambient;
  alignas(16) glm::vec3 diffuse;
  alignas(16) glm::vec3 specular;
  alignas(16) float specularExponent;
};

class Material {
 public:
  Material(Renderer *renderer,
           std::shared_ptr<ShaderState> shaders,
           std::shared_ptr<Texture> texture,
           const MaterialAttributes &attrs = MaterialAttributes());
  ~Material();

  void cleanup();

  void onResume(Renderer *renderer);

  void fillPipelineInfo(VkGraphicsPipelineCreateInfo *pipeline_info);

  VkDescriptorSetLayout getMaterialDescriptorSetLayout() const { return material_descriptors_layout_; }
  VkDescriptorSet getMaterialDescriptorSet(uint_t frame_index) const { return material_descriptor_sets_[frame_index]; }

  static void cleanupStatic() { default_texture_->cleanup(); }
  static void onResumeStatic(BenderKit::Device &device,
                             android_app *app) { default_texture_->onResume(device, app); }
  static void defaultTextureGenerator(uint8_t *data);

 private:
  static std::shared_ptr<Texture> default_texture_;

  Renderer *renderer_;

  std::shared_ptr<ShaderState> shaders_;
  std::shared_ptr<Texture> texture_;
  MaterialAttributes material_attributes_;
  VkSampler sampler_;

  std::unique_ptr<UniformBufferObject<MaterialAttributes>> material_buffer_;

  VkDescriptorSetLayout material_descriptors_layout_;
  std::vector<VkDescriptorSet> material_descriptor_sets_;

  std::shared_ptr<ShaderState> getShaders() const { return shaders_; }

  void createDefaultTexture(Renderer &renderer);
  void createSampler();
  void createMaterialDescriptorSetLayout();
  void createMaterialDescriptorSets();
};

#endif //BENDER_BASE_MATERIAL_H
