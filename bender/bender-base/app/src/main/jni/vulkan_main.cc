// Copyright 2016 Google Inc. All Rights Reserved.
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

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include "vulkan_main.h"

#include <android_native_app_glue.h>
#include <cassert>
#include <vector>
#include <array>
#include <cstring>
#include <debug_marker.h>
#include <chrono>

#include "vulkan_wrapper.h"
#include "bender_kit.h"
#include "bender_helpers.h"
#include "renderer.h"
#include "shader_state.h"
#include "polyhedron.h"
#include "mesh.h"
#include "texture.h"
#include "font.h"
#include "uniform_buffer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader_bindings.h"

using namespace BenderKit;
using namespace BenderHelpers;

/// Global Variables ...

std::vector<VkImageView> displayViews_;
std::vector<VkFramebuffer> framebuffers_;

struct Camera {
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
} camera;

VkRenderPass render_pass;

struct AttachmentBuffer {
  VkFormat format;
  VkImage image;
  VkDeviceMemory device_memory;
  VkImageView image_view;
};
AttachmentBuffer depthBuffer;

// Android Native App pointer...
android_app *androidAppCtx = nullptr;
Device *device;
Renderer *renderer;

glm::mat4 view;
glm::mat4 proj;


std::shared_ptr<ShaderState> shaders;
std::vector<Mesh *> meshes;
Font *font;

auto lastTime = std::chrono::high_resolution_clock::now();
auto currentTime = lastTime;
float frameTime;
float totalTime;

std::vector<const char *> texFiles;
std::vector<Texture *> textures;
std::vector<Material *> materials;
const std::string sample_string = "B3nDeR V1";

bool windowResized = false;

void createTextures() {
  assert(androidAppCtx != nullptr);
  assert(device != nullptr);

  for (uint32_t i = 0; i < texFiles.size(); ++i) {
    textures.push_back(new Texture(*device, androidAppCtx, texFiles[i], VK_FORMAT_R8G8B8A8_SRGB));
  }
}

void createMaterials() {
  for (uint32_t i = 0; i < textures.size(); ++i) {
    materials.push_back(new Material(*renderer, shaders, textures[i], nullptr));
  }
}

void createFrameBuffers(VkRenderPass &renderPass,
                        VkImageView depthView = VK_NULL_HANDLE) {
  // create image view for each swapchain image
  displayViews_.resize(device->getDisplayImagesSize());
  for (uint32_t i = 0; i < device->getDisplayImagesSize(); i++) {
    VkImageViewCreateInfo viewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .image = device->getDisplayImage(i),
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = device->getDisplayFormat(),
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_R,
                .g = VK_COMPONENT_SWIZZLE_G,
                .b = VK_COMPONENT_SWIZZLE_B,
                .a = VK_COMPONENT_SWIZZLE_A,
            },
        .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        .flags = 0,
    };
    CALL_VK(vkCreateImageView(device->getDevice(), &viewCreateInfo, nullptr,
                              &displayViews_[i]));
  }

  framebuffers_.resize(device->getSwapchainLength());
  for (uint32_t i = 0; i < device->getSwapchainLength(); i++) {
    VkImageView attachments[2] = {
        displayViews_[i], depthView,
    };
    VkFramebufferCreateInfo fbCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .renderPass = renderPass,
        .layers = 1,
        .attachmentCount = 2,  // 2 if using depth
        .pAttachments = attachments,
        .width = static_cast<uint32_t>(device->getDisplaySize().width),
        .height = static_cast<uint32_t>(device->getDisplaySize().height),
    };

    CALL_VK(vkCreateFramebuffer(device->getDevice(), &fbCreateInfo, nullptr,
                                &framebuffers_[i]));
  }
}


