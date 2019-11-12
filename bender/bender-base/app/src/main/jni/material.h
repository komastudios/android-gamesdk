//
// Created by Matt Wang on 2019-11-08.
//

#ifndef BENDER_BASE_MATERIAL_H
#define BENDER_BASE_MATERIAL_H

#include "vulkan_wrapper.h"
#include "renderer.h"
#include "shader_state.h"
#include "texture.h"

struct MaterialAttributes {
    alignas(16) glm::vec3 color;
};

class Material {
public:
    Material(Renderer& renderer, std::shared_ptr<ShaderState> shaders, Texture *texture, glm::vec3 *color);
    ~Material();

    std::shared_ptr<ShaderState> getShaders() const { return shaders_; }
    VkDescriptorSetLayout getMaterialDescriptorSetLayout() const { return material_descriptors_layout_; }
    VkDescriptorSet getMaterialDescriptorSet(uint_t frame_index) const {return material_descriptor_sets_[frame_index]; }

private:
    Renderer& renderer_;
    std::shared_ptr<ShaderState> shaders_;
    Texture *texture_;
    glm::vec3 *color_;
    VkSampler sampler_;

    std::unique_ptr<UniformBufferObject<MaterialAttributes>> material_buffer_;

    VkDescriptorSetLayout material_descriptors_layout_;
    std::vector<VkDescriptorSet> material_descriptor_sets_;

    void createSampler();
    void createMaterialDescriptorSetLayout();
    void createMaterialDescriptorSets();
};


#endif //BENDER_BASE_MATERIAL_H
