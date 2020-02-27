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

#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "bender_helpers.h"

#include <cmath>
#include <cstdlib>
#include <vector>

Texture::Texture(Renderer *renderer,
                 uint8_t *img_data,
                 uint32_t img_width,
                 uint32_t img_height,
                 VkFormat texture_format,
                 bool use_mipmaps,
                 std::function<void(uint8_t *)> generator)
    : renderer_(renderer), texture_format_(texture_format), generator_(generator) {
  tex_width_ = img_width;
  tex_height_ = img_height;
  mip_levels_ = std::floor(std::log2(std::max(tex_width_, tex_height_))) + 1;
  CALL_VK(CreateTexture(img_data, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));
  CreateImageView();
}

Texture::Texture(Renderer *renderer,
                 android_app &android_app_ctx,
                 const std::string &texture_file_name,
                 VkFormat texture_format,
                 bool use_mipmaps,
                 std::function<void(uint8_t *)> generator)
    : renderer_(renderer), texture_format_(texture_format), generator_(generator) {
  unsigned char *img_data = LoadFileData(android_app_ctx, texture_file_name.c_str());
  file_name_ = texture_file_name;
  mip_levels_ = std::floor(std::log2(std::max(tex_width_, tex_height_))) + 1;
  CALL_VK(CreateTexture(img_data, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));
  CreateImageView();
  stbi_image_free(img_data);
}

Texture::~Texture() {
  Cleanup();
}

void Texture::Cleanup() {
  vkDestroyImageView(renderer_->GetVulkanDevice(), view_, nullptr);
  vkDestroyImage(renderer_->GetVulkanDevice(), image_, nullptr);
  vkFreeMemory(renderer_->GetVulkanDevice(), mem_, nullptr);
}

void Texture::OnResume(Renderer *renderer, android_app *app) {
  renderer_ = renderer;
  if (generator_ != nullptr) {
    unsigned char
        *img_data = (unsigned char *) malloc(tex_height_ * tex_width_ * 4 * sizeof(unsigned char));
    generator_(img_data);
    CALL_VK(CreateTexture(img_data,
                          VK_IMAGE_USAGE_SAMPLED_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));
    CreateImageView();
    delete img_data;
  } else {
    unsigned char *img_data = LoadFileData(*app, file_name_.c_str());
    CALL_VK(CreateTexture(img_data,
                          VK_IMAGE_USAGE_SAMPLED_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));
    CreateImageView();
    stbi_image_free(img_data);
  }
}

unsigned char *Texture::LoadFileData(android_app &app, const char *file_path) {
  AAsset *file = AAssetManager_open(app.activity->assetManager,
                                    file_path, AASSET_MODE_BUFFER);
  uint32_t img_width, img_height, n;
  unsigned char *img_data;
  if (file == nullptr) {
    img_width = 128;
    img_height = 128;
    img_data = (unsigned char *) malloc(img_width * img_height * 4 * sizeof(unsigned char));
    for (int32_t i = 0; i < img_height; i++) {
      for (int32_t j = 0; j < img_width; j++) {
        img_data[(i + j * img_width) * 4] = 215;
        img_data[(i + j * img_width) * 4 + 1] = 95;
        img_data[(i + j * img_width) * 4 + 2] = 175;
        img_data[(i + j * img_width) * 4 + 3] = 255;
      }
    }
  } else {
    size_t file_length = AAsset_getLength(file);
    stbi_uc *file_content = new unsigned char[file_length];
    AAsset_read(file, file_content, file_length);
    AAsset_close(file);

    img_data = stbi_load_from_memory(
        file_content, file_length, reinterpret_cast<int *>(&img_width),
        reinterpret_cast<int *>(&img_height), reinterpret_cast<int *>(&n), 4);
    delete[] file_content;
  }
  tex_width_ = img_width;
  tex_height_ = img_height;

  return img_data;
}