void updateCamera(Input::Data *inputData) {
  camera.rotation =
      glm::quat(glm::vec3(0.0f, inputData->deltaX / device->getDisplaySize().width, 0.0f))
          * camera.rotation;
  camera.rotation *=
      glm::quat(glm::vec3(inputData->deltaY / device->getDisplaySize().height, 0.0f, 0.0f));
  camera.rotation = glm::normalize(camera.rotation);

  glm::vec3 up = glm::normalize(camera.rotation * glm::vec3(0.0f, 1.0f, 0.0f));
  glm::vec3 right = glm::normalize(camera.rotation * glm::vec3(1.0f, 0.0f, 0.0f));
  camera.position += right * (inputData->deltaXPan / device->getDisplaySize().width) +
      up * -(inputData->deltaYPan / device->getDisplaySize().height);

  if (inputData->doubleTapHoldUpper) {
    glm::vec3 forward = glm::normalize(camera.rotation * glm::vec3(0.0f, 0.0f, -1.0f));
    camera.position += forward * 2.0f * frameTime;
  }
  else if (inputData->doubleTapHoldLower) {
    glm::vec3 forward = glm::normalize(camera.rotation * glm::vec3(0.0f, 0.0f, -1.0f));
    camera.position -= forward * 2.0f * frameTime;
  }

  float aspect_ratio = device->getDisplaySize().width / (float) device->getDisplaySize().height;
  auto horizontal_fov = glm::radians(60.0f);
  auto vertical_fov =
      static_cast<float>(2 * atan((0.5 * device->getDisplaySize().height)
                                      / (0.5 * device->getDisplaySize().width
                                          / tan(horizontal_fov / 2))));
  auto fov = (aspect_ratio > 1.0f) ? horizontal_fov : vertical_fov;

  glm::mat4 pre_rotate_mat = glm::mat4(1.0f);
  glm::vec3 rotation_axis = glm::vec3(0.0f, 0.0f, -1.0f);
  if (device->getPretransformFlag() & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
    pre_rotate_mat = glm::rotate(pre_rotate_mat, glm::radians(90.0f), rotation_axis);
  }
  else if (device->getPretransformFlag() & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
    pre_rotate_mat = glm::rotate(pre_rotate_mat, glm::radians(270.0f), rotation_axis);
  }
  else if (device->getPretransformFlag() & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) {
    pre_rotate_mat = glm::rotate(pre_rotate_mat, glm::radians(180.0f), rotation_axis);
  }

  view = pre_rotate_mat
      * glm::inverse(glm::translate(glm::mat4(1.0f), camera.position) * glm::mat4(camera.rotation));
  proj = glm::perspective(fov, aspect_ratio, 0.1f, 100.0f);
  proj[1][1] *= -1;
}

void updateInstances(Input::Data *inputData) {
  if (inputData->singleTapLowerLeft && meshes.size() > 0) {
      meshes.pop_back();
  }
  else if (inputData->singleTapLowerRight) {
    meshes.push_back(createPolyhedron(*renderer, *materials[0], 20));
    meshes[meshes.size() - 1]->translate(glm::vec3(rand() % 3, rand() % 3, rand() % 3));
  }

  for (int x = 0; x < meshes.size(); x++) {
    meshes[x]->rotate(glm::vec3(0.0f, 1.0f, 1.0f), 90 * frameTime);
    meshes[x]->translate(.02f * glm::vec3(std::sin(2 * totalTime),
                                          std::sin(x * totalTime),
                                          std::cos(2 * totalTime)));

    meshes[x]->update(renderer->getCurrentFrame(), camera.position, view, proj);
  }
  renderer->updateLights(camera.position);
}

void handleInput(Input::Data *inputData){
  updateCamera(inputData);
  updateInstances(inputData);
  inputData->singleTapLowerRight = false;
  inputData->singleTapLowerLeft = false;
}

void createShaderState() {
  VertexFormat vertex_format { {
        VertexElement::float3,
        VertexElement::float3,
        VertexElement::float2,
      },
  };

  shaders = std::make_shared<ShaderState>("mesh", vertex_format, androidAppCtx, device->getDevice());
}

