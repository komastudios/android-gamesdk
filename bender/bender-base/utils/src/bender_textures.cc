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
#include "bender_textures.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "bender_helpers.h"

#include <cstdlib>
#include <vector>

extern struct texture_object textures[TEXTURE_COUNT];

#define VK_CHECK(x) CALL_VK(x)

const char* texFiles[TEXTURE_COUNT] = {
        "textures/sample_texture0.png",
};

VkResult AllocateMemoryTypeFromProperties(BenderKit::Device* device,
                                          uint32_t typeBits,
                                          VkFlags requirements_mask,
                                          uint32_t* typeIndex) {
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < 32; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((device->getGpuMemProperties().memoryTypes[i].propertyFlags &
                 requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return VK_SUCCESS;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return VK_ERROR_MEMORY_MAP_FAILED;
}

VkResult LoadTextureFromFile(const char* filePath, android_app *androidAppCtx,
                             struct texture_object* tex_obj, BenderKit::Device *device,
                             VkImageUsageFlags usage, VkFlags required_props) {
    if (!(usage | required_props)) {
        __android_log_print(ANDROID_LOG_ERROR, "tutorial texture",
                            "No usage and required_pros");
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    // Check for linear supportability
    VkFormatProperties props;
    bool needBlit = true;
    vkGetPhysicalDeviceFormatProperties(device->getPhysicalDevice(), kTexFmt, &props);
    assert((props.linearTilingFeatures | props.optimalTilingFeatures) &
           VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

    if (props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
        // linear format supporting the required texture
        needBlit = false;
    }

    // Read the file:
    AAsset* file = AAssetManager_open(androidAppCtx->activity->assetManager,
                                      filePath, AASSET_MODE_BUFFER);
    uint32_t imgWidth, imgHeight, n;
    unsigned char *imageData;
    if(file == nullptr) {
        imgWidth = 128;
        imgHeight = 128;
        imageData = (unsigned char *) malloc(imgWidth * imgHeight * 4 * sizeof(unsigned char));
        for (int32_t y = 0; y < imgHeight; y++) {
            for (int32_t x = 0; x < imgWidth; x++) {
                imageData[(x + y * imgWidth) * 4] = 215;
                imageData[(x + y * imgWidth) * 4 + 1] =  95;
                imageData[(x + y * imgWidth) * 4 + 2] = 175;
                imageData[(x + y * imgWidth) * 4 + 3] = 1;
            }
        }

    } else {
        size_t fileLength = AAsset_getLength(file);
        stbi_uc *fileContent = new unsigned char[fileLength];
        AAsset_read(file, fileContent, fileLength);
        AAsset_close(file);

        imageData = stbi_load_from_memory(
                fileContent, fileLength, reinterpret_cast<int *>(&imgWidth),
                reinterpret_cast<int *>(&imgHeight), reinterpret_cast<int *>(&n), 4);
        assert(n == 4);
        delete[] fileContent;
    }
    tex_obj->tex_width = imgWidth;
    tex_obj->tex_height = imgHeight;


    uint32_t queue_family_index = device->getQueueFamilyIndex();
    // Allocate the linear texture so texture could be copied over
    VkImageCreateInfo image_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = kTexFmt,
            .extent = {static_cast<uint32_t>(imgWidth),
                       static_cast<uint32_t>(imgHeight), 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_LINEAR,
            .usage = (needBlit ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                               : VK_IMAGE_USAGE_SAMPLED_BIT),
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
    CALL_VK(vkCreateImage(device->getDevice(), &image_create_info, nullptr,
                          &tex_obj->image));
    vkGetImageMemoryRequirements(device->getDevice(), tex_obj->image, &mem_reqs);
    mem_alloc.allocationSize = mem_reqs.size;
    VK_CHECK(AllocateMemoryTypeFromProperties(device,
                                              mem_reqs.memoryTypeBits,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                              &mem_alloc.memoryTypeIndex));
    CALL_VK(vkAllocateMemory(device->getDevice(), &mem_alloc, nullptr, &tex_obj->mem));
    CALL_VK(vkBindImageMemory(device->getDevice(), tex_obj->image, tex_obj->mem, 0));

    if (required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        const VkImageSubresource subres = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .arrayLayer = 0,
        };
        VkSubresourceLayout layout;
        void* data;

        vkGetImageSubresourceLayout(device->getDevice(), tex_obj->image, &subres,
                                    &layout);
        CALL_VK(vkMapMemory(device->getDevice(), tex_obj->mem, 0,
                            mem_alloc.allocationSize, 0, &data));

        for (int32_t y = 0; y < imgHeight; y++) {
            unsigned char* row = (unsigned char*)((char*)data + layout.rowPitch * y);
            for (int32_t x = 0; x < imgWidth; x++) {
                row[x * 4] = imageData[(x + y * imgWidth) * 4];
                row[x * 4 + 1] = imageData[(x + y * imgWidth) * 4 + 1];
                row[x * 4 + 2] = imageData[(x + y * imgWidth) * 4 + 2];
                row[x * 4 + 3] = imageData[(x + y * imgWidth) * 4 + 3];
            }
        }

        vkUnmapMemory(device->getDevice(), tex_obj->mem);
        stbi_image_free(imageData);
    }


    tex_obj->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkCommandPoolCreateInfo cmdPoolCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queue_family_index,
    };

    VkCommandPool cmdPool;
    CALL_VK(vkCreateCommandPool(device->getDevice(), &cmdPoolCreateInfo, nullptr,
                                &cmdPool));

    VkCommandBuffer gfxCmd;
    const VkCommandBufferAllocateInfo cmd = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = cmdPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
    };

    CALL_VK(vkAllocateCommandBuffers(device->getDevice(), &cmd, &gfxCmd));
    VkCommandBufferBeginInfo cmd_buf_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr};
    CALL_VK(vkBeginCommandBuffer(gfxCmd, &cmd_buf_info));

    // If linear is supported, we are done
    VkImage stageImage = VK_NULL_HANDLE;
    VkDeviceMemory stageMem = VK_NULL_HANDLE;
    if (!needBlit) {
        BenderHelpers::setImageLayout(gfxCmd, tex_obj->image, VK_IMAGE_LAYOUT_PREINITIALIZED,
                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                      VK_PIPELINE_STAGE_HOST_BIT,
                                      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    } else {
        // save current image and mem as staging image and memory
        stageImage = tex_obj->image;
        stageMem = tex_obj->mem;
        tex_obj->image = VK_NULL_HANDLE;
        tex_obj->mem = VK_NULL_HANDLE;

        // Create a tile texture to blit into
        image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_create_info.usage =
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        CALL_VK(vkCreateImage(device->getDevice(), &image_create_info, nullptr,
                              &tex_obj->image));
        vkGetImageMemoryRequirements(device->getDevice(), tex_obj->image, &mem_reqs);

        mem_alloc.allocationSize = mem_reqs.size;
        VK_CHECK(AllocateMemoryTypeFromProperties(
                device, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                &mem_alloc.memoryTypeIndex));
        CALL_VK(
                vkAllocateMemory(device->getDevice(), &mem_alloc, nullptr, &tex_obj->mem));
        CALL_VK(vkBindImageMemory(device->getDevice(), tex_obj->image, tex_obj->mem, 0));

        // transitions image out of UNDEFINED type
        BenderHelpers::setImageLayout(gfxCmd, stageImage, VK_IMAGE_LAYOUT_PREINITIALIZED,
                                      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                      VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
        BenderHelpers::setImageLayout(gfxCmd, tex_obj->image, VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                      VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
        VkImageCopy bltInfo{
                .srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .srcSubresource.mipLevel = 0,
                .srcSubresource.baseArrayLayer = 0,
                .srcSubresource.layerCount = 1,
                .srcOffset.x = 0,
                .srcOffset.y = 0,
                .srcOffset.z = 0,
                .dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .dstSubresource.mipLevel = 0,
                .dstSubresource.baseArrayLayer = 0,
                .dstSubresource.layerCount = 1,
                .dstOffset.x = 0,
                .dstOffset.y = 0,
                .dstOffset.z = 0,
                .extent.width = imgWidth,
                .extent.height = imgHeight,
                .extent.depth = 1,
        };
        vkCmdCopyImage(gfxCmd, stageImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       tex_obj->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                       &bltInfo);

        BenderHelpers::setImageLayout(gfxCmd, tex_obj->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                      VK_PIPELINE_STAGE_TRANSFER_BIT,
                                      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    }

    CALL_VK(vkEndCommandBuffer(gfxCmd));
    VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    VkFence fence;
    CALL_VK(vkCreateFence(device->getDevice(), &fenceInfo, nullptr, &fence));

    VkSubmitInfo submitInfo = {
            .pNext = nullptr,
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &gfxCmd,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr,
    };
    CALL_VK(vkQueueSubmit(device->getQueue(), 1, &submitInfo, fence) != VK_SUCCESS);
    CALL_VK(vkWaitForFences(device->getDevice(), 1, &fence, VK_TRUE, 100000000) !=
            VK_SUCCESS);
    vkDestroyFence(device->getDevice(), fence, nullptr);

    vkFreeCommandBuffers(device->getDevice(), cmdPool, 1, &gfxCmd);
    vkDestroyCommandPool(device->getDevice(), cmdPool, nullptr);
    if (stageImage != VK_NULL_HANDLE) {
        vkDestroyImage(device->getDevice(), stageImage, nullptr);
        vkFreeMemory(device->getDevice(), stageMem, nullptr);
    }
    return VK_SUCCESS;
}

void createTexture(BenderKit::Device *device, android_app *androidAppCtx) {
    for (uint32_t i = 0; i < TEXTURE_COUNT; i++) {
        LoadTextureFromFile(texFiles[i], androidAppCtx, &textures[i], device, VK_IMAGE_USAGE_SAMPLED_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        const VkSamplerCreateInfo sampler = {
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext = nullptr,
                .magFilter = VK_FILTER_NEAREST,
                .minFilter = VK_FILTER_NEAREST,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .mipLodBias = 0.0f,
                .maxAnisotropy = 1,
                .compareOp = VK_COMPARE_OP_NEVER,
                .minLod = 0.0f,
                .maxLod = 0.0f,
                .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
                .unnormalizedCoordinates = VK_FALSE,
        };
        VkImageViewCreateInfo view = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .image = textures[i].image,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = kTexFmt,
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

        CALL_VK(vkCreateSampler(device->getDevice(), &sampler, nullptr,
                                &textures[i].sampler));
        CALL_VK(
                vkCreateImageView(device->getDevice(), &view, nullptr, &textures[i].view));
    }
}
