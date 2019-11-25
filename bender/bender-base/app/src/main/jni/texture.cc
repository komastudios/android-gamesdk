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

#include <cstdlib>
#include <vector>

Texture::Texture(BenderKit::Device& device, uint8_t *imgData, uint32_t imgWidth,
        uint32_t imgHeight, VkFormat textureFormat) : device_(device), texture_format_(textureFormat) {
    tex_width_ = imgWidth;
    tex_height_ = imgHeight;
    CALL_VK(createTexture(imgData, VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));
    createImageView();
}

Texture::Texture(BenderKit::Device& device, android_app &androidAppCtx,
                 const char *textureFileName, VkFormat textureFormat) : device_(device), texture_format_(textureFormat) {
    unsigned char *imgData = loadFileData(androidAppCtx, textureFileName);
    CALL_VK(createTexture(imgData, VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));
    createImageView();
    stbi_image_free(imgData);
}

Texture::~Texture() {
    vkDestroyImageView(device_.getDevice(), view_, nullptr);
    vkDestroyImage(device_.getDevice(), image_, nullptr);
    vkFreeMemory(device_.getDevice(), mem_, nullptr);
}

unsigned char *Texture::loadFileData(android_app &app, const char *filePath) {
    AAsset *file = AAssetManager_open(app.activity->assetManager,
                                      filePath, AASSET_MODE_BUFFER);
    uint32_t img_width, img_height, n;
    unsigned char *img_data;
    if (file == nullptr) {
        img_width = 128;
        img_height = 128;
        img_data = (unsigned char *) malloc(img_width * img_height * 4 * sizeof(unsigned char));
        for (int32_t y = 0; y < img_height; y++) {
            for (int32_t x = 0; x < img_width; x++) {
                img_data[(x + y * img_width) * 4] = 215;
                img_data[(x + y * img_width) * 4 + 1] = 95;
                img_data[(x + y * img_width) * 4 + 2] = 175;
                img_data[(x + y * img_width) * 4 + 3] = 255;
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
        assert(n == 4);
        delete[] file_content;
    }
    tex_width_ = img_width;
    tex_height_ = img_height;

    return img_data;
}

VkResult Texture::createTexture(uint8_t *imgData,
                                VkImageUsageFlags usage,
                                VkFlags required_props) {
    if (!(usage | required_props)) {
        LOGE("Texture: No usage and required_pros");
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(device_.getPhysicalDevice(), texture_format_, &props);
    assert((props.linearTilingFeatures | props.optimalTilingFeatures) &
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

    uint32_t queue_family_index = device_.getQueueFamilyIndex();
    VkImageCreateInfo image_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = texture_format_,
            .extent = {static_cast<uint32_t>(tex_width_),
                       static_cast<uint32_t>(tex_height_), 1},
            .mipLevels = 1,
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
    CALL_VK(vkCreateImage(device_.getDevice(), &image_create_info, nullptr, &image_));

    vkGetImageMemoryRequirements(device_.getDevice(), image_, &mem_reqs);

    mem_alloc.allocationSize = mem_reqs.size;
    mem_alloc.memoryTypeIndex = BenderHelpers::findMemoryType(mem_reqs.memoryTypeBits,
                                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                                              device_.getPhysicalDevice());
    assert(mem_alloc.memoryTypeIndex != -1);

    CALL_VK(vkAllocateMemory(device_.getDevice(), &mem_alloc, nullptr, &mem_));
    CALL_VK(vkBindImageMemory(device_.getDevice(), image_, mem_, 0));

    if (required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        VkSubresourceLayout layout;

        const VkImageSubresource subres = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .arrayLayer = 0,
        };

        vkGetImageSubresourceLayout(device_.getDevice(), image_, &subres,
                                    &layout);
        void *data;
        CALL_VK(vkMapMemory(device_.getDevice(), mem_, 0,
                            mem_alloc.allocationSize, 0, &data));

        for (int32_t y = 0; y < tex_height_; y++) {
            unsigned char* row = (unsigned char*)((char*)data + layout.rowPitch * y);
            for (int32_t x = 0; x < tex_width_; x++) {
                row[x * 4] = imgData[(x + y * tex_width_) * 4];
                row[x * 4 + 1] = imgData[(x + y * tex_width_) * 4 + 1];
                row[x * 4 + 2] = imgData[(x + y * tex_width_) * 4 + 2];
                row[x * 4 + 3] = imgData[(x + y * tex_width_) * 4 + 3];
            }
        }

        vkUnmapMemory(device_.getDevice(), mem_);
    }


    image_layout_ = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkCommandPoolCreateInfo cmd_pool_create_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queue_family_index,
    };

    VkCommandPool cmd_pool;
    CALL_VK(vkCreateCommandPool(device_.getDevice(), &cmd_pool_create_info, nullptr,
                                &cmd_pool));

    VkCommandBuffer gfx_cmd;
    const VkCommandBufferAllocateInfo cmd = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = cmd_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
    };

    CALL_VK(vkAllocateCommandBuffers(device_.getDevice(), &cmd, &gfx_cmd));
    VkCommandBufferBeginInfo cmd_buf_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr};
    CALL_VK(vkBeginCommandBuffer(gfx_cmd, &cmd_buf_info));

    BenderHelpers::setImageLayout(gfx_cmd, image_, VK_IMAGE_LAYOUT_PREINITIALIZED,
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
    CALL_VK(vkCreateFence(device_.getDevice(), &fence_info, nullptr, &fence));

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
    CALL_VK(vkQueueSubmit(device_.getQueue(), 1, &submit_info, fence) != VK_SUCCESS);
    CALL_VK(vkWaitForFences(device_.getDevice(), 1, &fence, VK_TRUE, 100000000) !=
            VK_SUCCESS);
    vkDestroyFence(device_.getDevice(), fence, nullptr);

    vkFreeCommandBuffers(device_.getDevice(), cmd_pool, 1, &gfx_cmd);
    vkDestroyCommandPool(device_.getDevice(), cmd_pool, nullptr);
    return VK_SUCCESS;
}

void Texture::createImageView() {
    VkImageViewCreateInfo viewCreateInfo = {
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
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
            .flags = 0,
    };
    CALL_VK(vkCreateImageView(device_.getDevice(), &viewCreateInfo, nullptr, &view_));
}