void createDepthBuffer() {
  depthBuffer.format = BenderHelpers::findDepthFormat(device);
  VkImageCreateInfo imageInfo = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .extent.width = device->getDisplaySize().width,
      .extent.height = device->getDisplaySize().height,
      .extent.depth = 1,
      .mipLevels = 1,
      .arrayLayers = 1,
      .format = depthBuffer.format,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  CALL_VK(vkCreateImage(device->getDevice(), &imageInfo, nullptr, &depthBuffer.image));

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device->getDevice(), depthBuffer.image, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = memRequirements.size,
      .memoryTypeIndex = BenderHelpers::findMemoryType(memRequirements.memoryTypeBits,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                       device->getPhysicalDevice()),
  };

  CALL_VK(vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &depthBuffer.device_memory))

  vkBindImageMemory(device->getDevice(), depthBuffer.image, depthBuffer.device_memory, 0);

  VkImageViewCreateInfo viewInfo = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .pNext = nullptr,
      .image = depthBuffer.image,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = depthBuffer.format,
      .subresourceRange =
          {
              .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
      .flags = 0,
  };

  CALL_VK(vkCreateImageView(device->getDevice(), &viewInfo, nullptr, &depthBuffer.image_view));

}

bool InitVulkan(android_app *app) {
  androidAppCtx = app;

  device = new Device(app->window);
  assert(device->isInitialized());
  device->setObjectName(reinterpret_cast<uint64_t>(device->getDevice()),
      VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT, "TEST NAME: VULKAN DEVICE");

  VkAttachmentDescription color_description{
      .format = device->getDisplayFormat(),
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };

  VkAttachmentDescription depth_description{
      .format = BenderHelpers::findDepthFormat(device),
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
  };

  VkAttachmentReference color_attachment_reference = {
      .attachment = 0,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
  };

  VkAttachmentReference depth_attachment_reference = {
      .attachment = 1,
      .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
  };

  VkSubpassDescription subpass_description{
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .flags = 0,
      .inputAttachmentCount = 0,
      .pInputAttachments = nullptr,
      .colorAttachmentCount = 1,
      .pColorAttachments = &color_attachment_reference,
      .pResolveAttachments = nullptr,
      .pDepthStencilAttachment = &depth_attachment_reference,
      .preserveAttachmentCount = 0,
      .pPreserveAttachments = nullptr,
  };

  std::array<VkAttachmentDescription, 2> attachment_descriptions =
      {color_description, depth_description};

  VkRenderPassCreateInfo render_pass_createInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .pNext = nullptr,
      .attachmentCount = static_cast<uint32_t>(attachment_descriptions.size()),
      .pAttachments = attachment_descriptions.data(),
      .subpassCount = 1,
      .pSubpasses = &subpass_description,
      .dependencyCount = 0,
      .pDependencies = nullptr,
  };
  CALL_VK(vkCreateRenderPass(device->getDevice(), &render_pass_createInfo, nullptr,
                             &render_pass));

  createShaderState();

  renderer = new Renderer(*device);

  texFiles.push_back("textures/sample_texture.png");

  createTextures();

  createMaterials();

  meshes.push_back(createPolyhedron(*renderer, *materials[0], 20));

  const std::vector<float> vertexData = {
      -0.5f, -0.5f, 0.5f,          -0.5774f, -0.5774f, 0.5774f,       1.0f, 0.0f,
      0.5f, -0.5f, 0.5f,           0.5774f, -0.5774f, 0.5774f,        0.0f, 0.0f,
      0.5f, 0.5f, 0.5f,            0.5774f, 0.5774f, 0.5774f,         0.0f, 1.0f,
      -0.5f, 0.5f, 0.5f,           -0.5774f, 0.5774f, 0.5774f,      1.0f, 1.0f,
      0.0f, 0.0f, 0.0f,            0.0f, 1.0f, 0.0f,           0.0f, 0.0f,
  };

  const std::vector<u_int16_t> indexData = {
      1, 2, 4, 2, 1, 0, 0, 3, 2, 2, 3, 4, 3, 0, 4, 0, 1, 4
  };
  
  createDepthBuffer();

  createFrameBuffers(render_pass, depthBuffer.image_view);

  texFiles.push_back("textures/sample_texture.png");

  createTextures();

  font = new Font(*renderer, androidAppCtx, FONT_SDF_PATH, FONT_INFO_PATH);

  return true;
}

