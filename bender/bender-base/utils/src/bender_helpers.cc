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

#include "bender_helpers.h"
#define GLM_FORCE_CXX17
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"
#include <sstream>
#include <unordered_map>

namespace BenderHelpers {

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties,
                        VkPhysicalDevice gpuDevice) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(gpuDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    LOGE("failed to find suitable memory type!");
    return -1;
}

void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image,
                    VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
                    VkPipelineStageFlags srcStages,
                    VkPipelineStageFlags destStages) {
    VkImageMemoryBarrier imageMemoryBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = NULL,
            .srcAccessMask = 0,
            .dstAccessMask = 0,
            .oldLayout = oldImageLayout,
            .newLayout = newImageLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange =
                    {
                            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                            .baseMipLevel = 0,
                            .levelCount = 1,
                            .baseArrayLayer = 0,
                            .layerCount = 1,
                    },
    };

    switch (oldImageLayout) {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_PREINITIALIZED:imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;

        default:break;
    }

    switch (newImageLayout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask =
                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        default:break;
    }

    vkCmdPipelineBarrier(cmdBuffer, srcStages, destStages, 0, 0, NULL, 0, NULL, 1,
                         &imageMemoryBarrier);
}

VkFormat findSupportedFormat(BenderKit::Device *device,
                             const std::vector<VkFormat> &candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features) {
  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(device->getPhysicalDevice(), format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL
        && (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }

  throw std::runtime_error("failed to find supported format!");
}

VkFormat findDepthFormat(BenderKit::Device *device) {
  return findSupportedFormat(
      device,
      {VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT},
      VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
  );
}

void loadOBJ(AAssetManager *mgr,
             const std::string &fileName,
             std::vector<float> &vertexData,
             std::vector<u_int16_t> &indexBuffer) {
  std::vector<glm::vec3> position;
  std::vector<glm::vec3> normal;
  std::vector<glm::vec2> texCoord;

  std::unordered_map<glm::vec3, int> vertToIndex;


  // Read the file:
  AAsset *file = AAssetManager_open(mgr, fileName.c_str(), AASSET_MODE_BUFFER);
  size_t fileLength = AAsset_getLength(file);

  char *fileContent = new char[fileLength];
  AAsset_read(file, fileContent, fileLength);

  std::stringstream data;
  data << fileContent;

  std::string label;
  while (data >> label){
    if (label == "v"){
      float x, y, z;
      data >> x >> y >> z;
      position.push_back({x, y, z});
    }
    else if (label == "vt"){
      float x, y;
      data >> x >> y;
      texCoord.push_back({x, y});
    }
    else if (label == "vn") {
      float x, y, z;
      data >> x >> y >> z;
      normal.push_back({x, y, z});
    }
    else if (label == "f"){
      glm::vec3 currVert;
      char skip;
      data >> currVert.x >> skip >> currVert.y >> skip >> currVert.z;
      int index;
      if (vertToIndex.find(currVert) == vertToIndex.end()){
        index = vertToIndex.size();
        vertToIndex[currVert] = index;
      }

      indexBuffer.push_back(vertToIndex[currVert]);
      vertexData.push_back(position[currVert.x-1].x);
      vertexData.push_back(position[currVert.x-1].y);
      vertexData.push_back(position[currVert.x-1].z);

      vertexData.push_back(normal[currVert.z-1].x);
      vertexData.push_back(normal[currVert.z-1].y);
      vertexData.push_back(normal[currVert.z-1].z);

      vertexData.push_back(1.0f);
      vertexData.push_back(0.0f);
      vertexData.push_back(0.0f);

      vertexData.push_back(texCoord[currVert.y-1].x);
      vertexData.push_back(texCoord[currVert.y-1].y);


      data >> currVert.x >> skip >> currVert.y >> skip >> currVert.z;
      if (vertToIndex.find(currVert) == vertToIndex.end()){
        index = vertToIndex.size();
        vertToIndex[currVert] = index;
      }

      indexBuffer.push_back(vertToIndex[currVert]);
      vertexData.push_back(position[currVert.x-1].x);
      vertexData.push_back(position[currVert.x-1].y);
      vertexData.push_back(position[currVert.x-1].z);

      vertexData.push_back(normal[currVert.z-1].x);
      vertexData.push_back(normal[currVert.z-1].y);
      vertexData.push_back(normal[currVert.z-1].z);

      vertexData.push_back(1.0f);
      vertexData.push_back(0.0f);
      vertexData.push_back(0.0f);

      vertexData.push_back(texCoord[currVert.y-1].x);
      vertexData.push_back(texCoord[currVert.y-1].y);


      data >> currVert.x >> skip >> currVert.y >> skip >> currVert.z;
      if (vertToIndex.find(currVert) == vertToIndex.end()){
        index = vertToIndex.size();
        vertToIndex[currVert] = index;
      }

      indexBuffer.push_back(vertToIndex[currVert]);
      vertexData.push_back(position[currVert.x-1].x);
      vertexData.push_back(position[currVert.x-1].y);
      vertexData.push_back(position[currVert.x-1].z);

      vertexData.push_back(normal[currVert.z-1].x);
      vertexData.push_back(normal[currVert.z-1].y);
      vertexData.push_back(normal[currVert.z-1].z);

      vertexData.push_back(1.0f);
      vertexData.push_back(0.0f);
      vertexData.push_back(0.0f);

      vertexData.push_back(texCoord[currVert.y-1].x);
      vertexData.push_back(texCoord[currVert.y-1].y);
    }
  }
}

}