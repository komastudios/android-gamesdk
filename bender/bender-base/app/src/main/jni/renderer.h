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

#include "glm/glm.hpp"
#include "vulkan_wrapper.h"
#include "bender_kit.h"
#include "uniform_buffer.h"

struct PointLight{
    alignas(16) float intensity;
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
};

struct AmbientLight{
    alignas(16) float intensity;
    alignas(16) glm::vec3 color;
};

struct LightBlock{
    PointLight pointLight;
    AmbientLight ambientLight;
    alignas(16) glm::vec3 cameraPos;
};

class Renderer {
 public:
  Renderer(BenderKit::Device& device);
  ~Renderer();

  void beginFrame();
  void endFrame();

  void beginPrimaryCommandBufferRecording();
  void endPrimaryCommandBufferRecording();

  void updateLights(glm::vec3 camera);

  BenderKit::Device& getDevice() const { return device_; };
  VkDevice getVulkanDevice() const { return device_.getDevice(); }
  VkCommandBuffer getCurrentCommandBuffer() const;
  uint32_t getCurrentFrame() const;

  VkDescriptorPool getDescriptorPool() const { return descriptor_pool_; };
  VkDescriptorSetLayout getLightsDescriptorSetLayout() const { return lights_descriptors_layout_; }
  VkDescriptorSet getLightsDescriptorSet(uint_t frame_index) const { return lights_descriptor_sets_[frame_index]; }

private:
  BenderKit::Device& device_;
  VkCommandPool cmd_pool_;
  VkCommandBuffer *cmd_buffer_;
  uint32_t cmd_buffer_len_;
  VkSemaphore *acquire_image_semaphore_;
  VkSemaphore *render_finished_semaphore_;
  VkFence *fence_;

  VkDescriptorPool descriptor_pool_;
  VkDescriptorSetLayout lights_descriptors_layout_;
  std::vector<VkDescriptorSet> lights_descriptor_sets_;
  std::unique_ptr<UniformBufferObject<LightBlock>> lights_buffer_;

  void init();

  void createPool();

  void destroyPool();

  void createLightsDescriptorSetLayout();

  void createLightsDescriptors();

  VkImage getCurrentDisplayImage() const;
};

#endif // _RENDERER_HPP_
