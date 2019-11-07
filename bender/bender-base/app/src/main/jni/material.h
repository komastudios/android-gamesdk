//
// Created by Matt Wang on 2019-11-04.
//

#ifndef BENDER_BASE_MATERIAL_H
#define BENDER_BASE_MATERIAL_H

#import "vulkan_wrapper.h"
#import "texture.h"

struct MaterialComponent {
    Texture* texture;
    VkSampler* sampler;
};

class Material {
public:
    Material(android_app *app, BenderKit::Device *device, std::vector<const char*> texFiles);
    ~Material();

    Texture *getTexture(int i) const {return material_components_[i].texture; }
    VkSampler *getSampler(int i) const {return material_components_[i].sampler; }

private:
    android_app *app_;
    BenderKit::Device *device_;
    std::vector<MaterialComponent> material_components_;

    void createSampler(VkSampler *sampler);
    void createTextures(std::vector<const char*> texFiles);
};


#endif //BENDER_BASE_MATERIAL_H