VkResult Texture::CreateTexture(uint8_t *img_data,
                                VkImageUsageFlags usage,
                                VkFlags required_props) {
  if (!(usage | required_props)) {
    LOGE("Texture: No usage and required_pros");
    return VK_ERROR_FORMAT_NOT_SUPPORTED;
  }

  VkFormatProperties props;
  vkGetPhysicalDeviceFormatProperties(renderer_->GetDevice().GetPhysicalDevice(), texture_format_, &props);
  assert((props.linearTilingFeatures | props.optimalTilingFeatures) &
      VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

  VkMemoryAllocateInfo mem_alloc = {
          .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
          .pNext = nullptr,
          .allocationSize = 0,
          .memoryTypeIndex = 0,
  };

  VkMemoryRequirements mem_reqs;

  VkBuffer staging;
  VkDeviceMemory staging_mem = 0;
  VkBufferCreateInfo staging_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .size = sizeof(unsigned char) * tex_height_ * tex_width_ * 4,
  };

  CALL_VK(vkCreateBuffer(renderer_->GetVulkanDevice(), &staging_info, nullptr, &staging));
  vkGetBufferMemoryRequirements(renderer_->GetVulkanDevice(), staging, &mem_reqs);
  mem_alloc.allocationSize = mem_reqs.size;
  mem_alloc.memoryTypeIndex = benderhelpers::FindMemoryType(mem_reqs.memoryTypeBits,
                                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                            renderer_->GetDevice().GetPhysicalDevice());
  CALL_VK(vkAllocateMemory(renderer_->GetVulkanDevice(), &mem_alloc, nullptr, &staging_mem));
  CALL_VK((vkBindBufferMemory(renderer_->GetVulkanDevice(), staging, staging_mem, 0)));

  void *data;
  CALL_VK(vkMapMemory(renderer_->GetVulkanDevice(), staging_mem, 0,
                      mem_alloc.allocationSize, 0, &data));

  memcpy(data, img_data, sizeof(unsigned char) * tex_height_ * tex_width_ * 4);

  vkUnmapMemory(renderer_->GetVulkanDevice(), staging_mem);


  uint32_t queue_family_index = renderer_->GetDevice().GetQueueFamilyIndex();
  VkImageCreateInfo image_create_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .pNext = nullptr,
      .imageType = VK_IMAGE_TYPE_2D,
      .format = texture_format_,
      .extent = {static_cast<uint32_t>(tex_width_),
                 static_cast<uint32_t>(tex_height_), 1},
      .mipLevels = mip_levels_,
      .arrayLayers = 1,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 1,
      .pQueueFamilyIndices = &queue_family_index,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .flags = 0,
  };

  CALL_VK(vkCreateImage(renderer_->GetVulkanDevice(), &image_create_info, nullptr, &image_));

  vkGetImageMemoryRequirements(renderer_->GetVulkanDevice(), image_, &mem_reqs);

  mem_alloc.allocationSize = mem_reqs.size;
  mem_alloc.memoryTypeIndex = benderhelpers::FindMemoryType(mem_reqs.memoryTypeBits,
                                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                            renderer_->GetDevice().GetPhysicalDevice());
  assert(mem_alloc.memoryTypeIndex != -1);

  CALL_VK(vkAllocateMemory(renderer_->GetVulkanDevice(), &mem_alloc, nullptr, &mem_));
  CALL_VK(vkBindImageMemory(renderer_->GetVulkanDevice(), image_, mem_, 0));

  VkImageSubresourceRange subres = {
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
          .levelCount = 1,
          .layerCount = 1,
  };


  VkCommandPoolCreateInfo cmd_pool_create_info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queue_family_index,
  };

  VkCommandPool cmd_pool;
  CALL_VK(vkCreateCommandPool(renderer_->GetVulkanDevice(), &cmd_pool_create_info, nullptr,
                              &cmd_pool));

  VkCommandBuffer copy_cmd;
  const VkCommandBufferAllocateInfo cmd = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = cmd_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };

  CALL_VK(vkAllocateCommandBuffers(renderer_->GetVulkanDevice(), &cmd, &copy_cmd));

  VkCommandBufferBeginInfo cmd_buf_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pInheritanceInfo = nullptr};

  CALL_VK(vkBeginCommandBuffer(copy_cmd, &cmd_buf_info));

  VkImageMemoryBarrier mem_barrier = {
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
          .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
          .srcAccessMask = 0,
          .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
          .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          .image = image_,
          .subresourceRange = subres,
  };
  vkCmdPipelineBarrier(copy_cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &mem_barrier);

  VkBufferImageCopy buffer_copy_region = {
          .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
          .imageSubresource.mipLevel = 0,
          .imageSubresource.baseArrayLayer = 0,
          .imageSubresource.layerCount = 1,
          .imageExtent.width = static_cast<uint32_t>(tex_width_),
          .imageExtent.height = static_cast<uint32_t>(tex_height_),
          .imageExtent.depth = 1,
  };
  vkCmdCopyBufferToImage(copy_cmd, staging, image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_copy_region);

  mem_barrier = {
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
          .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
          .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
          .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
          .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
          .image = image_,
          .subresourceRange = subres,
  };
  vkCmdPipelineBarrier(copy_cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &mem_barrier);

  CALL_VK(vkEndCommandBuffer(copy_cmd));

  VkFenceCreateInfo fence_info = {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
  };
  VkFence fence;
  CALL_VK(vkCreateFence(renderer_->GetVulkanDevice(), &fence_info, nullptr, &fence));

  VkSubmitInfo submit_info = {
      .pNext = nullptr,
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount = 0,
      .pWaitSemaphores = nullptr,
      .pWaitDstStageMask = nullptr,
      .commandBufferCount = 1,
      .pCommandBuffers = &copy_cmd,
      .signalSemaphoreCount = 0,
      .pSignalSemaphores = nullptr,
  };
  CALL_VK(vkQueueSubmit(renderer_->GetDevice().GetWorkerQueue(), 1, &submit_info, fence) != VK_SUCCESS);
  CALL_VK(vkWaitForFences(renderer_->GetVulkanDevice(), 1, &fence, VK_TRUE, 100000000) !=
          VK_SUCCESS);
  vkDestroyFence(renderer_->GetVulkanDevice(), fence, nullptr);

  vkFreeMemory(renderer_->GetVulkanDevice(), staging_mem, nullptr);
  vkDestroyBuffer(renderer_->GetVulkanDevice(), staging, nullptr);

  VkCommandBuffer mipmaps_cmd;

  CALL_VK(vkAllocateCommandBuffers(renderer_->GetVulkanDevice(), &cmd, &mipmaps_cmd));

  VkCommandBufferBeginInfo mipmaps_info = {
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
          .pNext = nullptr,
          .flags = 0,
          .pInheritanceInfo = nullptr
  };
  CALL_VK(vkBeginCommandBuffer(mipmaps_cmd, &mipmaps_info));
  GenerateMipmaps(&mipmaps_cmd);
  CALL_VK(vkEndCommandBuffer(mipmaps_cmd));

  CALL_VK(vkCreateFence(renderer_->GetVulkanDevice(), &fence_info, nullptr, &fence));
  submit_info.pCommandBuffers = &mipmaps_cmd;
  CALL_VK(vkQueueSubmit(renderer_->GetDevice().GetWorkerQueue(), 1, &submit_info, fence) != VK_SUCCESS);
  CALL_VK(vkWaitForFences(renderer_->GetVulkanDevice(), 1, &fence, VK_TRUE, 100000000) !=
          VK_SUCCESS);
  vkDestroyFence(renderer_->GetVulkanDevice(), fence, nullptr);

  vkFreeCommandBuffers(renderer_->GetVulkanDevice(), cmd_pool, 1, &copy_cmd);
  vkFreeCommandBuffers(renderer_->GetVulkanDevice(), cmd_pool, 1, &mipmaps_cmd);
  vkDestroyCommandPool(renderer_->GetVulkanDevice(), cmd_pool, nullptr);
  return VK_SUCCESS;
}

