//
// Created by Matt Wang on 2019-11-08.
//

#ifndef BENDER_BASE_MATERIAL_H
#define BENDER_BASE_MATERIAL_H

#include "vulkan_wrapper.h"
#include "renderer.h"
#include "texture.h"

class Material {
public:
    Material(Renderer& renderer, Texture& texture);

    ~Material();

private:
    Renderer& renderer_;
    Texture texture_;
    VkSampler sampler_;

    VkDescriptorSetLayout material_descriptors_layout_;
    std::vector<VkDescriptorSet> material_descriptor_sets_;

    void createSampler();
    void createMaterialDescriptorSetLayout();
    void createMaterialDescriptorSets();
};


#endif //BENDER_BASE_MATERIAL_H
