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
#include <functional>

#include <android_native_app_glue.h>

class Texture {
 public:
  Texture(benderkit::Device &device,
          uint8_t *imgData,
          uint32_t imgWidth,
          uint32_t imgHeight,
          VkFormat textureFormat,
          std::function<void(uint8_t *)> generator = nullptr);

  Texture(benderkit::Device &device,
          android_app &androidAppCtx,
          const char *textureFileName,
          VkFormat textureFormat,
          std::function<void(uint8_t *)> generator = nullptr);

  ~Texture();

  void cleanup();

  void onResume(benderkit::Device &device, android_app *app);

  VkImageView getImageView() const { return view_; }

  int32_t getWidth() const { return tex_width_; };

  int32_t getHeight() const { return tex_height_; };
 private:
  benderkit::Device &device_;

  const char *file_name_ = nullptr;
  VkImage image_;
  VkImageLayout image_layout_;
  VkDeviceMemory mem_;
  VkImageView view_;
  int32_t tex_width_;
  int32_t tex_height_;
  VkFormat texture_format_;

  std::function<void(uint8_t *)> generator_ = nullptr;

  unsigned char *loadFileData(android_app &app, const char *filePath);
  VkResult createTexture(uint8_t *imgData, VkImageUsageFlags usage, VkFlags required_props);
  void createImageView();
};

#endif //BENDER_BASE_TEXTURE_H
