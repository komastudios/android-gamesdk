//
// Created by mattkw on 10/31/2019.
//

#ifndef BENDER_BASE_MESH_H
#define BENDER_BASE_MESH_H

#import <vector>

#import "vulkan_wrapper.h"
#import "glm/glm.hpp"
#import "glm/gtc/quaternion.hpp"
#import "bender_kit.h"
#import "geometry.h"
#import "shader_state.h"

class Mesh {
public:
    Mesh(BenderKit::Device *device, std::vector<float> vertexData, std::vector<uint16_t> indexData,
         VkDescriptorSetLayout *descriptorSetLayout, ShaderState *shaderState, VkRenderPass* renderPass);
    ~Mesh();
    void submitDraw(VkCommandBuffer commandBuffer, VkDescriptorSet& descriptorSet) const;


    void translate(glm::vec3 offset);
    void rotate(glm::vec3 axis, float angle);
    void scale(glm::vec3 scaling);

    void setPosition(glm::vec3 position);
    void setRotation(glm::vec3 axis, float angle);
    void setScale(glm::vec3 scale);

    glm::vec3 getPosition();
    glm::quat getRotation();
    glm::vec3 getScale();

    glm::mat4 getTransform();

private:
    BenderKit::Device *device_;
    Geometry *geometry_;
    glm::vec3 position_;
    glm::quat rotation_;
    glm::vec3 scale_;

    VkPipelineLayout layout_;
    VkPipelineCache cache_;
    VkPipeline pipeline_;

    void createMeshPipeline(VkDescriptorSetLayout *descriptorSetLayout, ShaderState *shaderState,
            VkRenderPass *renderPass);
};

#endif //BENDER_BASE_MESH_H
