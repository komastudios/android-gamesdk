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
#include "renderer.h"
#include <functional>

#include <android_native_app_glue.h>

#define ASTC_MAGIC 0x5CA1AB13

class Texture {
 public:
  struct ASTCHeader {
      uint8_t magic[4];
      uint8_t block_dim_x;
      uint8_t block_dim_y;
      uint8_t block_dim_z;
      uint8_t x_size[3];
      uint8_t y_size[3];
      uint8_t z_size[3];
  };

  Texture(Renderer &renderer,
          uint8_t *img_data,
          uint32_t img_width,
          uint32_t img_height,
          VkFormat texture_format);

  Texture(Renderer &renderer,
          android_app &android_app_ctx,
          const std::string &texture_file_name,
          VkFormat texture_format = VK_FORMAT_R8G8B8A8_SRGB);

  ~Texture();


  void ToggleMipmaps();

  VkImageView GetImageView() const { return view_; }

  int32_t GetWidth() const { return tex_width_; };

  int32_t GetHeight() const { return tex_height_; };

  uint32_t GetMipLevel() { return mip_levels_; }

 private:
  Renderer &renderer_;

  VkImage image_;
  VkDeviceMemory mem_;
  VkImageView view_;
  int32_t tex_width_;
  int32_t tex_height_;
  VkFormat texture_format_;

  uint32_t mip_levels_;

  unsigned char *LoadDefaultTexture();
  unsigned char *LoadFileData(android_app &app, const char *file_path);
  unsigned char *LoadASTCFileData(android_app &app, const char *file_path, uint32_t& img_bytes);
  VkResult CreateTexture(uint8_t *img_data, uint32_t img_bytes, VkImageUsageFlags usage, VkFlags required_props);
  void CreateImageView();
  void GenerateMipmaps(VkCommandBuffer *command_buffer);
};

#endif //BENDER_BASE_TEXTURE_H
