//
// Created by chingtangyu on 11/11/2019.
//

#import "font.h"
#include "shader_bindings.h"

#include <glm/ext/matrix_transform.hpp>

#import <string>
#import <sstream>
#import <istream>

namespace {
    uint32_t NextValuePair(std::stringstream *stream) {
        std::string pair;
        *stream >> pair;
        u_int32_t pos = pair.find('=');
        std::string value = pair.substr(pos+1);
        uint32_t val = static_cast<uint32_t>(std::stoi(value));
        return val;
    }
    void PackByteToUint(uint* text_data, const char& text, const int& i) {
        /*
         * Packing 8-bit unsigned integer to 32-bit unsigned integer
         * The idea is to fill up 32-bit uint with 4 8-bit short uint
         *
         *      8-bit         8-bit         8-bit          8-bit
         * |-------------|-------------|-------------|-------------|
         *
         */
        text_data[i / 4] |= static_cast<uint32_t>(static_cast<uint8_t>(text)) << 8 * (3 - i % 4);
    }
}

void Font::ParseFontInfo(const char *info_file_path, android_app &android_app_ctx) {
    AAsset *asset = AAssetManager_open(android_app_ctx.activity->assetManager,
                                       info_file_path, AASSET_MODE_STREAMING);
    if(asset == nullptr) {
        LOGE("Font ParseFontInfo(): font info not found [%s]", info_file_path);
        return;
    }

    size_t size = AAsset_getLength(asset);
    assert(size > 0);

    void *file_data = malloc(size);
    AAsset_read(asset, file_data, size);
    AAsset_close(asset);

    std::stringbuf sbuf((const char*)file_data);
    std::istream istream(&sbuf);
    assert(istream.good());

    while(!istream.eof()) {
        std::string line;
        std::stringstream line_stream;
        std::getline(istream, line);
        line_stream << line;

        std::string info;
        line_stream >> info;

        if (info == "char") {
            uint32_t charid = NextValuePair(&line_stream);
            char_map_[charid].x = static_cast<uint16_t>(NextValuePair(&line_stream));
            char_map_[charid].y = static_cast<uint16_t>(NextValuePair(&line_stream));
            char_map_[charid].width = static_cast<uint8_t>(NextValuePair(&line_stream));
            char_map_[charid].height = static_cast<uint8_t>(NextValuePair(&line_stream));
            char_map_[charid].x_offset = static_cast<int8_t>(NextValuePair(&line_stream));
            char_map_[charid].y_offset = static_cast<uint8_t>(NextValuePair(&line_stream));
            char_map_[charid].x_advance = static_cast<uint8_t>(NextValuePair(&line_stream));
            NextValuePair(&line_stream);
            NextValuePair(&line_stream);
        }
    }
}

void Font::UpdateFontUBO() {
    float w = static_cast<float>(texture_->GetWidth());
    float h = static_cast<float>(texture_->GetHeight());
    for(int i = 0; i < renderer_.GetDevice().GetSwapchainLength(); ++i) {
            font_data_ubo_->Update(i, [this, w, h](Font::font_data &font_data) {
                for(auto &char_data : char_map_) {
                    int index = char_data.first;
                    map_character current_char = char_data.second;
                    float pos_x = static_cast<float>(current_char.x) / w;
                    float pos_y = static_cast<float>(current_char.y) / h;
                    float x_off = static_cast<float>(current_char.x_offset) / w;
                    float y_off = static_cast<float>(current_char.y_offset) / h;
                    float char_width = static_cast<float>(current_char.width) / w;
                    float char_height = static_cast<float>(current_char.height) / h;
                    float x_adv = static_cast<float>(current_char.x_advance) / w;
                    font_data.positions[index][0] = glm::vec4(pos_x, pos_y, char_width, char_height);
                    font_data.positions[index][1] = glm::vec4(x_off, y_off, x_adv, 0.0f);
                }
            });
    }
}

