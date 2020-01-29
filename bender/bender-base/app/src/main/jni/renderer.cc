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

#include "renderer.h"
#include <android_native_app_glue.h>
#include <debug_marker.h>
#include <timing.h>

#include "trace.h"
#include "bender_helpers.h"
#include "shader_bindings.h"
#include "constants.h"

using namespace benderhelpers;

Renderer::Renderer(benderkit::Device& device)
  : device_(device)
  , default_states_(device) {
  Init();
}

Renderer::~Renderer() {
  vkFreeCommandBuffers(GetVulkanDevice(), cmd_pool_, cmd_buffer_len_,
                       cmd_buffer_);

  for (int i = 0; i < device_.GetDisplayImages().size(); i++){
    vkDestroySemaphore(device_.GetDevice(), acquire_image_semaphore_[i], nullptr);
    vkDestroySemaphore(device_.GetDevice(), render_finished_semaphore_[i], nullptr);
    vkDestroyFence(device_.GetDevice(), fence_[i], nullptr);
  }

  delete[] cmd_buffer_;
  delete[] acquire_image_semaphore_;
  delete[] render_finished_semaphore_;
  delete[] fence_;

  DestroyPool();
  vkDestroyDescriptorSetLayout(device_.GetDevice(), lights_descriptors_layout_, nullptr);

  vkDestroyPipelineCache(GetVulkanDevice(), cache_, nullptr);
  vkDestroyCommandPool(GetVulkanDevice(), cmd_pool_, nullptr);
}

void Renderer::BeginFrame() {
  timing::timer.Time("vkAcquireNextImageKHR", timing::OTHER, [this](){
    uint32_t next_index;
    CALL_VK(vkAcquireNextImageKHR(device_.GetDevice(), device_.GetSwapchain(),
                                  UINT64_MAX, acquire_image_semaphore_[GetCurrentFrame()], VK_NULL_HANDLE,
                                  &next_index));
  });

  timing::timer.Time("vkWaitForFences", timing::OTHER, [this](){
    CALL_VK(vkWaitForFences(device_.GetDevice(), 1, &fence_[GetCurrentFrame()], VK_TRUE, 100000000));
    CALL_VK(vkResetFences(device_.GetDevice(), 1, &fence_[GetCurrentFrame()]));
  });
}

void Renderer::EndFrame() {
  VkPipelineStageFlags wait_stage_mask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &acquire_image_semaphore_[GetCurrentFrame()],
      .pWaitDstStageMask = &wait_stage_mask,
      .commandBufferCount = 1,
      .pCommandBuffers = &cmd_buffer_[GetCurrentFrame()],
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &render_finished_semaphore_[GetCurrentFrame()]};

  timing::timer.Time("vkQueueSubmit", timing::OTHER, [this, submit_info](){
    CALL_VK(vkQueueSubmit(device_.GetQueue(), 1, &submit_info, fence_[GetCurrentFrame()]));
  });

  timing::timer.Time("Device::Present", timing::OTHER, [this](){
    device_.Present(&render_finished_semaphore_[GetCurrentFrame()]);
  });
}

