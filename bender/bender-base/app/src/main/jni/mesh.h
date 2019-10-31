//
// Created by mattkw on 10/31/2019.
//

#ifndef BENDER_BASE_MESH_H
#define BENDER_BASE_MESH_H

#import <vector>

#import "vulkan_wrapper.h"
#import "glm/glm.hpp"
#import "bender_kit.h"
#import "geometry.h"
#import "shader_state.h"

class Mesh {
public:
    Mesh(BenderKit::Device *device, ShaderState *shaderState, std::vector<float> vertexData, std::vector<uint16_t> indexData);
    ~Mesh();

    void draw(VkCommandBuffer commandBuffer);

private:
    BenderKit::Device *device_;
    Geometry *geometry_;
    ShaderState *shaderState_;

    void createPipeline();
};

#endif //BENDER_BASE_MESH_H
