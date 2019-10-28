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

#ifndef BENDER_BASE_RENDERER_H_
#define BENDER_BASE_RENDERER_H_

#include "vulkan_wrapper.h"
#include "bender_kit.h"

class Renderer {
 public:
  Renderer(BenderKit::Device *device);
  ~Renderer();
  void begin();
  void end();
  VkCommandBuffer getCurrentCommandBuffer();
  uint32_t getCurrentFrame();
  VkImage &getCurrentDisplayImage();
 private:
  BenderKit::Device *device_;
  VkCommandPool cmd_pool_;
  VkCommandBuffer *cmd_buffer_;
  uint32_t cmd_buffer_len_;
  VkSemaphore *acquire_image_semaphore_;
  VkSemaphore *render_finished_semaphore_;
  VkFence *fence_;
  uint32_t current_frame;
  void init();
};

#endif // _RENDERER_HPP_