void Font::DrawString(VkCommandBuffer cmd_buffer, VkRenderPass render_pass, uint_t frame_index,
                      const std::string& text, float text_size, float x, float y, glm::vec4 color) {
    UpdatePipeline(render_pass);

    assert(text_size > 0.0f && "text_size must be greater than zero");

    if(text.size() == 0) {
        assert(text.size() != 0 && "Unexpected empty string.");
        return;
    }

    FontPushConstants text_push_consts= {};

    float resolution_ratio_x = (float) renderer_.GetDevice().GetDisplaySize().width
        / renderer_.GetDevice().GetDisplaySizeOriented().width;
    float resolution_ratio_y = (float) renderer_.GetDevice().GetDisplaySize().height
        / renderer_.GetDevice().GetDisplaySizeOriented().height;

    float text_size_x = text_size * resolution_ratio_x;
    float text_size_y = text_size * resolution_ratio_y;
    text_push_consts.coordinates = glm::vec4(text_size_x, text_size_y, x, y);
    text_push_consts.colors = color;

    for(int i = 0; i < text.size(); ++i) {
        PackByteToUint(text_push_consts.text_data, text[i], i);
    }

    orientation_matrix_->Update(frame_index, [this](auto &matrix) {
      auto transform = renderer_.GetDevice().GetPretransformFlag();
      switch (transform) {
          case VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR:
              matrix = glm::rotate(glm::mat4(1.0f),
                                   glm::radians(90.0f),
                                   glm::vec3(0.0f, 0.0f, 1.0f));
          break;
          case VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR:
              matrix = glm::rotate(glm::mat4(1.0f),
                                   glm::radians(270.0f),
                                   glm::vec3(0.0f, 0.0f, 1.0f));
          break;
          default:
              matrix = glm::mat4(1.0f);
          break;
      }
    });

    vkCmdBindPipeline(cmd_buffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline_);

    vkCmdPushConstants(cmd_buffer,
                       layout_,
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(text_push_consts),
                       &text_push_consts);

    vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout_, 0, 1, &font_descriptor_sets_[frame_index], 0, nullptr);

    vkCmdDraw(cmd_buffer,
              text.size() * kFontNumQuadIndices,
              1, 0, 0);
}

Font::Font(Renderer& renderer, android_app &android_app_ctx,
           const std::string& font_texture_path, const std::string& font_info_path) : renderer_(renderer){
    texture_ = std::make_unique<Texture>(renderer, android_app_ctx,
                           font_texture_path, VK_FORMAT_R8G8B8A8_SRGB);

    orientation_matrix_ = std::make_unique<UniformBufferObject<glm::mat4>>(renderer_.GetDevice());
    font_data_ubo_ = std::make_unique<UniformBufferObject<font_data>>(renderer_.GetDevice());

    GetPushConstSize();
    CreateFontShaders(android_app_ctx);
    ParseFontInfo(font_info_path.c_str(), android_app_ctx);
    UpdateFontUBO();
    CreateSampler();
    CreateDescriptorSetLayout();
    CreateDescriptors(renderer);
}

Font::~Font() {

    vkDestroyPipeline(renderer_.GetVulkanDevice(), pipeline_, nullptr);
    vkDestroyPipelineCache(renderer_.GetVulkanDevice(), cache_, nullptr);
    vkDestroyPipelineLayout(renderer_.GetVulkanDevice(), layout_, nullptr);

    vkDestroySampler(renderer_.GetVulkanDevice(), sampler_, nullptr);
    vkDestroyDescriptorSetLayout(renderer_.GetVulkanDevice(), font_descriptors_layout_, nullptr);
}

void Font::CreateFontShaders(android_app &android_app_ctx) {
    benderkit::VertexFormat vertex_format{};
    shader_ = std::make_shared<ShaderState>("sdf", vertex_format, android_app_ctx,
                                            renderer_.GetVulkanDevice());
}

void Font::CreateSampler() {
    const VkSamplerCreateInfo sampler_create_info = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
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
    CALL_VK(vkCreateSampler(renderer_.GetVulkanDevice(), &sampler_create_info, nullptr,
                            &sampler_));
}

void Font::CreateDescriptors(Renderer& renderer) {
    std::vector<VkDescriptorSetLayout> layouts(renderer_.GetDevice().GetDisplayImages().size(),
                                               font_descriptors_layout_);

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = renderer.GetDescriptorPool();
    allocInfo.descriptorSetCount = renderer_.GetDevice().GetDisplayImages().size();
    allocInfo.pSetLayouts = layouts.data();

    font_descriptor_sets_.resize(renderer_.GetDevice().GetDisplayImages().size());
    CALL_VK(vkAllocateDescriptorSets(renderer_.GetVulkanDevice(), &allocInfo, font_descriptor_sets_.data()));

    for (size_t i = 0; i < renderer_.GetDevice().GetDisplayImages().size(); i++) {
        VkDescriptorImageInfo image_info {
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .imageView = texture_->GetImageView(),
                .sampler = sampler_
        };
        VkDescriptorBufferInfo matrix_buffer_info {
                .buffer = orientation_matrix_->GetBuffer(i),
                .offset = 0,
                .range = sizeof(glm::mat4)
        };
        VkDescriptorBufferInfo font_buffer_info {
                .buffer = font_data_ubo_->GetBuffer(i),
                .offset = 0,
                .range = sizeof(Font::font_data)
        };

        VkWriteDescriptorSet write_descriptors[3];

        write_descriptors[0] = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = font_descriptor_sets_[i],
                .dstBinding = FONT_FRAG_SAMPLER_BINDING,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .pImageInfo = &image_info,
        };

        write_descriptors[1] = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = font_descriptor_sets_[i],
                .dstBinding = FONT_VERTEX_MATRIX_BINDING,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .pBufferInfo = &matrix_buffer_info,
        };

        write_descriptors[2] = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = font_descriptor_sets_[i],
                .dstBinding = FONT_VERTEX_UBO_BINDING,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .pBufferInfo = &font_buffer_info,
        };


        vkUpdateDescriptorSets(renderer_.GetVulkanDevice(),
                               3, write_descriptors,
                               0, nullptr);
    }
}

