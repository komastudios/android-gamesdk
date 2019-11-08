//
// Created by chingtangyu on 11/11/2019.
//

#import "font.h"
#import <string>
#import <sstream>
#import <istream>

#include <glm/gtc/matrix_transform.hpp>
#include "shader_bindings.h"

int nextValuePair(std::stringstream *stream) {
    std::string pair;
    *stream >> pair;
    u_int32_t pos = pair.find('=');
    std::string value = pair.substr(pos+1);
    int val = std::stoi(value);
    return val;
}

void Font::parseFontInfo(const char *info_file_path, android_app *androidAppCtx) {
    AAsset *asset = AAssetManager_open(androidAppCtx->activity->assetManager,
                                       info_file_path, AASSET_MODE_STREAMING);
    assert(asset);

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

void Font::generateText(std::string text, VkCommandBuffer commandBuffer, uint_t frame_index) {
    std::vector<float> vertex_data;
    std::vector<uint16_t> index_data;
    uint32_t index_offset = 0;

    float w = (float)texture_->getWidth();
    float h = (float)texture_->getHeight();

    float posx = 0.0f;
    float posy = 0.0f;

    float scale = 64.0f;

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

        vertex_data.push_back((posx + dimx + xo)/ scale);
        vertex_data.push_back((posy + dimy + yo)/ scale);
        vertex_data.push_back(0.0f);
        vertex_data.push_back(ue);
        vertex_data.push_back(te);

        vertex_data.push_back((posx +  xo) / scale);
        vertex_data.push_back((posy + dimy + yo) / scale);
        vertex_data.push_back(0.0f);
        vertex_data.push_back(us);
        vertex_data.push_back(te);

        vertex_data.push_back((posx + xo) / scale);
        vertex_data.push_back((posy + yo) / scale);
        vertex_data.push_back(0.0f);
        vertex_data.push_back(us);
        vertex_data.push_back(ts);

        vertex_data.push_back((posx + dimx + xo) / scale);
        vertex_data.push_back((posy + yo) / scale);
        vertex_data.push_back(0.0f);
        vertex_data.push_back(ue);
        vertex_data.push_back(ts);

        std::array<uint32_t, 6> letterIndices = {0,1,2,2,3,0};
        for (auto& index : letterIndices)
        {
            index_data.push_back(index_offset + index);
        }
        index_offset += 4;

        posx += (float)(char_info->xadvance);
    }

    void *data;
    vkMapMemory(device_.getDevice(), vertexBufferDeviceMemory_, 0,
                sizeof(vertex_data[0]) * vertex_data.size(), 0, &data);
    memset(data, '\0', sizeof(vertex_data[0]) * vertex_data.size());
    memcpy(data, vertex_data.data(), sizeof(vertex_data[0]) * vertex_data.size());
    vkUnmapMemory(device_.getDevice(), vertexBufferDeviceMemory_);

    vkMapMemory(device_.getDevice(), indexBufferDeviceMemory_, 0,
                sizeof(index_data[0]) * index_data.size(), 0, &data);
    memset(data, '\0', sizeof(index_data[0]) * index_data.size());
    memcpy(data, index_data.data(), sizeof(index_data[0]) * index_data.size());
    vkUnmapMemory(device_.getDevice(), indexBufferDeviceMemory_);

    vkCmdBindPipeline(commandBuffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline_);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuf_, &offset);
    vkCmdBindIndexBuffer(commandBuffer, indexBuf_, offset, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout_, 0, 1, &font_descriptor_sets_[frame_index], 0, nullptr);

    vkCmdDrawIndexed(commandBuffer,
                     index_data.size(),
                     1, 0, 0, 0);
}

Font::Font(BenderKit::Device& device, Renderer& renderer, std::shared_ptr<ShaderState> shaders,
        android_app *androidAppCtx, std::string font_texture_path, std::string font_info_path) : device_(device){

    texture_ = new Texture(device, androidAppCtx, font_texture_path.c_str(), VK_FORMAT_R8G8B8A8_SRGB);
    meshBuffer = new UniformBufferObject<ModelViewProjection>(device_);
    shaders_ = shaders;

    parseFontInfo(font_info_path.c_str(), androidAppCtx);
    createSampler();
    createDescriptorSetLayout();
    createDescriptors(renderer);

    VkDeviceSize bufferSizeVertex = sizeof(float) * 200;
    device_.createBuffer(bufferSizeVertex, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                          vertexBuf_, vertexBufferDeviceMemory_);
    VkDeviceSize bufferSizeIndex = sizeof(u_int16_t) * 60;
    device_.createBuffer(bufferSizeIndex, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                          indexBuf_, indexBufferDeviceMemory_);

    position_ = glm::vec3(-1.65f, -2.0f, 0.0f);
    rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    scale_ = glm::vec3(1.0f, 1.0f, 1.0f);

    view_ = glm::inverse(glm::translate(glm::mat4(1.0f), camera_position_) * glm::mat4(camera_rotation_));
    proj_ = glm::perspective(glm::radians(100.0f),
                            device_.getDisplaySize().width / (float) device_.getDisplaySize().height, 0.1f, 100.0f);


}

