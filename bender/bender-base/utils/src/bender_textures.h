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
#ifndef BENDER_BASE_UTILS_SRC_BENDER_TEXTURES_H_
#define BENDER_BASE_UTILS_SRC_BENDER_TEXTURES_H_


#include "vulkan_wrapper.h"
#include "bender_kit.h"

#include <android_native_app_glue.h>

typedef struct texture_object {
    VkSampler sampler;
    VkImage image;
    VkImageLayout imageLayout;
    VkDeviceMemory mem;
    VkImageView view;
    int32_t tex_width;
    int32_t tex_height;
} texture_object;

static const VkFormat kTexFmt = VK_FORMAT_R8G8B8A8_SRGB;
#define TEXTURE_COUNT 1

VkResult LoadTextureFromFile(const char* filePath, android_app *androidAppCtx,
                             struct texture_object* tex_obj, BenderKit::Device *device,
                             VkImageUsageFlags usage, VkFlags required_props);

void createSampler(BenderKit::Device *device, texture_object *texture);

#endif //BENDER_BASE_TEXTURE_LOADER_H