void Font::CreateDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding sampler_layout_binding = {
        .binding = FONT_FRAG_SAMPLER_BINDING,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImmutableSamplers = nullptr,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    };
    VkDescriptorSetLayoutBinding orientation_matrix_layout_binding = {
        .binding = FONT_VERTEX_MATRIX_BINDING,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImmutableSamplers = nullptr,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };

    VkDescriptorSetLayoutBinding font_ubo_layout_binding = {
        .binding = FONT_VERTEX_UBO_BINDING,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImmutableSamplers = nullptr,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };

    std::array<VkDescriptorSetLayoutBinding, 3> bindings = {sampler_layout_binding,
                                                            orientation_matrix_layout_binding,
                                                            font_ubo_layout_binding, };

    VkDescriptorSetLayoutCreateInfo layout_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = bindings.size(),
            .pBindings = bindings.data(),
    };

    CALL_VK(vkCreateDescriptorSetLayout(renderer_.GetVulkanDevice(), &layout_info, nullptr,
                                        &font_descriptors_layout_));
}

void Font::CreateFontPipeline(VkRenderPass renderPass) {
    VkViewport viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(renderer_.GetDevice().GetDisplaySize().width),
            .height = static_cast<float>(renderer_.GetDevice().GetDisplaySize().height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
    };

    VkRect2D scissor{
            .offset = {0, 0},
            .extent = renderer_.GetDevice().GetDisplaySize(),
    };

    VkPipelineViewportStateCreateInfo pipelineViewportState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissor,
    };

    VkPipelineDepthStencilStateCreateInfo depthStencilState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_FALSE,
            .depthBoundsTestEnable = VK_FALSE,
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f,
            .stencilTestEnable = VK_FALSE,
    };

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .lineWidth = 1.0f,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
    };

    // Multisample anti-aliasing setup
    VkPipelineMultisampleStateCreateInfo pipelineMultisampleState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 0,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
    };

    // Describes how to blend pixels from past framebuffers to current framebuffers
    // Could be used for transparency or cool screen-space effects
    VkPipelineColorBlendAttachmentState attachmentStates{
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .blendEnable = VK_TRUE,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_NO_OP,
            .attachmentCount = 1,
            .pAttachments = &attachmentStates,
            .flags = 0,
    };

    VkPushConstantRange push_const_range = {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = max_push_const_size,
    };

    VkDescriptorSetLayout layout = font_descriptors_layout_;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {

            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .setLayoutCount = 1,
            .pSetLayouts = &layout,
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &push_const_range,
    };

    CALL_VK(vkCreatePipelineLayout(renderer_.GetVulkanDevice(), &pipelineLayoutInfo, nullptr,
                                   &layout_))

    VkPipelineCacheCreateInfo pipelineCacheInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext = nullptr,
            .initialDataSize = 0,
            .pInitialData = nullptr,
            .flags = 0,  // reserved, must be 0
    };

    CALL_VK(vkCreatePipelineCache(renderer_.GetVulkanDevice(), &pipelineCacheInfo, nullptr,
                                  &cache_));

    VkGraphicsPipelineCreateInfo pipelineInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount = 2,
            .pTessellationState = nullptr,
            .pViewportState = &pipelineViewportState,
            .pRasterizationState = &pipelineRasterizationState,
            .pMultisampleState = &pipelineMultisampleState,
            .pDepthStencilState = &depthStencilState,
            .pColorBlendState = &colorBlendInfo,
            .pDynamicState = nullptr,
            .layout = layout_,
            .renderPass = renderPass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0,
    };

    shader_->FillPipelineInfo(&pipelineInfo);

    CALL_VK(vkCreateGraphicsPipelines(renderer_.GetVulkanDevice(), cache_, 1, &pipelineInfo,
                                      nullptr, &pipeline_));
}

void Font::UpdatePipeline(VkRenderPass renderPass) {
    if (pipeline_ != VK_NULL_HANDLE)
        return;

    CreateFontPipeline(renderPass);
}

void Font::GetPushConstSize() {
    VkPhysicalDeviceProperties device_prop;
    vkGetPhysicalDeviceProperties(renderer_.GetDevice().GetPhysicalDevice(), &device_prop);
    max_push_const_size = device_prop.limits.maxPushConstantsSize;
}
