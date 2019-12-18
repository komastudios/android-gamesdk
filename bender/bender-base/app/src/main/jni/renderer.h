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
#include "uniform_buffer.h"
#include "default_states.h"

#include "glm/glm.hpp"

struct alignas(16) PointLight{
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
    alignas(4) float intensity;
};

struct alignas(16) AmbientLight{
    alignas(16) glm::vec3 color;
    alignas(4) float intensity;
};

struct alignas(16) LightBlock{
    PointLight pointLight;
    AmbientLight ambientLight;
    alignas(16) glm::vec3 cameraPos;
};

class Renderer {
public:
  Renderer(benderkit::Device& device);
  ~Renderer();

  void BeginFrame();
  void EndFrame();

  void BeginPrimaryCommandBufferRecording();
  void EndPrimaryCommandBufferRecording();

  void UpdateLights(glm::vec3 camera);

  benderkit::Device& GetDevice() const { return device_; };
  VkDevice GetVulkanDevice() const { return device_.GetDevice(); }
  VkCommandBuffer GetCurrentCommandBuffer() const;
  uint32_t GetCurrentFrame() const;

  VkDescriptorPool GetDescriptorPool() const { return descriptor_pool_; };
  VkDescriptorSetLayout GetLightsDescriptorSetLayout() const { return lights_descriptors_layout_; }
  VkDescriptorSet GetLightsDescriptorSet(uint_t frame_index) const { return lights_descriptor_sets_[frame_index]; }

  const DefaultStates &GetDefaultStates() const {
    return default_states_;
  }

  VkPipelineCache GetPipelineCache() const { return cache_; }
  VkGraphicsPipelineCreateInfo GetDefaultPipelineInfo(
          VkPipelineLayout layout,
          VkRenderPass render_pass) const;

private:
  benderkit::Device& device_;

  VkCommandPool cmd_pool_;
  VkCommandBuffer *cmd_buffer_;
  uint32_t cmd_buffer_len_;

  VkSemaphore *acquire_image_semaphore_;
  VkSemaphore *render_finished_semaphore_;

  VkFence *fence_;

  VkPipelineCache cache_;

  VkDescriptorPool descriptor_pool_;
  VkDescriptorSetLayout lights_descriptors_layout_;
  std::vector<VkDescriptorSet> lights_descriptor_sets_;
  std::unique_ptr<UniformBufferObject<LightBlock>> lights_buffer_;

  DefaultStates default_states_;

  void Init();

  void CreatePool();

  void DestroyPool();

  void CreateLightsDescriptorSetLayout();

  void CreateLightsDescriptors();

  VkImage GetCurrentDisplayImage() const;
};

#endif // BENDER_BASE_RENDERER_HPP_
