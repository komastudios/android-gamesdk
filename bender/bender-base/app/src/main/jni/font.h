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
#define FONT_NUM_QUAD_INDICES 6

class Font {
public:
    Font(Renderer& renderer, android_app &androidAppCtx,
         const std::string& font_texture_path, const std::string& font_info_path);
    ~Font();

    void DrawString(const std::string& text, float text_size, float x, float y,
                    VkCommandBuffer cmd_buffer, VkRenderPass render_pass, uint_t frame_index);
private:
    Renderer& renderer_;

    std::unique_ptr<Texture> texture_;
    VkSampler sampler_;

    std::unique_ptr<UniformBufferObject<glm::mat4>> orientation_matrix_;

    std::shared_ptr<ShaderState> shader_;

    VkDescriptorSetLayout font_descriptors_layout_;
    std::vector<VkDescriptorSet> font_descriptor_sets_;

    VkPipelineLayout layout_;
    VkPipelineCache cache_;
    VkPipeline pipeline_ = VK_NULL_HANDLE;

    typedef struct map_character {
        uint16_t x, y;
        int8_t  x_offset;
        uint8_t width, height, y_offset, x_advance;
    } map_character;

    typedef struct font_data {
        glm::vec4 positions[128][2];
    } font_data;

    uint32_t max_push_const_size;
    typedef struct push_constants {
        glm::vec4 coordinates;
        uint32_t string_data[28];
    } push_constants;

    std::unique_ptr<UniformBufferObject<Font::font_data>> font_data_ubo_;

    std::unordered_map<int, Font::map_character> char_map_;

    void CreateFontShaders(android_app &android_app_ctx);
    void ParseFontInfo(const char* info_file_path, android_app &android_app_ctx);
    void UpdateFontUBO();
    void CreateSampler();
    void CreateDescriptorSetLayout();
    void CreateDescriptors(Renderer& renderer);
    void CreateFontPipeline(VkRenderPass render_pass);
    void UpdatePipeline(VkRenderPass render_pass);
    void GetPushConstSize();
};

#endif //BENDER_BASE_FONT_H_
