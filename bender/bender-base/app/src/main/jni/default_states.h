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

#ifndef BENDER_BASE_DEFAULT_STATES_H_
#define BENDER_BASE_DEFAULT_STATES_H_

#include "vulkan_wrapper.h"

class DefaultStates {
public:
  DefaultStates(const benderkit::Device &device);

  VkPipelineViewportStateCreateInfo GetPipelineViewportState() const { return pipeline_viewport_state_; }
  VkPipelineDepthStencilStateCreateInfo GetDepthStencilState() const { return depth_stencil_state_; }
  VkPipelineRasterizationStateCreateInfo GetPipelineRasterizationState() const { return pipeline_rasterization_state_; }
  VkPipelineMultisampleStateCreateInfo GetPipelineMultisampleState() const { return pipeline_multisample_state_;  }
  VkPipelineColorBlendStateCreateInfo GetColorBlendInfo() const { return color_blend_info_; }

  void FillDefaultPipelineCreateInfo(VkGraphicsPipelineCreateInfo* pipeline_create_info) const;

private:
  const benderkit::Device &device_;

  VkViewport viewport_;
  VkRect2D scissor_;
  VkPipelineColorBlendAttachmentState attachments_;

  VkPipelineViewportStateCreateInfo pipeline_viewport_state_;
  VkPipelineDepthStencilStateCreateInfo depth_stencil_state_;
  VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_;
  VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_;
  VkPipelineColorBlendStateCreateInfo color_blend_info_;

  VkPipelineViewportStateCreateInfo DefaultPipelineViewportState() const;
  VkPipelineDepthStencilStateCreateInfo DefaultDepthStencilState() const;
  VkPipelineRasterizationStateCreateInfo DefaultPipelineRasterizationState() const;
  VkPipelineMultisampleStateCreateInfo DefaultPipelineMultisampleState() const;
  VkPipelineColorBlendStateCreateInfo DefaultColorBlendInfo() const;
};

#endif // BENDER_BASE_DEFAULT_STATES_H_
