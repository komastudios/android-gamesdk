//
// Created by Andy Yu on 2019-11-08.
//

#ifndef BENDER_BASE_FONT_H_
#define BENDER_BASE_FONT_H_

#import "vulkan_wrapper.h"
#import "bender_kit.h"
#import "shader_state.h"
#import "texture.h"
#import "renderer.h"
#import <unordered_map>

#define FONT_SDF_PATH "textures/font_sdf.png"
#define FONT_INFO_PATH "textures/font_sdf.fnt"
#define FONT_SDF_INDEX 0
#define FONT_STRING_SIZE 120
#define FONT_NUM_QUAD_INDICES 6
#define FONT_ATTR_COUNT 4
#define FONT_VERTEX_BUFFER_SIZE sizeof(float) * FONT_STRING_SIZE * FONT_NUM_QUAD_INDICES * FONT_ATTR_COUNT
#define FONT_SCREEN_TOP -1.0f
#define FONT_SCREEN_LEFT -1.0f

class Font {
public:
    Font(Renderer& renderer, android_app &androidAppCtx,
         const std::string& font_texture_path, const std::string& font_info_path);
    ~Font();

    void drawString(const std::string& text, float text_size, float x, float y,
                    VkCommandBuffer commandBuffer, VkRenderPass render_pass, uint_t frame_index);

  void setResolutionRatios() {
      resolution_ratio_x_ = (float) renderer_.getDevice().getDisplaySize().width
          / renderer_.getDevice().getDisplaySizeOriented().width;
      resolution_ratio_y = (float) renderer_.getDevice().getDisplaySize().height
          / renderer_.getDevice().getDisplaySizeOriented().height;
  }

private:
    Renderer& renderer_;

    Texture *texture_;
    VkSampler sampler_;

    std::unique_ptr<UniformBufferObject<glm::mat4>> orientation_matrix_;

    std::shared_ptr<ShaderState> shader_;

    VkDescriptorSetLayout font_descriptors_layout_;
    std::vector<VkDescriptorSet> font_descriptor_sets_;

    VkPipelineLayout layout_;
    VkPipelineCache cache_;
    VkPipeline pipeline_ = VK_NULL_HANDLE;

    VkBuffer vertexBuf_;
    VkDeviceMemory vertexBufferDeviceMemory_;

    VkDeviceSize offset_;
    int current_frame_;
    float resolution_ratio_x_, resolution_ratio_y;

    typedef struct Character {
        int x, y, width, height, xoffset, yoffset, xadvance;
    } Character;

    std::unordered_map<int, Font::Character> char_map;

    void createFontShaders(android_app &androidAppCtx);
    void parseFontInfo(const char* info_file_path, android_app &androidAppCtx);
    void createSampler();
    void createDescriptorSetLayout();
    void createDescriptors(Renderer& renderer);
    void createFontPipeline(VkRenderPass renderPass);
    void updatePipeline(VkRenderPass render_pass);
};

#endif //BENDER_BASE_FONT_H_
