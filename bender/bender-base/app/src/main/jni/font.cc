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
    int NextValuePair(std::stringstream *stream) {
        std::string pair;
        *stream >> pair;
        u_int32_t pos = pair.find('=');
        std::string value = pair.substr(pos+1);
        int val = std::stoi(value);
        return val;
    }

    void WriteVertex(float data, float **ptr) {
        **ptr = data;
        (*ptr)++;
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
            char_map_[charid].x = NextValuePair(&line_stream);
            char_map_[charid].y = NextValuePair(&line_stream);
            char_map_[charid].width = NextValuePair(&line_stream);
            char_map_[charid].height = NextValuePair(&line_stream);
            char_map_[charid].x_offset = NextValuePair(&line_stream);
            char_map_[charid].y_offset = NextValuePair(&line_stream);
            char_map_[charid].x_advance = NextValuePair(&line_stream);
            NextValuePair(&line_stream);
            NextValuePair(&line_stream);
        }
    }
}

void Font::DrawString(const std::string& text, float text_size, float x, float y,
                      VkCommandBuffer cmd_buffer, VkRenderPass render_pass, uint_t frame_index) {
    UpdatePipeline(render_pass);

    if((int)frame_index != current_frame_) {
        offset_ = 0;
        current_frame_ = frame_index;
    }

    if(offset_ >= FONT_VERTEX_BUFFER_SIZE) {
        return;
    }

    if(text_size < 0.0f) {
        text_size = 0.0f;
    }

    float resolution_ratio_x_ = (float) renderer_.GetDevice().GetDisplaySize().width
        / renderer_.GetDevice().GetDisplaySizeOriented().width;
    float resolution_ratio_y = (float) renderer_.GetDevice().GetDisplaySize().height
        / renderer_.GetDevice().GetDisplaySizeOriented().height;

    float text_size_x = text_size * resolution_ratio_x_;
    float text_size_y = text_size * resolution_ratio_y;

    float w = (float)texture_->GetWidth();
    float h = (float)texture_->GetHeight();

    float posx = x;
    float posy = y;

    void *data;
    CALL_VK(vkMapMemory(renderer_.GetVulkanDevice(), vertexBufferDeviceMemory_, offset_,
                sizeof(float) * text.size() * FONT_NUM_QUAD_INDICES * FONT_ATTR_COUNT, 0, &data));

    float *head = (float*)data;

    for (auto& c : text) {
        Character *char_info = &char_map_[c];

        float dimx = (float)(char_info->width);
        float dimy = (float)(char_info->height);

        float us = char_info->x / w;
        float ue = (char_info->x + char_info->width) / w;
        float ts = char_info->y / h;
        float te = (char_info->y + char_info->height) / h;

        float xo = char_info->x_offset;
        float yo = char_info->y_offset;

        WriteVertex((posx) + (dimx + xo) / w * text_size_x, &head);
        WriteVertex((posy) + (dimy + yo) / h * text_size_y, &head);
        WriteVertex(ue, &head);
        WriteVertex(te, &head);


        WriteVertex((posx) +  (xo) / w * text_size_x, &head);
        WriteVertex((posy) + (dimy + yo) / h * text_size_y, &head);
        WriteVertex(us, &head);
        WriteVertex(te, &head);

        WriteVertex((posx) + (xo) / w * text_size_x, &head);
        WriteVertex((posy) + (yo) / h * text_size_y, &head);
        WriteVertex(us, &head);
        WriteVertex(ts, &head);

        WriteVertex((posx) + (xo) / w * text_size_x, &head);
        WriteVertex((posy) + (yo) / h * text_size_y, &head);
        WriteVertex(us, &head);
        WriteVertex(ts, &head);

        WriteVertex((posx) + (dimx + xo) / w * text_size_x, &head);
        WriteVertex((posy) + (yo) / h * text_size_y, &head);
        WriteVertex(ue, &head);
        WriteVertex(ts, &head);

        WriteVertex((posx) + (dimx + xo) / w * text_size_x, &head);
        WriteVertex((posy) + (dimy + yo) / h * text_size_y, &head);
        WriteVertex(ue, &head);
        WriteVertex(te, &head);

        posx += (float)(char_info->x_advance) / w * text_size_x;
    }

    orientation_matrix_->Update(frame_index, [this](auto &matrix) {
      auto transform = renderer_.GetDevice().GetPretransformFlag();
      switch (transform) {
          case VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR:
              matrix = glm::rotate(glm::mat4(1.0f),
                                   glm::radians(90.0f),
                                   glm::vec3(0, 0, 1));
          break;
          case VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR:
              matrix = glm::rotate(glm::mat4(1.0f),
                                   glm::radians(270.0f),
                                   glm::vec3(0, 0, 1));
          break;
          default:
              matrix = glm::mat4(1.0f);
          break;
      }
    });

    vkUnmapMemory(renderer_.GetVulkanDevice(), vertexBufferDeviceMemory_);

    vkCmdBindPipeline(cmd_buffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline_);

    vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &vertexBuf_, &offset_);

    vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout_, 0, 1, &font_descriptor_sets_[frame_index], 0, nullptr);

    vkCmdDraw(cmd_buffer,
              text.size() * FONT_NUM_QUAD_INDICES,
              1, 0, 0);
    offset_ += sizeof(float) * text.size() * FONT_NUM_QUAD_INDICES * FONT_ATTR_COUNT;
}

