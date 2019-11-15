// Copyright 2019 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "renderer.h"
#include <android_native_app_glue.h>
#include <debug_marker.h>

#include "trace.h"
#include "bender_helpers.h"
#include "shader_bindings.h"
#include "constants.h"

DefaultStates::DefaultStates(const BenderKit::Device &device): device_(device) {
  viewport_ = {
          .x = 0.0f,
          .y = 0.0f,
          .width = static_cast<float>(device_.getDisplaySize().width),
          .height = static_cast<float>(device_.getDisplaySize().height),
          .minDepth = 0.0f,
          .maxDepth = 1.0f,
  };

  scissor_ = {
          .offset = {0, 0},
          .extent = device_.getDisplaySize(),
  };

  attachments_ = {
          .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
          .blendEnable = VK_FALSE,
  };

  pipeline_viewport_state_ = defaultPipelineViewportState();
  depth_stencil_state_ = defaultDepthStencilState();
  pipeline_rasterization_state_ = defaultPipelineRasterizationState();
  pipeline_multisample_state_ = defaultPipelineMultisampleState();
  color_blend_info_ = defaultColorBlendInfo();
}

void DefaultStates::fillDefaultPipelineCreateInfo(VkGraphicsPipelineCreateInfo* pipeline_create_info) const {
  pipeline_create_info->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_create_info->pNext = nullptr;
  pipeline_create_info->flags = 0;
  pipeline_create_info->stageCount = 2;
  pipeline_create_info->pTessellationState = nullptr;
  pipeline_create_info->pViewportState = &pipeline_viewport_state_;
  pipeline_create_info->pRasterizationState = &pipeline_rasterization_state_;
  pipeline_create_info->pMultisampleState = &pipeline_multisample_state_;
  pipeline_create_info->pDepthStencilState = &depth_stencil_state_;
  pipeline_create_info->pColorBlendState = &color_blend_info_;
  pipeline_create_info->pDynamicState = nullptr;
  pipeline_create_info->subpass = 0;
  pipeline_create_info->basePipelineHandle = VK_NULL_HANDLE;
  pipeline_create_info->basePipelineIndex = 0;
}

VkPipelineViewportStateCreateInfo DefaultStates::defaultPipelineViewportState() const {
  return {
          .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
          .viewportCount = 1,
          .pViewports = &viewport_,
          .scissorCount = 1,
          .pScissors = &scissor_,
  };
}

VkPipelineDepthStencilStateCreateInfo DefaultStates::defaultDepthStencilState() const {
  return {
          .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
          .depthTestEnable = VK_TRUE,
          .depthWriteEnable = VK_TRUE,
          .depthCompareOp = VK_COMPARE_OP_LESS,
          .depthBoundsTestEnable = VK_FALSE,
          .minDepthBounds = 0.0f,
          .maxDepthBounds = 1.0f,
          .stencilTestEnable = VK_FALSE,
  };
}

VkPipelineRasterizationStateCreateInfo DefaultStates::defaultPipelineRasterizationState() const {
  return {
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
}

VkPipelineMultisampleStateCreateInfo DefaultStates::defaultPipelineMultisampleState() const {
  return {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .pNext = nullptr,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable = VK_FALSE,
    .minSampleShading = 0,
    .pSampleMask = nullptr,
    .alphaToCoverageEnable = VK_FALSE,
    .alphaToOneEnable = VK_FALSE,
  };
}

VkPipelineColorBlendStateCreateInfo DefaultStates::defaultColorBlendInfo() const {
  return {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .pNext = nullptr,
    .logicOpEnable = VK_FALSE,
    .logicOp = VK_LOGIC_OP_COPY,
    .attachmentCount = 1,
    .pAttachments = &attachments_,
    .flags = 0,
  };
};