void Renderer::BeginPrimaryCommandBufferRecording() {
  VkCommandBufferBeginInfo cmd_buffer_begin_info{
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
          .pNext = nullptr,
          .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
          .pInheritanceInfo = nullptr,
  };
  CALL_VK(vkBeginCommandBuffer(GetCurrentCommandBuffer(),
                               &cmd_buffer_begin_info));

  // transition the display image to color attachment layout
  SetImageLayout(GetCurrentCommandBuffer(),
                 device_.GetCurrentDisplayImage(),
                 VK_IMAGE_LAYOUT_UNDEFINED,
                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                 VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
}

void Renderer::EndPrimaryCommandBufferRecording() {
  SetImageLayout(GetCurrentCommandBuffer(),
                 GetCurrentDisplayImage(),
                 VK_IMAGE_LAYOUT_UNDEFINED,
                 VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                 VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

  CALL_VK(vkEndCommandBuffer(GetCurrentCommandBuffer()));
}

void Renderer::Init() {
  VkCommandPoolCreateInfo cmd_pool_create_info {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = 0,
  };
  CALL_VK(vkCreateCommandPool(device_.GetDevice(), &cmd_pool_create_info, nullptr,
                              &cmd_pool_));

  cmd_buffer_len_ = device_.GetSwapchainLength();
  cmd_buffer_ = new VkCommandBuffer[device_.GetSwapchainLength()];
  acquire_image_semaphore_ = new VkSemaphore[device_.GetSwapchainLength()];
  render_finished_semaphore_ = new VkSemaphore[device_.GetSwapchainLength()];
  fence_ = new VkFence[device_.GetSwapchainLength()];

  for (int i = 0; i < device_.GetSwapchainLength(); ++i) {
    VkCommandBufferAllocateInfo cmd_buffer_create_info {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = cmd_pool_,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    CALL_VK(vkAllocateCommandBuffers(device_.GetDevice(), &cmd_buffer_create_info,
                                     &cmd_buffer_[i]));

    debugmarker::SetObjectName(device_.GetDevice(), (uint64_t)cmd_buffer_[i],
        VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT,
        ("TEST NAME: COMMAND BUFFER" + std::to_string(i)).c_str());

    VkSemaphoreCreateInfo semaphore_create_info {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    CALL_VK(vkCreateSemaphore(device_.GetDevice(),
                              &semaphore_create_info,
                              nullptr,
                              &acquire_image_semaphore_[i]));

    CALL_VK(vkCreateSemaphore(device_.GetDevice(),
                              &semaphore_create_info,
                              nullptr,
                              &render_finished_semaphore_[i]));

    VkFenceCreateInfo fence_create_info {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    CALL_VK(
        vkCreateFence(device_.GetDevice(),
                      &fence_create_info,
                      nullptr,
                      &fence_[i]));
  }

  lights_buffer_ = std::make_unique<UniformBufferObject<LightBlock>>(device_);

  VkPipelineCacheCreateInfo pipeline_cache_create_info {
          .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
          .pNext = nullptr,
          .initialDataSize = 0,
          .pInitialData = nullptr,
          .flags = 0,  // reserved, must be 0
  };

  CALL_VK(vkCreatePipelineCache(GetVulkanDevice(), &pipeline_cache_create_info, nullptr,
                                &cache_));

  CreatePool();
  CreateLightsDescriptorSetLayout();
  CreateLightsDescriptors();
}

void Renderer::UpdateLights(glm::vec3 camera) {
  lights_buffer_->Update(GetCurrentFrame(), [&camera](auto &lights_buffer) {
      lights_buffer.pointLight.position = camera;
      lights_buffer.pointLight.color = {1.0f, 1.0f, 1.0f};
      lights_buffer.pointLight.intensity = 50.0f;
      lights_buffer.ambientLight.color = {1.0f, 1.0f, 1.0f};
      lights_buffer.ambientLight.intensity = 0.1f;
      lights_buffer.cameraPos = camera;
  });
}

void Renderer::CreatePool() {
  uint32_t max_mvp_buffers = MAX_MESHES * device_.GetDisplayImages().size();
  uint32_t max_lights_buffers = MAX_LIGHTS * device_.GetDisplayImages().size();
  uint32_t max_samplers = MAX_SAMPLERS * device_.GetDisplayImages().size();
  uint32_t max_texts = MAX_LINES_TEXTS * device_.GetDisplayImages().size();

  std::array<VkDescriptorPoolSize, 2> pool_sizes = {};
  pool_sizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  pool_sizes[0].descriptorCount = max_samplers + max_texts;
  pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  pool_sizes[1].descriptorCount = max_mvp_buffers + max_lights_buffers + max_texts;

  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = pool_sizes.size();
  pool_info.pPoolSizes = pool_sizes.data();
  pool_info.maxSets = max_mvp_buffers + max_lights_buffers + max_samplers + max_texts;

  CALL_VK(vkCreateDescriptorPool(device_.GetDevice(), &pool_info, nullptr, &descriptor_pool_));
}

void Renderer::DestroyPool() {
    vkDestroyDescriptorPool(device_.GetDevice(), descriptor_pool_, nullptr);
}

void Renderer::CreateLightsDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding light_block_layout_binding = {
          .binding = FRAGMENT_BINDING_LIGHTS,
          .descriptorCount = 1,
          .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          .pImmutableSamplers = nullptr,
          .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
  };

  std::array<VkDescriptorSetLayoutBinding, 1> bindings = {light_block_layout_binding};

  VkDescriptorSetLayoutCreateInfo layout_info = {
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
          .bindingCount = static_cast<uint32_t>(bindings.size()),
          .pBindings = bindings.data()
  };

  CALL_VK(vkCreateDescriptorSetLayout(device_.GetDevice(), &layout_info, nullptr,
                                      &lights_descriptors_layout_));
}

void Renderer::CreateLightsDescriptors() {
  std::vector<VkDescriptorSetLayout> layouts(device_.GetDisplayImages().size(), lights_descriptors_layout_);

  VkDescriptorSetAllocateInfo alloc_info = {
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
          .descriptorPool = descriptor_pool_,
          .descriptorSetCount = static_cast<uint32_t>(device_.GetDisplayImages().size()),
          .pSetLayouts = layouts.data()
  };

  lights_descriptor_sets_.resize(device_.GetDisplayImages().size());
  CALL_VK(vkAllocateDescriptorSets(device_.GetDevice(), &alloc_info, lights_descriptor_sets_.data()));

  for (size_t i = 0; i < device_.GetDisplayImages().size(); i++) {
    VkDescriptorBufferInfo light_block_info = {
            .buffer = lights_buffer_->GetBuffer(i),
            .offset = 0,
            .range = sizeof(LightBlock)
    };

    std::array<VkWriteDescriptorSet, 1> descriptor_writes = {};

    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = lights_descriptor_sets_[i];
    descriptor_writes[0].dstBinding = FRAGMENT_BINDING_LIGHTS;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &light_block_info;

    vkUpdateDescriptorSets(device_.GetDevice(), descriptor_writes.size(), descriptor_writes.data(),
                           0, nullptr);
  }
}

VkGraphicsPipelineCreateInfo Renderer::GetDefaultPipelineInfo(
        VkPipelineLayout layout,
        VkRenderPass render_pass) const {
  VkGraphicsPipelineCreateInfo pipeline_info {
          .layout = layout,
          .renderPass = render_pass,
  };

  GetDefaultStates().FillDefaultPipelineCreateInfo(&pipeline_info);

  return pipeline_info;
}

VkCommandBuffer Renderer::GetCurrentCommandBuffer() const {
  return cmd_buffer_[GetCurrentFrame()];
}

uint32_t Renderer::GetCurrentFrame() const {
  return device_.GetCurrentFrameIndex();
}

VkImage Renderer::GetCurrentDisplayImage() const {
  return device_.GetCurrentDisplayImage();
}