bool IsVulkanReady(void) { return device != nullptr && device->isInitialized(); }

void DeleteVulkan(void) {
  delete renderer;
  for (int x = 0; x < meshes.size(); x++) {
    delete meshes[x];
  }
  delete font;

  shaders->cleanup();
  shaders.reset();

  for (int i = 0; i < device->getSwapchainLength(); ++i) {
    vkDestroyImageView(device->getDevice(), displayViews_[i], nullptr);
    vkDestroyFramebuffer(device->getDevice(), framebuffers_[i], nullptr);
  }

  vkDestroyImageView(device->getDevice(), depthBuffer.image_view, nullptr);
  vkDestroyImage(device->getDevice(), depthBuffer.image, nullptr);
  vkFreeMemory(device->getDevice(), depthBuffer.device_memory, nullptr);

  vkDestroyRenderPass(device->getDevice(), render_pass, nullptr);

  delete device;
  device = nullptr;
}

bool VulkanDrawFrame(Input::Data *inputData) {
  TRACE_BEGIN_SECTION("Draw Frame");
  if (windowResized) {
    OnOrientationChange();
  }
  currentTime = std::chrono::high_resolution_clock::now();
  frameTime = std::chrono::duration<float>(currentTime - lastTime).count();;
  lastTime = currentTime;
  totalTime += frameTime;

  handleInput(inputData);

  renderer->beginFrame();
  renderer->beginPrimaryCommandBufferRecording();

  // Now we start a renderpass. Any draw command has to be recorded in a
  // renderpass
  std::array<VkClearValue, 2> clear_values = {};
  clear_values[0].color = {{0.0f, 0.34f, 0.90f, 1.0}};
  clear_values[1].depthStencil = {1.0f, 0};

  VkRenderPassBeginInfo render_pass_beginInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = render_pass,
      .framebuffer = framebuffers_[renderer->getCurrentFrame()],
      .renderArea = {.offset =
          {
              .x = 0, .y = 0,
          },
          .extent = device->getDisplaySize()},
      .clearValueCount = static_cast<uint32_t>(clear_values.size()),
      .pClearValues = clear_values.data(),
  };

  vkCmdBeginRenderPass(renderer->getCurrentCommandBuffer(), &render_pass_beginInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  device->insertDebugMarker(renderer->getCurrentCommandBuffer(), "TEST MARKER: PIPELINE BINDING",
                            {1.0f, 0.0f, 1.0f, 0.0f});

  for (int x = 0; x < meshes.size(); x++){
    meshes[x]->updatePipeline(render_pass);
    meshes[x]->submitDraw(renderer->getCurrentCommandBuffer(), renderer->getCurrentFrame());
  }
  font->drawString(sample_string, 2.0f, 0.0f, 0.0f,
                   renderer->getCurrentCommandBuffer(), render_pass, renderer->getCurrentFrame());

  vkCmdEndRenderPass(renderer->getCurrentCommandBuffer());

  renderer->endPrimaryCommandBufferRecording();
  renderer->endFrame();

  TRACE_END_SECTION();

  return true;
}

void ResizeCallback(ANativeActivity *activity, ANativeWindow *window){
  windowResized = true;
}

void OnOrientationChange() {
  vkDeviceWaitIdle(device->getDevice());

  for (int i = 0; i < device->getSwapchainLength(); ++i) {
    vkDestroyImageView(device->getDevice(), displayViews_[i], nullptr);
    vkDestroyFramebuffer(device->getDevice(), framebuffers_[i], nullptr);
  }
  vkDestroyImageView(device->getDevice(), depthBuffer.image_view, nullptr);
  vkDestroyImage(device->getDevice(), depthBuffer.image, nullptr);
  vkFreeMemory(device->getDevice(), depthBuffer.device_memory, nullptr);

  device->createSwapChain(device->getSwapchain());
  createDepthBuffer();
  createFrameBuffers(render_pass, depthBuffer.image_view);
  windowResized = false;
}