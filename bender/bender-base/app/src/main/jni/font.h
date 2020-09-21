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
#import "push_consts_app.h"

#import <unordered_map>

const std::string kFontSDFPath = "textures/font_sdf.png";
const std::string kFontInfoPath = "textures/font_sdf.fnt";
constexpr size_t kFontNumQuadIndices = 6;

class Font {
    // Stateless font rendering using SDF fonts.
    // To generate SDF fonts, please refer to
    // https://github.com/libgdx/libgdx/wiki/Distance-field-fonts
public:
    Font(Renderer& renderer, android_app& androidAppCtx,
         const std::string& font_texture_path, const std::string& font_info_path);
    ~Font();

    void DrawString(VkCommandBuffer cmd_buffer, VkRenderPass render_pass, uint_t frame_index,
                    const std::string& text, float text_size, float x, float y, glm::vec4 color = glm::vec4(1.0));
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

    struct map_character {
        uint16_t x, y;
        int8_t  x_offset;
        uint8_t width, height, y_offset, x_advance;
    } ;

    struct font_data {
        /*
         * Currently we are using a character set of 97 characters.
         * The ascii value of the set ranges from 0 to 126
         * So having a size of 128 should be enough to hold them without another layer of mapping
         */
        glm::vec4 positions[128][2];
    };

    uint32_t max_push_const_size;

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
