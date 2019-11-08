//
// Created by mattkw on 10/31/2019.
//

#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"
#include "shader_bindings.h"

VkDescriptorPool Mesh::mesh_descriptor_pool_ = VK_NULL_HANDLE;
VkDescriptorPool Mesh::material_descriptor_pool_ = VK_NULL_HANDLE;

void Mesh::createPools(BenderKit::Device& device) {
  if (material_descriptor_pool_ == VK_NULL_HANDLE) {
    std::array<VkDescriptorPoolSize, 2> poolSizes = {};

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[0].descriptorCount = device.getDisplayImagesSize();
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = device.getDisplayImagesSize();

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = device.getDisplayImagesSize();     // maxSets will need to take into account
                                                          // the max number of materials in a scene

    CALL_VK(vkCreateDescriptorPool(device.getDevice(), &poolInfo, nullptr, &material_descriptor_pool_));
  }

  if (mesh_descriptor_pool_ == VK_NULL_HANDLE) {
    std::array<VkDescriptorPoolSize, 1> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = device.getDisplayImagesSize();

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = device.getDisplayImagesSize();     // maxSets will need to take into account
                                                          // the max number of meshes in a scene

    CALL_VK(vkCreateDescriptorPool(device.getDevice(), &poolInfo, nullptr, &mesh_descriptor_pool_));
  }
}

void Mesh::destroyPools(BenderKit::Device& device) {
  vkDestroyDescriptorPool(device.getDevice(), material_descriptor_pool_, nullptr);
  vkDestroyDescriptorPool(device.getDevice(), mesh_descriptor_pool_, nullptr);
}

Mesh::Mesh(Renderer *renderer, const std::vector<float>& vertexData,
        const std::vector<uint16_t>& indexData, std::shared_ptr<ShaderState> shaders) : device_(renderer->getDevice()) {
  renderer_ = renderer;

  geometry_ = std::make_shared<Geometry>(device_, vertexData, indexData);
  shaders_ = shaders;

  position_ = glm::vec3(0.0f, 0.0f, 0.0f);
  rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  scale_ = glm::vec3(1.0f, 1.0f, 1.0f);

  createDescriptorSetLayout();

  meshBuffer = new UniformBufferObject<ModelViewProjection>(device_);
}

Mesh::~Mesh() {
  vkDestroyPipeline(device_.getDevice(), pipeline_, nullptr);
  vkDestroyPipelineCache(device_.getDevice(), cache_, nullptr);
  vkDestroyPipelineLayout(device_.getDevice(), layout_, nullptr);

  delete meshBuffer;
}

void Mesh::createDescriptors(Texture* texture) {
  // TODO move this descriptor set to a Material class
  {
    std::vector<VkDescriptorSetLayout> layouts(device_.getDisplayImagesSize(),
                                               material_descriptors_layout_);

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = Mesh::getMaterialDescriptorPool();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(device_.getDisplayImagesSize());
    allocInfo.pSetLayouts = layouts.data();

    material_descriptor_sets_.resize(device_.getDisplayImagesSize());
    CALL_VK(vkAllocateDescriptorSets(device_.getDevice(), &allocInfo, material_descriptor_sets_.data()));

    for (size_t i = 0; i < device_.getDisplayImagesSize(); i++) {
      VkDescriptorImageInfo imageInfo = {};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfo.imageView = texture->getImageView();
      imageInfo.sampler = VK_NULL_HANDLE;

      std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};

      descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[0].dstSet = material_descriptor_sets_[i];
      descriptorWrites[0].dstBinding = FRAGMENT_BINDING_SAMPLER;
      descriptorWrites[0].dstArrayElement = 0;
      descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptorWrites[0].descriptorCount = 1;
      descriptorWrites[0].pImageInfo = &imageInfo;

      vkUpdateDescriptorSets(device_.getDevice(), descriptorWrites.size(), descriptorWrites.data(),
                             0, nullptr);
    }
  }

  {
    std::vector<VkDescriptorSetLayout> layouts(device_.getDisplayImagesSize(),
                                               mesh_descriptors_layout_);

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = Mesh::getMeshDescriptorPool();
    allocInfo.descriptorSetCount = device_.getDisplayImagesSize();
    allocInfo.pSetLayouts = layouts.data();

    model_view_projection_descriptor_sets_.resize(device_.getDisplayImagesSize());
    CALL_VK(vkAllocateDescriptorSets(device_.getDevice(), &allocInfo, model_view_projection_descriptor_sets_.data()));

    for (size_t i = 0; i < device_.getDisplayImagesSize(); i++) {
      VkDescriptorBufferInfo bufferInfo = {};
      bufferInfo.buffer = meshBuffer->getBuffer(i);
      bufferInfo.offset = 0;
      bufferInfo.range = sizeof(ModelViewProjection);

      std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};

      descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[0].dstSet = model_view_projection_descriptor_sets_[i];
      descriptorWrites[0].dstBinding = 0;
      descriptorWrites[0].dstArrayElement = 0;
      descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrites[0].descriptorCount = 1;
      descriptorWrites[0].pBufferInfo = &bufferInfo;

      vkUpdateDescriptorSets(device_.getDevice(), descriptorWrites.size(), descriptorWrites.data(),
                             0, nullptr);
    }
  }
}

