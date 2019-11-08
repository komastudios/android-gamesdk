//
// Created by Andy Yu on 2019-11-08.
//

#ifndef BENDER_BASE_FONT_H_
#define BENDER_BASE_FONT_H_

#import "vulkan_wrapper.h"
#import "glm/glm.hpp"
#import "glm/gtc/quaternion.hpp"
#import "bender_helpers.h"
#import "bender_kit.h"
#import "geometry.h"
#import "shader_state.h"
#import "texture.h"
#import "mesh.h"
#import "renderer.h"
#import <unordered_map>
#import <vector>

#define FONT_SDF_PATH "textures/font_sdf.png"
#define FONT_INFO_PATH "textures/font_sdf.fnt"
#define FONT_SDF_INDEX 0

class Font {
public:
    Font(BenderKit::Device& device, Renderer& renderer, std::shared_ptr<ShaderState> shaders,
            android_app *androidAppCtx, std::string font_texture_path, std::string font_info_path);
    ~Font();
    void updatePipeline(VkRenderPass render_pass);
    void update(uint_t frame_index);
    glm::mat4 getTransform() const;

    void generateText(std::string text, VkCommandBuffer commandBuffer, uint_t frame_index);
private:
    UniformBufferObject<ModelViewProjection> *meshBuffer;

    BenderKit::Device& device_;

    Texture *texture_;
    VkSampler sampler_;

    std::shared_ptr<ShaderState> shaders_;

    VkDescriptorSetLayout font_descriptors_layout_;
    std::vector<VkDescriptorSet> font_descriptor_sets_;

    VkPipelineLayout layout_;
    VkPipelineCache cache_;
    VkPipeline pipeline_ = VK_NULL_HANDLE;

    glm::vec3 position_;
    glm::quat rotation_;
    glm::vec3 scale_;

    glm::mat4 view_;
    glm::mat4 proj_;

    VkBuffer vertexBuf_;
    VkDeviceMemory vertexBufferDeviceMemory_;

    VkBuffer indexBuf_;
    VkDeviceMemory indexBufferDeviceMemory_;

    const glm::vec3 camera_position_ = glm::vec3(0.0f, 0.0f, 3.0f);
    const glm::quat camera_rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    typedef struct Character {
        int x, y, width, height, xoffset, yoffset, xadvance;
    } Character;

    std::unordered_map<int, Font::Character> char_map;

    void parseFontInfo(const char* info_file_path, android_app *androidAppCtx);
    void createSampler();
    void createDescriptorSetLayout();
    void createDescriptors(Renderer& renderer);
    void createFontPipeline(VkRenderPass renderPass);
};

#endif //BENDER_BASE_FONT_H_
