// Copyright 2016 Google Inc. All Rights Reserved.
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

#ifndef BENDER_BASE_UTILS_SRC_BENDER_HELPERS_H_
#define BENDER_BASE_UTILS_SRC_BENDER_HELPERS_H_

#include "vulkan_wrapper.h"
#include "bender_kit.h"

namespace benderhelpers {

uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties,
                        VkPhysicalDevice gpu_device);

void SetImageLayout(VkCommandBuffer cmd_buffer, VkImage image,
                    VkImageLayout old_image_layout, VkImageLayout new_image_layout,
                    VkPipelineStageFlags src_stages,
                    VkPipelineStageFlags dest_stages);

VkFormat FindSupportedFormat(benderkit::Device *device,
                             const std::vector<VkFormat> &candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features);

VkFormat FindDepthFormat(benderkit::Device *device);

}

#endif  // BENDER_BASE_UTILS_SRC_BENDER_HELPERS_H
