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
int trueIndex(int idx, int size) {
  if (idx < 0) {
    return size + idx;
  } else {
    return idx - 1;
  }
}

void addVertex(glm::vec3 &currVert,
               OBJ &currOBJ,
               std::vector<float> &vertexData,
               std::unordered_map<glm::vec3, uint32_t> &vertToIndex,
               std::vector<glm::vec3> &position,
               std::vector<glm::vec3> &normal,
               std::vector<glm::vec2> &texCoord) {
  uint32_t index;
  if (vertToIndex.find(currVert) != vertToIndex.end()) {
    currOBJ.indexBuffer.push_back(vertToIndex[currVert]);
    return;
  }

  index = vertToIndex.size();
  vertToIndex[currVert] = index;
  currOBJ.indexBuffer.push_back(vertToIndex[currVert]);
  vertexData.push_back(position[trueIndex(currVert.x, position.size())].x);
  vertexData.push_back(position[trueIndex(currVert.x, position.size())].y);
  vertexData.push_back(position[trueIndex(currVert.x, position.size())].z);

  vertexData.push_back(normal[trueIndex(currVert.z, normal.size())].x);
  vertexData.push_back(normal[trueIndex(currVert.z, normal.size())].y);
  vertexData.push_back(normal[trueIndex(currVert.z, normal.size())].z);

  vertexData.push_back(texCoord[trueIndex(currVert.y, texCoord.size())].x);
  vertexData.push_back(texCoord[trueIndex(currVert.y, texCoord.size())].y);
}

void loadMTL(AAssetManager *mgr,
             const std::string &fileName,
             std::unordered_map<std::string, MTL> &mtllib) {

  AAsset *file = AAssetManager_open(mgr, ("models/" + fileName).c_str(), AASSET_MODE_BUFFER);
  size_t fileLength = AAsset_getLength(file);

  char *fileContent = new char[fileLength];
  AAsset_read(file, fileContent, fileLength);

  std::stringstream data;
  data << fileContent;
  std::string label;
  std::string currentMaterialName = "";

  while (data >> label) {
    if (label == "newmtl") {
      data >> currentMaterialName;
      mtllib[currentMaterialName] = MTL();
    } else if (label == "Ns") {
      data >> mtllib[currentMaterialName].specularExponent;
    } else if (label == "Ka") {
      data >> mtllib[currentMaterialName].ambient.x >>
           mtllib[currentMaterialName].ambient.y >>
           mtllib[currentMaterialName].ambient.z;
    } else if (label == "Kd") {
      data >> mtllib[currentMaterialName].diffuse.x >>
           mtllib[currentMaterialName].diffuse.y >>
           mtllib[currentMaterialName].diffuse.z;
    } else if (label == "Ks") {
      data >> mtllib[currentMaterialName].specular.x >>
           mtllib[currentMaterialName].specular.y >>
           mtllib[currentMaterialName].specular.z;
    } else if (label == "map_Ka") {
      data >> mtllib[currentMaterialName].map_Ka;
    } else if (label == "map_Ks") {
      data >> mtllib[currentMaterialName].map_Ks;
    } else if (label == "map_Kd") {
      data >> mtllib[currentMaterialName].map_Kd;
    } else if (label == "map_Ns") {
      data >> mtllib[currentMaterialName].map_Ns;
    } else if (label == "map_bump") {
      data >> mtllib[currentMaterialName].map_bump;
    }
  }
}

void loadOBJ(AAssetManager *mgr,
             const std::string &fileName,
             std::vector<float> &vertexData,
             std::unordered_map<std::string, MTL> &mtllib,
             std::vector<OBJ> &modelData) {

  std::vector<glm::vec3> position;
  std::vector<glm::vec3> normal;
  std::vector<glm::vec2> texCoord;
  std::unordered_map<glm::vec3, uint32_t> vertToIndex;


  // Read the file:
  AAsset *file = AAssetManager_open(mgr, fileName.c_str(), AASSET_MODE_BUFFER);
  size_t fileLength = AAsset_getLength(file);

  char *fileContent = new char[fileLength];
  AAsset_read(file, fileContent, fileLength);

  std::stringstream data;
  data << fileContent;
  std::string label;

  while (data >> label) {
    if (label == "mtllib") {
      std::string mtllibName;
      data >> mtllibName;
      loadMTL(mgr, mtllibName, mtllib);
    } else if (label == "v") {
      float x, y, z;
      data >> x >> y >> z;
      position.push_back({x, y, z});
    } else if (label == "vt") {
      float x, y;
      data >> x >> y;
      texCoord.push_back({x, y});
    } else if (label == "vn") {
      float x, y, z;
      data >> x >> y >> z;
      normal.push_back({x, y, z});
    } else if (label == "usemtl"){
      std::string materialName;
      data >> materialName;
      OBJ curr;
      curr.materialName = materialName;
      modelData.push_back(curr);
    } else if (label == "f") {
      glm::vec3 vertex1, vertex2, vertex3;
      char skip;
      data >> vertex1.x >> skip >> vertex1.y >> skip >> vertex1.z;
      data >> vertex2.x >> skip >> vertex2.y >> skip >> vertex2.z;
      data >> vertex3.x >> skip >> vertex3.y >> skip >> vertex3.z;
      addVertex(vertex3, modelData.back(), vertexData, vertToIndex, position, normal, texCoord);
      addVertex(vertex2, modelData.back(), vertexData, vertToIndex, position, normal, texCoord);
      addVertex(vertex1, modelData.back(), vertexData, vertToIndex, position, normal, texCoord);
    }
  }
}
}