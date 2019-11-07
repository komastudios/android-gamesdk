//
// Created by mattkw on 10/31/2019.
//

#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"
#include "shader_bindings.h"

Mesh::Mesh(android_app *app, BenderKit::Device *device, Renderer *renderer,
        const std::vector<float>& vertexData, const std::vector<uint16_t>& indexData,
        std::shared_ptr<ShaderState> shaders) {
  device_ = device;
  renderer_ = renderer;
  geometry_ = std::make_shared<Geometry>(device, vertexData, indexData);
  std::vector<const char *> texFiles = {"textures/sample_texture.png"};
  material_ = std::make_shared<Material>(app, device, texFiles);
  shaders_ = shaders;
  mvp_buffer_ = new UniformBufferObject<ModelViewProjection>(*device);

  position_ = glm::vec3(0.0f, 0.0f, 0.0f);
  rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  scale_ = glm::vec3(1.0f, 1.0f, 1.0f);

  createDescriptorSetLayout();
  createDescriptorSets();
}

Mesh::~Mesh() {
  vkDestroyPipeline(device_->getDevice(), pipeline_, nullptr);
  vkDestroyPipelineCache(device_->getDevice(), cache_, nullptr);
  vkDestroyPipelineLayout(device_->getDevice(), layout_, nullptr);

  vkDestroyDescriptorSetLayout(device_->getDevice(), descriptor_set_layout_, nullptr);

  delete mvp_buffer_;
}

void Mesh::createMeshPipeline(VkRenderPass renderPass) {
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

  VkDescriptorSetLayout layouts[] = { descriptor_set_layout_ };
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{
          .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
          .pNext = nullptr,
          .setLayoutCount = 1,
          .pSetLayouts = layouts,
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
          .renderPass = renderPass,
          .subpass = 0,
          .basePipelineHandle = VK_NULL_HANDLE,
          .basePipelineIndex = 0,
  };

  shaders_->updatePipelineInfo(pipelineInfo);

  CALL_VK(vkCreateGraphicsPipelines(device_->getDevice(), cache_, 1, &pipelineInfo,
                                    nullptr, &pipeline_));
}

void Mesh::createDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding uboLayoutBinding = {};
  uboLayoutBinding.binding = VERTEX_BINDING_MODEL_VIEW_PROJECTION;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.pImmutableSamplers = nullptr;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
  samplerLayoutBinding.binding = FRAGMENT_BINDING_SAMPLER;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding lightBlockLayoutBinding = {};
  lightBlockLayoutBinding.binding = FRAGMENT_BINDING_LIGHTS;
  lightBlockLayoutBinding.descriptorCount = 1;
  lightBlockLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  lightBlockLayoutBinding.pImmutableSamplers = nullptr;
  lightBlockLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 3> bindings = {uboLayoutBinding, samplerLayoutBinding,
                                                          lightBlockLayoutBinding};
  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  CALL_VK(vkCreateDescriptorSetLayout(device_->getDevice(), &layoutInfo, nullptr, &descriptor_set_layout_));
}

void Mesh::createDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(device_->getDisplayImagesSize(), descriptor_set_layout_);
  VkDescriptorSetAllocateInfo allocInfo = {
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
          .descriptorPool = *renderer_->getDescriptorPool(),
          .descriptorSetCount = static_cast<uint32_t>(device_->getDisplayImagesSize()),
          .pSetLayouts = layouts.data(),
  };

  descriptor_sets_.resize(device_->getDisplayImagesSize());
  CALL_VK(vkAllocateDescriptorSets(device_->getDevice(), &allocInfo, descriptor_sets_.data()));

  for (size_t i = 0; i < device_->getDisplayImagesSize(); i++) {
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = mvp_buffer_->getBuffer(i);
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(ModelViewProjection);

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = material_->getTexture(0)->getImageView();
    imageInfo.sampler = *material_->getSampler(0);

    VkDescriptorBufferInfo lightBlockInfo = {};
    lightBlockInfo.buffer = renderer_->getLightBuffer()->getBuffer(i);
    lightBlockInfo.offset = 0;
    lightBlockInfo.range = sizeof(LightBlock);

    std::array<VkWriteDescriptorSet, 3> descriptorWrites = {};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptor_sets_[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptor_sets_[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[2].dstSet = descriptor_sets_[i];
    descriptorWrites[2].dstBinding = 2;
    descriptorWrites[2].dstArrayElement = 0;
    descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[2].descriptorCount = 1;
    descriptorWrites[2].pBufferInfo = &lightBlockInfo;

    vkUpdateDescriptorSets(device_->getDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
  }
}

void Mesh::updatePipeline(VkRenderPass renderPass) {
  if (pipeline_ != VK_NULL_HANDLE)
    return;

  createMeshPipeline(renderPass);
}

void Mesh::submitDraw() const {
  VkCommandBuffer commandBuffer = renderer_->getCurrentCommandBuffer();
  vkCmdBindPipeline(commandBuffer,
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipeline_);

  vkCmdBindPipeline(commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline_);

  geometry_->bind(commandBuffer);

  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          layout_, 0, 1, &descriptor_sets_[renderer_->getCurrentFrame()], 0, nullptr);

  vkCmdDrawIndexed(commandBuffer,
                   geometry_->getIndexCount(),
                   1, 0, 0, 0);
}

void Mesh::translate(glm::vec3 offset) {
  position_ += offset;
}

void Mesh::rotate(glm::vec3 axis, float angle) {
  rotation_ *= glm::angleAxis(glm::radians(angle), axis);
  rotation_ = glm::normalize(rotation_);
}

void Mesh::scale(glm::vec3 scaling) {
  scale_ *= scaling;
}

void Mesh::setPosition(glm::vec3 position) {
  position_ = position;
}

void Mesh::setRotation(glm::vec3 axis, float angle) {
  rotation_ = glm::angleAxis(glm::radians(angle), axis);
}

void Mesh::setScale(glm::vec3 scale) {
  scale_ = scale;
}

glm::vec3 Mesh::getPosition() const {
  return position_;
}

glm::quat Mesh::getRotation() const {
  return rotation_;
}

glm::vec3 Mesh::getScale() const {
  return scale_;
}

void Mesh::updateMvpBuffer(glm::vec3 camera) {
  glm::mat4 position = glm::translate(glm::mat4(1.0), position_);
  glm::mat4 scale = glm::scale(glm::mat4(1.0), scale_);

  glm::mat4 model = position * glm::mat4(rotation_) * scale;
  glm::mat4 view = glm::lookAt(camera, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 proj = glm::perspective(glm::radians(100.0f),
                                    device_->getDisplaySize().width / (float) device_->getDisplaySize().height, 0.1f, 100.0f);
  proj[1][1] *= -1;

  glm::mat4 mvp = proj * view * model;

  mvp_buffer_->update(renderer_->getCurrentFrame(), [&mvp, &model](auto& ubo) {
      ubo.mvp = mvp;
      ubo.model = model;
      ubo.invTranspose = glm::transpose(glm::inverse(model));
  });
}