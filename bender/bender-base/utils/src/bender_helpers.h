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


constexpr float SNORM_MAX = 32767.0f;
constexpr float SNORM_MIN = -32767.0f;
constexpr float UNORM_MAX = 65535.0f;

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

inline int16_t NormFloatToSnorm16(float v) {
    assert( v >= -1.0f && v <= 1.0f && "v out of range snorm");
    return static_cast<int16_t>(std::clamp(v >= 0.0f ?
                                           (v * SNORM_MAX + 0.5f) :
                                           (v * SNORM_MAX - 0.5f),
                                           SNORM_MIN, SNORM_MAX));
}

inline uint16_t NormFloatToUnorm16(float v) {
    assert( v >= 0.0f && v <= 1.0f && "v out of range unorm");
    return static_cast<uint16_t >( v * UNORM_MAX + 0.5f);
}

inline float Snorm16ToFloat(int16_t v) {
    // -32768 & -32767 both map to -1 according to D3D10 rules.
    return std::max(v / SNORM_MAX, -1.0f);
}

inline float Unorm16ToFloat(uint16_t v) {
    return std::max(v / UNORM_MAX, 0.0f);
}
}

#endif  // BENDER_BASE_UTILS_SRC_BENDER_HELPERS_H
