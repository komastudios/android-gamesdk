//
// Created by mattkw on 10/31/2019.
//

#include <glm/gtc/matrix_transform.hpp>
#include "mesh.h"

Mesh::Mesh(BenderKit::Device *device, std::vector<float> vertexData, std::vector<uint16_t> indexData,
           VkDescriptorSetLayout *descriptorSetLayout, ShaderState *shaderState, VkRenderPass* renderPass) {
    device_ = device;
    geometry_ = new Geometry(device, vertexData, indexData);
    position_ = glm::vec3(0.0f, 0.0f, 0.0f);
    rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    scale_ = glm::vec3(1.0f, 1.0f, 1.0f);

    createMeshPipeline(descriptorSetLayout, shaderState, renderPass);
}

Mesh::~Mesh() {
    delete geometry_;
    vkDestroyPipeline(device_->getDevice(), pipeline_, nullptr);
    vkDestroyPipelineCache(device_->getDevice(), cache_, nullptr);
    vkDestroyPipelineLayout(device_->getDevice(), layout_, nullptr);
}

void Mesh::createMeshPipeline(VkDescriptorSetLayout *descriptorSetLayout, ShaderState *shaderState,
        VkRenderPass *renderPass) {
    VkViewport viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(device_->getDisplaySize().width),
            .height = static_cast<float>(device_->getDisplaySize().height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
    };

    VkRect2D scissor{
            .offset = {0, 0},
            .extent = device_->getDisplaySize(),
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
            .depthTestEnable = VK_TRUE,
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

    // Describes the layout of things such as uniforms
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .setLayoutCount = 1,
            .pSetLayouts = descriptorSetLayout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
    };
    CALL_VK(vkCreatePipelineLayout(device_->getDevice(), &pipelineLayoutInfo, nullptr,
                                   &layout_))

    VkPipelineCacheCreateInfo pipelineCacheInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext = nullptr,
            .initialDataSize = 0,
            .pInitialData = nullptr,
            .flags = 0,  // reserved, must be 0
    };

    CALL_VK(vkCreatePipelineCache(device_->getDevice(), &pipelineCacheInfo, nullptr,
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
            .renderPass = *renderPass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0,
    };

    shaderState->updatePipelineInfo(pipelineInfo);

    CALL_VK(vkCreateGraphicsPipelines(device_->getDevice(), cache_, 1, &pipelineInfo,
                                      nullptr, &pipeline_));
}

void Mesh::submitDraw(VkCommandBuffer commandBuffer, VkDescriptorSet& descriptorSet) const {
    vkCmdBindPipeline(commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline_);

    vkCmdBindPipeline(commandBuffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline_);

    geometry_->bind(commandBuffer);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout_, 0, 1, &descriptorSet, 0, nullptr);
    vkCmdDrawIndexed(commandBuffer,
                     static_cast<u_int32_t>(geometry_->getIndexCount()),
                     1, 0, 0, 0);
}

void Mesh::translate(glm::vec3 offset){
    position_ += offset;
}

void Mesh::rotate(glm::vec3 axis, float angle){
    rotation_ *= glm::angleAxis(glm::radians(angle), angle);
    rotation_ = glm::normalize(rotation_);
}

void Mesh::scale(glm::vec3 scaling){
    scale_ *= scaling;
}

void Mesh::setPosition(glm::vec3 position){
    position_ = position;
}
void Mesh::setRotation(glm::vec3 axis, float angle){
    rotation_ = glm::angleAxis(glm::radians(angle), axis);
}
void Mesh::setScale(glm::vec3 scale){
    scale_ = scale;
}

glm::mat4 Mesh::getTransform() {
    glm::mat4 position = glm::translate(glm::mat4(1.0), position_);
    glm::mat4 scale = glm::scale(glm::mat4(1.0), scale_);
    return position * glm::mat4(rotation_) * scale;
}