void Mesh::createMeshPipeline(VkRenderPass renderPass) {
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

  std::array<VkDescriptorSetLayout, 3> layouts;

  layouts[BINDING_SET_MESH] = mesh_descriptors_layout_;
  layouts[BINDING_SET_MATERIAL] = material_descriptors_layout_;
  layouts[BINDING_SET_LIGHTS] = renderer_->getLightsDescriptorSetLayout();

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

void Mesh::createDescriptorSetLayout() {
  {
    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = FRAGMENT_BINDING_SAMPLER;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 1> bindings = {samplerLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    CALL_VK(vkCreateDescriptorSetLayout(device_.getDevice(), &layoutInfo, nullptr,
                                        &material_descriptors_layout_));
  }
  {
    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = VERTEX_BINDING_MODEL_VIEW_PROJECTION;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    std::array<VkDescriptorSetLayoutBinding, 1> bindings = { uboLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();

    CALL_VK(vkCreateDescriptorSetLayout(device_.getDevice(), &layoutInfo, nullptr,
                                        &mesh_descriptors_layout_));
  }
}

void Mesh::updatePipeline(VkRenderPass renderPass) {
  if (pipeline_ != VK_NULL_HANDLE)
    return;

  createMeshPipeline(renderPass);
}

void Mesh::update(uint_t frame_index, glm::vec3 camera, glm::mat4 view, glm::mat4 proj) {
  glm::mat4 model = getTransform();
  glm::mat4 mvp = proj * view * model;

  meshBuffer->update(frame_index, [&mvp, &model](auto& ubo) {
    ubo.mvp = mvp;
    ubo.model = model;
    ubo.invTranspose = glm::transpose(glm::inverse(model));
  });
}

void Mesh::submitDraw(VkCommandBuffer commandBuffer, uint_t frame_index) const {
  vkCmdBindPipeline(commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline_);

  geometry_->bind(commandBuffer);

  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          layout_, BINDING_SET_MESH, 1, &model_view_projection_descriptor_sets_[frame_index], 0, nullptr);

  VkDescriptorSet lightsDescriptorSet = renderer_->getLightsDescriptorSet(frame_index);

  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          layout_, BINDING_SET_LIGHTS, 1, &lightsDescriptorSet, 0, nullptr);

  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          layout_, BINDING_SET_MATERIAL, 1, &material_descriptor_sets_[frame_index], 0, nullptr);

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

glm::mat4 Mesh::getTransform() const {
  glm::mat4 position = glm::translate(glm::mat4(1.0), position_);
  glm::mat4 scale = glm::scale(glm::mat4(1.0), scale_);
  return position * glm::mat4(rotation_) * scale;
}