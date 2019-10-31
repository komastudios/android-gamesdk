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

#ifndef BENDER_BASE_TEXTURE_H
#define BENDER_BASE_TEXTURE_H

#include "vulkan_wrapper.h"
#include "bender_kit.h"

#include <android_native_app_glue.h>

class Texture {
public:
    Texture();
    Texture(const char *texture_file_name, BenderKit::Device *device, android_app *androidAppCtx);
    ~Texture();
    VkImageView getImageView();
    VkSampler getSampler();

private:
    BenderKit::Device *device_;
    android_app *android_app_;
    VkSampler sampler_;
    VkImage image_;
    VkImageLayout image_layout_;
    VkDeviceMemory mem_;
    VkImageView view_;
    int32_t tex_width_;
    int32_t tex_height_;
    static const VkFormat kTexFmt = VK_FORMAT_R8G8B8A8_SRGB;
    VkResult allocateMemoryTypeFromProperties(uint32_t typeBits,
                                              VkFlags requirements_mask,
                                              uint32_t* typeIndex);
    VkResult loadTextureFromFile(const char* filePath,
                                 VkImageUsageFlags usage, VkFlags required_props);
};

#endif //BENDER_BASE_TEXTURE_H
