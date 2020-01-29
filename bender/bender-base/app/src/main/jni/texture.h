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
          uint8_t *img_data,
          uint32_t img_width,
          uint32_t img_height,
          VkFormat texture_format,
          std::function<void(uint8_t *)> generator = nullptr);

  Texture(benderkit::Device &device,
          android_app &android_app_ctx,
          const std::string &texture_file_name,
          VkFormat texture_format,
          std::function<void(uint8_t *)> generator = nullptr);

  ~Texture();

  void Cleanup();

  void OnResume(benderkit::Device &device, android_app *app);

  VkImageView GetImageView() const { return view_; }

  int32_t GetWidth() const { return tex_width_; };

  int32_t GetHeight() const { return tex_height_; };

 private:
  benderkit::Device &device_;

  std::string file_name_;
  VkImage image_;
  VkImageLayout image_layout_;
  VkDeviceMemory mem_;
  VkImageView view_;
  int32_t tex_width_;
  int32_t tex_height_;
  VkFormat texture_format_;

  std::function<void(uint8_t *)> generator_ = nullptr;

  unsigned char *LoadFileData(android_app &app, const char *file_path);
  VkResult CreateTexture(uint8_t *img_data, VkImageUsageFlags usage, VkFlags required_props);
  void CreateImageView();
};

#endif //BENDER_BASE_TEXTURE_H
