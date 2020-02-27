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
    : renderer_(renderer), texture_format_(texture_format), use_mipmaps_(use_mipmaps), generator_(generator) {
  tex_width_ = img_width;
  tex_height_ = img_height;
  mip_levels_ = use_mipmaps ? std::floor(std::log2(std::max(tex_width_, tex_height_))) + 1 : 1;
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
    : renderer_(renderer), texture_format_(texture_format), use_mipmaps_(use_mipmaps), generator_(generator) {
  unsigned char *img_data = LoadFileData(android_app_ctx, texture_file_name.c_str());
  file_name_ = texture_file_name;
  mip_levels_ = use_mipmaps ? std::floor(std::log2(std::max(tex_width_, tex_height_))) + 1 : 1;
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
  RegenerateTexture(app);
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
      .tiling = VK_IMAGE_TILING_LINEAR,
      .usage = VK_IMAGE_USAGE_SAMPLED_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 1,
      .pQueueFamilyIndices = &queue_family_index,
      .initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED,
      .flags = 0,
  };
  VkMemoryAllocateInfo mem_alloc = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = nullptr,
      .allocationSize = 0,
      .memoryTypeIndex = 0,
  };

  VkMemoryRequirements mem_reqs;
  CALL_VK(vkCreateImage(renderer_->GetVulkanDevice(), &image_create_info, nullptr, &image_));

  vkGetImageMemoryRequirements(renderer_->GetVulkanDevice(), image_, &mem_reqs);

  mem_alloc.allocationSize = mem_reqs.size;
  mem_alloc.memoryTypeIndex = benderhelpers::FindMemoryType(mem_reqs.memoryTypeBits,
                                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                                            renderer_->GetDevice().GetPhysicalDevice());
  assert(mem_alloc.memoryTypeIndex != -1);

  CALL_VK(vkAllocateMemory(renderer_->GetVulkanDevice(), &mem_alloc, nullptr, &mem_));
  CALL_VK(vkBindImageMemory(renderer_->GetVulkanDevice(), image_, mem_, 0));

  if (required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
    VkSubresourceLayout layout;

    const VkImageSubresource subres = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .mipLevel = 0,
        .arrayLayer = 0,
    };

    vkGetImageSubresourceLayout(renderer_->GetVulkanDevice(), image_, &subres,
                                &layout);
    void *data;
    CALL_VK(vkMapMemory(renderer_->GetVulkanDevice(), mem_, 0,
                        mem_alloc.allocationSize, 0, &data));

    for (int32_t y = 0; y < tex_height_; y++) {
      unsigned char *row = (unsigned char *) ((char *) data + layout.rowPitch * y);
      for (int32_t x = 0; x < tex_width_; x++) {
        row[x * 4] = img_data[(x + y * tex_width_) * 4];
        row[x * 4 + 1] = img_data[(x + y * tex_width_) * 4 + 1];
        row[x * 4 + 2] = img_data[(x + y * tex_width_) * 4 + 2];
        row[x * 4 + 3] = img_data[(x + y * tex_width_) * 4 + 3];
      }
    }

    vkUnmapMemory(renderer_->GetVulkanDevice(), mem_);
  }

  image_layout_ = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkCommandPoolCreateInfo cmd_pool_create_info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queue_family_index,
  };

  VkCommandPool cmd_pool;
  CALL_VK(vkCreateCommandPool(renderer_->GetVulkanDevice(), &cmd_pool_create_info, nullptr,
                              &cmd_pool));

  VkCommandBuffer gfx_cmd;
  const VkCommandBufferAllocateInfo cmd = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = cmd_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };

  CALL_VK(vkAllocateCommandBuffers(renderer_->GetVulkanDevice(), &cmd, &gfx_cmd));
  VkCommandBufferBeginInfo cmd_buf_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pInheritanceInfo = nullptr};
  CALL_VK(vkBeginCommandBuffer(gfx_cmd, &cmd_buf_info));

  benderhelpers::SetImageLayout(gfx_cmd, image_, VK_IMAGE_LAYOUT_PREINITIALIZED,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                VK_PIPELINE_STAGE_HOST_BIT,
                                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

  CALL_VK(vkEndCommandBuffer(gfx_cmd));
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
      .pCommandBuffers = &gfx_cmd,
      .signalSemaphoreCount = 0,
      .pSignalSemaphores = nullptr,
  };
  CALL_VK(vkQueueSubmit(renderer_->GetDevice().GetWorkerQueue(), 1, &submit_info, fence) != VK_SUCCESS);
  CALL_VK(vkWaitForFences(renderer_->GetVulkanDevice(), 1, &fence, VK_TRUE, 100000000) !=
      VK_SUCCESS);
  vkDestroyFence(renderer_->GetVulkanDevice(), fence, nullptr);

  vkFreeCommandBuffers(renderer_->GetVulkanDevice(), cmd_pool, 1, &gfx_cmd);
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
      .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, mip_levels_, 0, 1},
      .flags = 0,
  };
  CALL_VK(vkCreateImageView(renderer_->GetVulkanDevice(), &view_create_info, nullptr, &view_));
}

void Texture::RegenerateTexture(android_app *app) {
  if (generator_ != nullptr) {
    unsigned char
            *img_data = (unsigned char *) malloc(tex_height_ * tex_width_ * 4 * sizeof(unsigned char));
    generator_(img_data);
    CALL_VK(CreateTexture(img_data,
                          VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));
    CreateImageView();
    delete img_data;
  } else {
    unsigned char *img_data = LoadFileData(*app, file_name_.c_str());
    CALL_VK(CreateTexture(img_data,
                          VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));
    CreateImageView();
    stbi_image_free(img_data);
  }
}

void Texture::ToggleMipmaps(android_app *app) {
  use_mipmaps_ = !use_mipmaps_;
  mip_levels_ = use_mipmaps_ ? std::floor(std::log2(std::max(tex_width_, tex_height_))) + 1 : 1;
  RegenerateTexture(app);
}

void Texture::GenerateMipmaps() {
  if (!use_mipmaps_) return;

  renderer_->BeginPrimaryCommandBufferRecording();

  VkImageMemoryBarrier barrier = {
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .image = image_,
          .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
          .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
          .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
  };

  renderer_->EndPrimaryCommandBufferRecording();

//  int32_t mip_width = tex_width_;
//  int32_t mip_height = tex_height_;

  for (uint32_t i = 1; i < mip_levels_; i++) {
      barrier.subresourceRange.baseMipLevel = i - 1;
      barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

//      vkCmdPipelineBarrier(cmd_buffer, )
  }
}