void Texture::CreateImageView() {
  VkImageViewCreateInfo view_create_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .pNext = nullptr,
      .image = image_,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = texture_format_,
      .components =
          {
              .r = VK_COMPONENT_SWIZZLE_R,
              .g = VK_COMPONENT_SWIZZLE_G,
              .b = VK_COMPONENT_SWIZZLE_B,
              .a = VK_COMPONENT_SWIZZLE_A,
          },
      .subresourceRange = {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = 0,
              .baseArrayLayer = 0,
              .layerCount = 1,
              .levelCount = renderer_->MipMapEnabled() ? mip_levels_ : 1,
      },
      .flags = 0,
  };
  CALL_VK(vkCreateImageView(renderer_->GetVulkanDevice(), &view_create_info, nullptr, &view_));
}

void Texture::ToggleMipmaps(android_app *app) {
  vkDestroyImageView(renderer_->GetVulkanDevice(), view_, nullptr);
  CreateImageView();
}

void Texture::GenerateMipmaps(VkCommandBuffer *commandBuffer) {

  VkImageMemoryBarrier barrier = {
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .image = image_,
          .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
          .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
          .subresourceRange = {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .levelCount = 1,
              .layerCount = 1,
          },
  };

//  int32_t mip_width = tex_width_;
//  int32_t mip_height = tex_height_;

  for (uint32_t i = 1; i < mip_levels_; i++) {

      VkImageBlit blit = {
              .srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .srcSubresource.layerCount = 1,
              .srcSubresource.mipLevel = i-1,
              .srcOffsets[1].x = int32_t (tex_width_ >> (i-1)),
              .srcOffsets[1].y = int32_t (tex_height_ >> (i-1)),
              .srcOffsets[1].z = 1,
              .dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .dstSubresource.layerCount = 1,
              .dstSubresource.mipLevel = i,
              .dstOffsets[1].x = int32_t (tex_width_ >> (i)),
              .dstOffsets[1].y = int32_t (tex_height_ >> (i)),
              .dstOffsets[1].z = 1,
      };

      VkImageSubresourceRange mip_subres= {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = i,
              .levelCount = 1,
              .layerCount = 1,
      };

      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.subresourceRange = mip_subres;

      vkCmdPipelineBarrier(*commandBuffer,
                           VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                           0, nullptr,
                           0, nullptr,
                           1, &barrier);

      vkCmdBlitImage(*commandBuffer,
                     image_, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                     image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                     1, &blit, VK_FILTER_LINEAR);

      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      barrier.subresourceRange = mip_subres;

      vkCmdPipelineBarrier(*commandBuffer,
                           VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                           0, nullptr,
                           0, nullptr,
                           1, &barrier);


  }

  barrier.subresourceRange.levelCount = mip_levels_;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
  barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

  vkCmdPipelineBarrier(*commandBuffer,
                       VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                       0, nullptr,
                       0, nullptr,
                       1, &barrier);
}
