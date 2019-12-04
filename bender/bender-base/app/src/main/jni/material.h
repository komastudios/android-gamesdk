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
                         specular(1.0f, 1.0f, 1.0f, 128.0f) {}
  alignas(16) glm::vec3 ambient;
  alignas(16) glm::vec3 diffuse;
  alignas(16) glm::vec4 specular;
};

class Material {
 public:
  Material(Renderer *renderer,
           std::shared_ptr<ShaderState> shaders,
           std::shared_ptr<Texture> texture,
           const MaterialAttributes &attrs = MaterialAttributes());
  ~Material();

  void Cleanup();

  void OnResume(Renderer *renderer);

  void FillPipelineInfo(VkGraphicsPipelineCreateInfo *pipeline_info);

  VkDescriptorSetLayout GetMaterialDescriptorSetLayout() const { return material_descriptors_layout_; }
  VkDescriptorSet GetMaterialDescriptorSet(uint_t frame_index) const { return material_descriptor_sets_[frame_index]; }

  static void CleanupStatic() { default_texture_->Cleanup(); }
  static void OnResumeStatic(benderkit::Device &device,
                             android_app *app) { default_texture_->OnResume(device, app); }
  static void DefaultTextureGenerator(uint8_t *data);

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

  std::shared_ptr<ShaderState> GetShaders() const { return shaders_; }

  void CreateDefaultTexture(Renderer &renderer);
  void CreateSampler();
  void CreateMaterialDescriptorSetLayout();
  void CreateMaterialDescriptorSets();
};

#endif //BENDER_BASE_MATERIAL_H
