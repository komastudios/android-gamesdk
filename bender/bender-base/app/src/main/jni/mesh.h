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
    Mesh(BenderKit::Device *device, std::vector<float> vertexData, std::vector<uint16_t> indexData,
         VkDescriptorSetLayout *descriptorSetLayout, ShaderState *shaderState, VkRenderPass* renderPass);
    ~Mesh();

    void submitDraw(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet);

private:
    BenderKit::Device *device_;
    Geometry *geometry_;
    VkDescriptorSetLayout *descriptorSetLayout_;
    ShaderState *shaderState_;
    VkRenderPass *renderPass_;

    VkPipelineLayout layout_;
    VkPipelineCache cache_;
    VkPipeline pipeline_;

    void createPipeline();
};

#endif //BENDER_BASE_MESH_H
