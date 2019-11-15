//
// Created by chingtangyu on 11/11/2019.
//

#import "font.h"
#import <string>
#import <sstream>
#import <istream>

#include <glm/gtc/matrix_transform.hpp>
#include "shader_bindings.h"

namespace {
    int nextValuePair(std::stringstream *stream) {
        std::string pair;
        *stream >> pair;
        u_int32_t pos = pair.find('=');
        std::string value = pair.substr(pos+1);
        int val = std::stoi(value);
        return val;
    }

    void write_vertex(float data, float **ptr) {
        **ptr = data;
        (*ptr)++;
    }
}

void Font::parseFontInfo(const char *info_file_path, android_app *androidAppCtx) {
    AAsset *asset = AAssetManager_open(androidAppCtx->activity->assetManager,
                                       info_file_path, AASSET_MODE_STREAMING);
    if(asset == nullptr) {
        LOGE("Font parseFontInfo(): font info not found [%s]", info_file_path);
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
        std::stringstream lineStream;
        std::getline(istream, line);
        lineStream << line;

        std::string info;
        lineStream >> info;

        if (info == "char") {
            uint32_t charid = nextValuePair(&lineStream);
            char_map[charid].x = nextValuePair(&lineStream);
            char_map[charid].y = nextValuePair(&lineStream);
            char_map[charid].width = nextValuePair(&lineStream);
            char_map[charid].height = nextValuePair(&lineStream);
            char_map[charid].xoffset = nextValuePair(&lineStream);
            char_map[charid].yoffset = nextValuePair(&lineStream);
            char_map[charid].xadvance = nextValuePair(&lineStream);
            nextValuePair(&lineStream);
            nextValuePair(&lineStream);
        }
    }
}

void Font::drawString(const std::string& text, const size_t text_size, float x, float y,
                      VkCommandBuffer commandBuffer, VkRenderPass render_pass, uint_t frame_index) {
    updatePipeline(render_pass);

    float w = (float)texture_->getWidth();
    float h = (float)texture_->getHeight();

    float posx = x;
    float posy = y;

    void *data;
    vkMapMemory(renderer_.getDevice().getDevice(), vertexBufferDeviceMemory_, 0,
                sizeof(float) * text_size * FONT_NUM_QUAD_INDICES, 0, &data);

    float *head = (float*)data;

    for (auto& c : text) {
        Character *char_info = &char_map[c];

        float dimx = (float)(char_info->width);
        float dimy = (float)(char_info->height);

        float us = char_info->x / w;
        float ue = (char_info->x + char_info->width) / w;
        float ts = char_info->y / h;
        float te = (char_info->y + char_info->height) / h;

        float xo = char_info->xoffset;
        float yo = char_info->yoffset;

        write_vertex((posx) + (dimx + xo) / w, &head);
        write_vertex((posy) + (dimy + yo) / h, &head);
        write_vertex(ue, &head);
        write_vertex(te, &head);


        write_vertex((posx) +  (xo) / w, &head);
        write_vertex((posy) + (dimy + yo) / h, &head);
        write_vertex(us, &head);
        write_vertex(te, &head);

        write_vertex((posx) + (xo) / w, &head);
        write_vertex((posy) + (yo) / h, &head);
        write_vertex(us, &head);
        write_vertex(ts, &head);

        write_vertex((posx) + (xo) / w, &head);
        write_vertex((posy) + (yo) / h, &head);
        write_vertex(us, &head);
        write_vertex(ts, &head);

        write_vertex((posx) + (dimx + xo) / w, &head);
        write_vertex((posy) + (yo) / h, &head);
        write_vertex(ue, &head);
        write_vertex(ts, &head);

        write_vertex((posx) + (dimx + xo) / w, &head);
        write_vertex((posy) + (dimy + yo) / h, &head);
        write_vertex(ue, &head);
        write_vertex(te, &head);

        posx += (float)(char_info->xadvance) / w;
    }

    vkUnmapMemory(renderer_.getDevice().getDevice(), vertexBufferDeviceMemory_);

    vkCmdBindPipeline(commandBuffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline_);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuf_, &offset);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout_, 0, 1, &font_descriptor_sets_[frame_index], 0, nullptr);

    vkCmdDraw(commandBuffer,
              text_size * FONT_NUM_QUAD_INDICES,
              1, 0, 0);
}