Font::Font(Renderer& renderer, android_app &android_app_ctx,
           const std::string& font_texture_path, const std::string& font_info_path) : renderer_(renderer){
    texture_ = std::make_unique<Texture>(renderer_.GetDevice(), android_app_ctx,
                           font_texture_path, VK_FORMAT_R8G8B8A8_SRGB);

    orientation_matrix_ = std::make_unique<UniformBufferObject<glm::mat4>>(renderer_.GetDevice());

    CreateFontShaders(android_app_ctx);
    ParseFontInfo(font_info_path.c_str(), android_app_ctx);
    CreateSampler();
    CreateDescriptorSetLayout();
    CreateDescriptors(renderer);

    VkDeviceSize bufferSizeVertex = FONT_VERTEX_BUFFER_SIZE;
    renderer_.GetDevice().CreateBuffer(bufferSizeVertex, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                          vertexBuf_, vertexBufferDeviceMemory_);
    offset_ = 0;
    current_frame_ = -1;
}

Font::~Font() {
    vkDestroyBuffer(renderer_.GetVulkanDevice(), vertexBuf_, nullptr);
    vkFreeMemory(renderer_.GetVulkanDevice(), vertexBufferDeviceMemory_, nullptr);

    vkDestroyPipeline(renderer_.GetVulkanDevice(), pipeline_, nullptr);
    vkDestroyPipelineCache(renderer_.GetVulkanDevice(), cache_, nullptr);
    vkDestroyPipelineLayout(renderer_.GetVulkanDevice(), layout_, nullptr);

    vkDestroySampler(renderer_.GetVulkanDevice(), sampler_, nullptr);
    vkDestroyDescriptorSetLayout(renderer_.GetVulkanDevice(), font_descriptors_layout_, nullptr);
}

void Font::CreateFontShaders(android_app &android_app_ctx) {
    benderkit::VertexFormat vertex_format{
            {
                    benderkit::VertexElement::float2,
                    benderkit::VertexElement::float2,
            },
    };
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
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture_->GetImageView();
        imageInfo.sampler = sampler_;

        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = orientation_matrix_->GetBuffer(i);
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(glm::mat4);

        std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = font_descriptor_sets_[i];
        descriptorWrites[0].dstBinding = FONT_FRAG_SAMPLER_BINDING;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pImageInfo = &imageInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = font_descriptor_sets_[i];
        descriptorWrites[1].dstBinding = FONT_VERTEX_UBO_BINDING;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(renderer_.GetVulkanDevice(),
                               descriptorWrites.size(), descriptorWrites.data(),
                               0, nullptr);
    }
}

void Font::CreateDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = FONT_FRAG_SAMPLER_BINDING;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding orientationMatrixLayoutBinding = {};
    orientationMatrixLayoutBinding.binding = FONT_VERTEX_UBO_BINDING;
    orientationMatrixLayoutBinding.descriptorCount = 1;
    orientationMatrixLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    orientationMatrixLayoutBinding.pImmutableSamplers = nullptr;
    orientationMatrixLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { samplerLayoutBinding, orientationMatrixLayoutBinding };

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();

    CALL_VK(vkCreateDescriptorSetLayout(renderer_.GetVulkanDevice(), &layoutInfo, nullptr,
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
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS,
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
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .blendEnable = VK_TRUE,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &attachmentStates,
            .flags = 0,
    };

    std::array<VkDescriptorSetLayout, 1> layouts;

    layouts[FONT_BINDING_SET] = font_descriptors_layout_;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .setLayoutCount = layouts.size(),
            .pSetLayouts = layouts.data(),
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
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

