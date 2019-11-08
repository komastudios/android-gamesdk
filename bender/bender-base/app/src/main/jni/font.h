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
#define FONT_STRING_SIZE 120
#define FONT_NUM_QUAD_INDICES 6
#define FONT_SCREEN_TOP -1.0f
#define FONT_SCREEN_LEFT -1.0f

class Font {
public:
    Font(Renderer& renderer, android_app *androidAppCtx,
         const std::string font_texture_path, const std::string font_info_path);
    ~Font();

    void drawString(std::string text, float x, float y,
                    VkCommandBuffer commandBuffer, VkRenderPass render_pass, uint_t frame_index);
private:
    Renderer& renderer_;

    Texture *texture_;
    VkSampler sampler_;

    std::shared_ptr<ShaderState> shader_;

    VkDescriptorSetLayout font_descriptors_layout_;
    std::vector<VkDescriptorSet> font_descriptor_sets_;

    VkPipelineLayout layout_;
    VkPipelineCache cache_;
    VkPipeline pipeline_ = VK_NULL_HANDLE;

    VkBuffer vertexBuf_;
    VkDeviceMemory vertexBufferDeviceMemory_;

    typedef struct Character {
        int x, y, width, height, xoffset, yoffset, xadvance;
    } Character;

    std::unordered_map<int, Font::Character> char_map;

    void createFontShaders(android_app *androidAppCtx);
    void parseFontInfo(const char* info_file_path, android_app *androidAppCtx);
    void createSampler();
    void createDescriptorSetLayout();
    void createDescriptors(Renderer& renderer);
    void createFontPipeline(VkRenderPass renderPass);
    void updatePipeline(VkRenderPass render_pass);
};

#endif //BENDER_BASE_FONT_H_