Font::Font(Renderer& renderer, android_app *androidAppCtx,
           const std::string& font_texture_path, const std::string& font_info_path) : renderer_(renderer){
    texture_ = new Texture(renderer_.getDevice(), androidAppCtx,
                           font_texture_path.c_str(), VK_FORMAT_R8G8B8A8_SRGB);

    createFontShaders(androidAppCtx);
    parseFontInfo(font_info_path.c_str(), androidAppCtx);
    createSampler();
    createDescriptorSetLayout();
    createDescriptors(renderer);

    VkDeviceSize bufferSizeVertex = sizeof(float) * FONT_STRING_SIZE * FONT_NUM_QUAD_INDICES;
    renderer_.getDevice().createBuffer(bufferSizeVertex, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                          vertexBuf_, vertexBufferDeviceMemory_);
}

Font::~Font() {
    vkDestroyBuffer(renderer_.getDevice().getDevice(), vertexBuf_, nullptr);
    vkFreeMemory(renderer_.getDevice().getDevice(), vertexBufferDeviceMemory_, nullptr);

    vkDestroyPipeline(renderer_.getDevice().getDevice(), pipeline_, nullptr);
    vkDestroyPipelineCache(renderer_.getDevice().getDevice(), cache_, nullptr);
    vkDestroyPipelineLayout(renderer_.getDevice().getDevice(), layout_, nullptr);
}

void Font::createFontShaders(android_app *androidAppCtx) {
    VertexFormat vertex_format{
            {
                    VertexElement::float2,
                    VertexElement::float2,
            },
    };
    shader_ = std::make_shared<ShaderState>("sdf", vertex_format, androidAppCtx,
                                            renderer_.getDevice().getDevice());
}

void Font::createSampler() {
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
    CALL_VK(vkCreateSampler(renderer_.getDevice().getDevice(), &sampler_create_info, nullptr,
                            &sampler_));
}

void Font::createDescriptors(Renderer& renderer) {
    std::vector<VkDescriptorSetLayout> layouts(renderer_.getDevice().getDisplayImagesSize(),
                                               font_descriptors_layout_);

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = renderer.getDescriptorPool();
    allocInfo.descriptorSetCount = renderer_.getDevice().getDisplayImagesSize();
    allocInfo.pSetLayouts = layouts.data();

    font_descriptor_sets_.resize(renderer_.getDevice().getDisplayImagesSize());
    CALL_VK(vkAllocateDescriptorSets(renderer_.getDevice().getDevice(), &allocInfo, font_descriptor_sets_.data()));

    for (size_t i = 0; i < renderer_.getDevice().getDisplayImagesSize(); i++) {
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture_->getImageView();
        imageInfo.sampler = sampler_;

        std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = font_descriptor_sets_[i];
        descriptorWrites[0].dstBinding = FONT_FRAG_SAMPLER_BINDING;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(renderer_.getDevice().getDevice(),
                               descriptorWrites.size(), descriptorWrites.data(),
                               0, nullptr);
    }
}

void Font::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = FONT_FRAG_SAMPLER_BINDING;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 1> bindings = { samplerLayoutBinding };

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();

    CALL_VK(vkCreateDescriptorSetLayout(renderer_.getDevice().getDevice(), &layoutInfo, nullptr,
                                        &font_descriptors_layout_));
}

void Font::createFontPipeline(VkRenderPass renderPass) {
    VkViewport viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(renderer_.getDevice().getDisplaySize().width),
            .height = static_cast<float>(renderer_.getDevice().getDisplaySize().height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
    };

    VkRect2D scissor{
            .offset = {0, 0},
            .extent = renderer_.getDevice().getDisplaySize(),
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

    CALL_VK(vkCreatePipelineLayout(renderer_.getDevice().getDevice(), &pipelineLayoutInfo, nullptr,
                                   &layout_))

    VkPipelineCacheCreateInfo pipelineCacheInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext = nullptr,
            .initialDataSize = 0,
            .pInitialData = nullptr,
            .flags = 0,  // reserved, must be 0
    };

    CALL_VK(vkCreatePipelineCache(renderer_.getDevice().getDevice(), &pipelineCacheInfo, nullptr,
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

    shader_->updatePipelineInfo(pipelineInfo);

    CALL_VK(vkCreateGraphicsPipelines(renderer_.getDevice().getDevice(), cache_, 1, &pipelineInfo,
                                      nullptr, &pipeline_));
}

void Font::updatePipeline(VkRenderPass renderPass) {
    if (pipeline_ != VK_NULL_HANDLE)
        return;

    createFontPipeline(renderPass);
}

