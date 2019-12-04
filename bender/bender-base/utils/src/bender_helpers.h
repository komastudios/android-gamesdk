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
#include <android/asset_manager.h>
#define GLM_FORCE_CXX17
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include <unordered_map>

namespace BenderHelpers {

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties,
                        VkPhysicalDevice gpuDevice);

void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image,
                    VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
                    VkPipelineStageFlags srcStages,
                    VkPipelineStageFlags destStages);

VkFormat findSupportedFormat(BenderKit::Device *device,
                             const std::vector<VkFormat> &candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features);

VkFormat findDepthFormat(BenderKit::Device *device);


struct MTL{
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float specularExponent;

  std::string map_Ka = "";
  std::string map_Kd = "";
  std::string map_Ks = "";
  std::string map_Ns = "";
  std::string map_bump = "";
};

struct OBJ{
  std::string name;
  std::string materialName;
  std::vector<uint32_t> indexBuffer;
};

void loadOBJ(AAssetManager *mgr,
             const std::string &fileName,
             std::vector<float> &vertexData,
             std::unordered_map<std::string, MTL> &mtllib,
             std::vector<OBJ> &modelData);

}

#endif  // BENDER_BASE_UTILS_SRC_BENDER_HELPERS_H
