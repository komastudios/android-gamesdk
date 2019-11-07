//
// Created by Matt Wang on 2019-11-04.
//

#include "material.h"

Material::Material(android_app *app, BenderKit::Device *device, std::vector<const char*> texFiles) {
    app_ = app;
    device_ = device;
    createTextures(texFiles);
}

Material::~Material() {
    for (uint32_t i = 0; i < material_components_.size(); i++) {
        delete material_components_[i].texture;
        vkDestroySampler(device_->getDevice(), *material_components_[i].sampler, nullptr);
    }
}

void Material::createSampler(VkSampler *sampler) {
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
    CALL_VK(vkCreateSampler(device_->getDevice(), &sampler_create_info, nullptr,
                            sampler));
}

void Material::createTextures(std::vector<const char*> texFiles) {
    assert(app_ != nullptr);
    assert(device_ != nullptr);

    for (uint32_t i = 0; i < texFiles.size(); i++) {
        VkSampler sampler;
        createSampler(&sampler);
        MaterialComponent component = {
                .texture = new Texture(*device_, app_, texFiles[i], VK_FORMAT_R8G8B8A8_SRGB),
                .sampler = &sampler,
        };
        material_components_.push_back(component);
    }
}