Font::~Font() {
    vkDestroyBuffer(device_.getDevice(), vertexBuf_, nullptr);
    vkFreeMemory(device_.getDevice(), vertexBufferDeviceMemory_, nullptr);
    vkDestroyBuffer(device_.getDevice(), indexBuf_, nullptr);
    vkFreeMemory(device_.getDevice(), indexBufferDeviceMemory_, nullptr);

    vkDestroyPipeline(device_.getDevice(), pipeline_, nullptr);
    vkDestroyPipelineCache(device_.getDevice(), cache_, nullptr);
    vkDestroyPipelineLayout(device_.getDevice(), layout_, nullptr);

    delete meshBuffer;
}

void Font::createSampler() {
    const VkSamplerCreateInfo sampler_create_info = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .magFilter = VK_FILTER_NEAREST,
            .minFilter = VK_FILTER_NEAREST,
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
    CALL_VK(vkCreateSampler(device_.getDevice(), &sampler_create_info, nullptr,
                            &sampler_));
}

void Font::createDescriptors(Renderer& renderer) {

    std::vector<VkDescriptorSetLayout> layouts(device_.getDisplayImagesSize(),
                                               font_descriptors_layout_);

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = renderer.getDescriptorPool();
    allocInfo.descriptorSetCount = device_.getDisplayImagesSize();
    allocInfo.pSetLayouts = layouts.data();

    font_descriptor_sets_.resize(device_.getDisplayImagesSize());
    CALL_VK(vkAllocateDescriptorSets(device_.getDevice(), &allocInfo, font_descriptor_sets_.data()));

    for (size_t i = 0; i < device_.getDisplayImagesSize(); i++) {
        VkDescriptorBufferInfo meshBufferInfo = {};
        meshBufferInfo.buffer = meshBuffer->getBuffer(i);
        meshBufferInfo.offset = 0;
        meshBufferInfo.range = sizeof(ModelViewProjection);
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture_->getImageView();
        imageInfo.sampler = sampler_;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = font_descriptor_sets_[i];
        descriptorWrites[0].dstBinding = FONT_VERTEX_UBO_BINDING;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &meshBufferInfo;
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = font_descriptor_sets_[i];
        descriptorWrites[1].dstBinding = FONT_FRAG_SAMPLER_BINDING;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device_.getDevice(), descriptorWrites.size(), descriptorWrites.data(),
                               0, nullptr);
    }
}

void Font::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = FONT_VERTEX_UBO_BINDING;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = FONT_FRAG_SAMPLER_BINDING;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();

    CALL_VK(vkCreateDescriptorSetLayout(device_.getDevice(), &layoutInfo, nullptr,
                                        &font_descriptors_layout_));
}

void Font::createFontPipeline(VkRenderPass renderPass) {
    VkViewport viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(device_.getDisplaySize().width),
            .height = static_cast<float>(device_.getDisplaySize().height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
    };

    VkRect2D scissor{
            .offset = {0, 0},
            .extent = device_.getDisplaySize(),
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
            .blendEnable = VK_FALSE,
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

    CALL_VK(vkCreatePipelineLayout(device_.getDevice(), &pipelineLayoutInfo, nullptr,
                                   &layout_))

    VkPipelineCacheCreateInfo pipelineCacheInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext = nullptr,
            .initialDataSize = 0,
            .pInitialData = nullptr,
            .flags = 0,  // reserved, must be 0
    };

    CALL_VK(vkCreatePipelineCache(device_.getDevice(), &pipelineCacheInfo, nullptr,
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

    shaders_->updatePipelineInfo(pipelineInfo);

    CALL_VK(vkCreateGraphicsPipelines(device_.getDevice(), cache_, 1, &pipelineInfo,
                                      nullptr, &pipeline_));
}

void Font::updatePipeline(VkRenderPass renderPass) {
    if (pipeline_ != VK_NULL_HANDLE)
        return;

    createFontPipeline(renderPass);
}

void Font::update(uint_t frame_index) {
    glm::mat4 model = getTransform();
    glm::mat4 mvp = proj_ * view_ * model;

    meshBuffer->update(frame_index, [&mvp, &model](auto& ubo) {
        ubo.mvp = mvp;
        ubo.model = model;
        ubo.invTranspose = glm::transpose(glm::inverse(model));
    });
}

glm::mat4 Font::getTransform() const {
    glm::mat4 position = glm::translate(glm::mat4(1.0), position_);
    glm::mat4 scale = glm::scale(glm::mat4(1.0), scale_);
    return position * glm::mat4(rotation_) * scale;
}



