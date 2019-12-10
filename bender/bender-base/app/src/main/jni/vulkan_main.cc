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

#include "vulkan_wrapper.h"
#include "bender_kit.h"
#include "bender_helpers.h"
#include "debug_marker.h"
#include "timing.h"

#include "renderer.h"
#include "shader_state.h"
#include "polyhedron.h"
#include "mesh.h"
#include "texture.h"
#include "font.h"
#include "uniform_buffer.h"
#include "button.h"
#include "userinterface.h"
#include "shader_bindings.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <android_native_app_glue.h>
#include <cassert>
#include <vector>
#include <array>
#include <cstring>
#include <chrono>

using namespace BenderKit;
using namespace BenderHelpers;

/// Global Variables ...

std::vector<VkImageView> display_views;
std::vector<VkFramebuffer> framebuffers;

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
AttachmentBuffer depth_buffer;

// Android Native App pointer...
android_app *android_app_ctx = nullptr;
Device *device;
Renderer *renderer;

const glm::mat4 kIdentityMat4 = glm::mat4(1.0f);
float aspect_ratio;
float fov;
glm::mat4 view;
glm::mat4 proj;

std::shared_ptr<ShaderState> shaders;
std::vector<Mesh *> meshes;
Font *font;

auto last_time = std::chrono::high_resolution_clock::now();
auto current_time = last_time;
float frame_time;
float total_time;

std::vector<const char *> tex_files;
std::vector<std::shared_ptr<Texture>> textures;
std::vector<std::shared_ptr<Material>> materials;

std::vector<std::shared_ptr<Material>> baseline_materials;
uint32_t materials_idx = 0;

static const std::array<int, 5> kAllowedPolyFaces = {4, 6, 8, 12, 20};
uint32_t poly_faces_idx = 0;

UserInterface *user_interface;
uint32_t mesh_info_handle;
uint32_t fps_handle;

bool window_resized = false;

void MoveForward() {
  glm::vec3 forward = glm::normalize(camera.rotation * glm::vec3(0.0f, 0.0f, -1.0f));
  camera.position += forward * 2.0f * frame_time;
}
void MoveBackward() {
  glm::vec3 forward = glm::normalize(camera.rotation * glm::vec3(0.0f, 0.0f, -1.0f));
  camera.position -= forward * 2.0f * frame_time;
}
void StrafeLeft() {
  glm::vec3 right = glm::normalize(camera.rotation * glm::vec3(1.0f, 0.0f, 0.0f));
  camera.position -= right * (20.0f / device->getDisplaySize().width);
}
void StrafeRight() {
  glm::vec3 right = glm::normalize(camera.rotation * glm::vec3(1.0f, 0.0f, 0.0f));
  camera.position += right * (20.0f / device->getDisplaySize().width);
}
void StrafeUp() {
  glm::vec3 up = glm::normalize(camera.rotation * glm::vec3(0.0f, 1.0f, 0.0f));
  camera.position += up * (20.0f / device->getDisplaySize().height);
}
void StrafeDown() {
  glm::vec3 up = glm::normalize(camera.rotation * glm::vec3(0.0f, 1.0f, 0.0f));
  camera.position -= up * (20.0f / device->getDisplaySize().height);
}
void CreateInstance() {
  meshes.push_back(createPolyhedron(*renderer, baseline_materials[materials_idx], kAllowedPolyFaces[poly_faces_idx]));
  meshes[meshes.size() - 1]->translate(glm::vec3(rand() % 3, rand() % 3, rand() % 3));
}
void DeleteInstance() {
  if (meshes.size() > 0) {
    meshes.pop_back();
  }
}
void ChangePolyhedralComplexity() {
  poly_faces_idx = (poly_faces_idx + 1) % kAllowedPolyFaces.size();
  for (uint32_t i = 0; i < meshes.size(); i++) {
    swapPolyhedron(*meshes[i], kAllowedPolyFaces[poly_faces_idx]);
  }
}
void ChangeMaterialComplexity() {
  materials_idx = (materials_idx + 1) % baseline_materials.size();
  for (uint32_t i = 0; i < meshes.size(); i++) {
      meshes[i]->swapMaterial(baseline_materials[materials_idx]);
  }
}

void CreateButtons(){
  Button::setScreenResolution(device->getDisplaySizeOriented());

  uint32_t handle;
  handle = user_interface->registerHandleForButton(-.7, .2, .7, .2, "<--");
  user_interface->setButtonOnHold(handle, StrafeLeft);
  handle = user_interface->registerHandleForButton(-.2, .2, .7, .2, "-->");
  user_interface->setButtonOnHold(handle, StrafeRight);
  handle = user_interface->registerHandleForButton(-.47, .2, .6, .2, "^");
  user_interface->setButtonOnHold(handle, StrafeUp);
  handle = user_interface->registerHandleForButton(-.47, .2, .85, .2, "O");
  user_interface->setButtonOnHold(handle, StrafeDown);
  handle = user_interface->registerHandleForButton(.43, .2, .65, .2, "Forward");
  user_interface->setButtonOnHold(handle, MoveForward);
  handle = user_interface->registerHandleForButton(.43, .2, .85, .2, "Backward");
  user_interface->setButtonOnHold(handle, MoveBackward);
  handle = user_interface->registerHandleForButton(-.2, .2, .4, .2, "+1 Mesh");
  user_interface->setButtonOnUp(handle, CreateInstance);
  handle = user_interface->registerHandleForButton(-.7, .2, .4, .2, "-1 Mesh");
  user_interface->setButtonOnUp(handle, DeleteInstance);
  handle = user_interface->registerHandleForButton(.5, .2, .2, .2, "Poly Switch");
  user_interface->setButtonOnUp(handle, ChangePolyhedralComplexity);
  handle = user_interface->registerHandleForButton(.5, .2, .4, .2, "Tex Switch");
  user_interface->setButtonOnUp(handle, ChangeMaterialComplexity);
}

void CreateUserInterface() {
  user_interface = new UserInterface(renderer, *font);

  mesh_info_handle = user_interface->registerHandleForTextField(1.0f, -.98f, -.98f);
  fps_handle = user_interface->registerHandleForTextField(1.0f, -0.98f, -0.88f);
  CreateButtons();
}

void CreateTextures() {
  Timing::timer.time("Texture Creation", Timing::OTHER, [](){
    assert(android_app_ctx != nullptr);
    assert(device != nullptr);

    for (uint32_t i = 0; i < tex_files.size(); ++i) {
      textures.push_back(std::make_shared<Texture>(*device, *android_app_ctx, tex_files[i], VK_FORMAT_R8G8B8A8_SRGB));
    }
  });
}

void CreateMaterials() {
  Timing::timer.time("Materials Creation", Timing::OTHER, [](){
    baseline_materials.push_back(std::make_shared<Material>(*renderer, shaders, nullptr));
    baseline_materials.push_back(std::make_shared<Material>(*renderer, shaders, nullptr, glm::vec3(0.8, 0.0, 0.5)));
    baseline_materials.push_back(std::make_shared<Material>(*renderer, shaders, textures[0]));
    baseline_materials.push_back(std::make_shared<Material>(*renderer, shaders, textures[0], glm::vec3(0.8, 0.0, 0.5)));

    for (uint32_t i = 0; i < textures.size(); ++i) {
      materials.push_back(std::make_shared<Material>(*renderer, shaders, textures[i]));
    }
  });
}

void CreateFrameBuffers(VkRenderPass &render_pass, VkImageView depth_view = VK_NULL_HANDLE) {
  display_views.resize(device->getDisplayImages().size());
  for (uint32_t i = 0; i < device->getDisplayImages().size(); i++) {
    VkImageViewCreateInfo view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .image = device->getDisplayImages()[i],
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
    CALL_VK(vkCreateImageView(device->getDevice(), &view_create_info, nullptr,
                              &display_views[i]));
  }

  framebuffers.resize(device->getSwapchainLength());
  for (uint32_t i = 0; i < device->getSwapchainLength(); i++) {
    VkImageView attachments[2] = {
        display_views[i], depth_view,
    };
    VkFramebufferCreateInfo fb_create_info{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .renderPass = render_pass,
        .layers = 1,
        .attachmentCount = 2,  // 2 if using depth
        .pAttachments = attachments,
        .width = static_cast<uint32_t>(device->getDisplaySize().width),
        .height = static_cast<uint32_t>(device->getDisplaySize().height),
    };

    CALL_VK(vkCreateFramebuffer(device->getDevice(), &fb_create_info, nullptr,
                                &framebuffers[i]));
  }
}

void UpdateCamera(Input::Data *input_data) {
  if ((input_data->lastButton != nullptr && input_data->lastInputCount > 1) || input_data->lastButton == nullptr){
    camera.rotation =
        glm::quat(glm::vec3(0.0f, input_data->deltaX / device->getDisplaySize().width, 0.0f))
            * camera.rotation;
    camera.rotation *=
        glm::quat(glm::vec3(input_data->deltaY / device->getDisplaySize().height, 0.0f, 0.0f));
    camera.rotation = glm::normalize(camera.rotation);
  }

  glm::mat4 pre_rotate_mat = kIdentityMat4;
  glm::vec3 rotation_axis = glm::vec3(0.0f, 0.0f, -1.0f);
  if (device->getPretransformFlag() & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
    pre_rotate_mat = glm::rotate(pre_rotate_mat, glm::half_pi<float>(), rotation_axis);
  }
  else if (device->getPretransformFlag() & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
    pre_rotate_mat = glm::rotate(pre_rotate_mat, glm::three_over_two_pi<float>(), rotation_axis);
  }
  else if (device->getPretransformFlag() & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) {
    pre_rotate_mat = glm::rotate(pre_rotate_mat, glm::pi<float>(), rotation_axis);
  }

  view = pre_rotate_mat
      * glm::inverse(glm::translate(glm::mat4(1.0f), camera.position) * glm::mat4(camera.rotation));
  proj = glm::perspective(fov, aspect_ratio, 0.1f, 100.0f);
  proj[1][1] *= -1;
}

void UpdateInstances(Input::Data *input_data) {
  for (int x = 0; x < meshes.size(); x++) {
    meshes[x]->rotate(glm::vec3(0.0f, 1.0f, 1.0f), 90 * frame_time);
    meshes[x]->translate(.02f * glm::vec3(std::sin(2 * total_time),
                                          std::sin(x * total_time),
                                          std::cos(2 * total_time)));

    meshes[x]->update(renderer->getCurrentFrame(), camera.position, view, proj);
  }
  renderer->updateLights(camera.position);
}

void HandleInput(Input::Data *input_data){
  UpdateCamera(input_data);
  UpdateInstances(input_data);
}

void CreateShaderState() {
  VertexFormat vertex_format { {
        VertexElement::float3,
        VertexElement::float3,
        VertexElement::float2,
      },
  };

  shaders = std::make_shared<ShaderState>("mesh", vertex_format, *android_app_ctx, device->getDevice());
}

void CreateDepthBuffer() {
  depth_buffer.format = BenderHelpers::findDepthFormat(device);
  VkImageCreateInfo image_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .extent.width = device->getDisplaySize().width,
      .extent.height = device->getDisplaySize().height,
      .extent.depth = 1,
      .mipLevels = 1,
      .arrayLayers = 1,
      .format = depth_buffer.format,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  CALL_VK(vkCreateImage(device->getDevice(), &image_info, nullptr, &depth_buffer.image));

  VkMemoryRequirements mem_requirements;
  vkGetImageMemoryRequirements(device->getDevice(), depth_buffer.image, &mem_requirements);

  VkMemoryAllocateInfo alloc_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = mem_requirements.size,
      .memoryTypeIndex = BenderHelpers::findMemoryType(mem_requirements.memoryTypeBits,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                       device->getPhysicalDevice()),
  };

  CALL_VK(vkAllocateMemory(device->getDevice(), &alloc_info, nullptr, &depth_buffer.device_memory))

  vkBindImageMemory(device->getDevice(), depth_buffer.image, depth_buffer.device_memory, 0);

  VkImageViewCreateInfo view_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .pNext = nullptr,
      .image = depth_buffer.image,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = depth_buffer.format,
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

  CALL_VK(vkCreateImageView(device->getDevice(), &view_info, nullptr, &depth_buffer.image_view));

}

bool InitVulkan(android_app *app) {
  Timing::timer.time("Initialization", Timing::OTHER, [app](){
    android_app_ctx = app;

    device = new Device(app->window);
    assert(device->isInitialized());
    device->setObjectName(reinterpret_cast<uint64_t>(device->getDevice()),
                          VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT, "TEST NAME: VULKAN DEVICE");

  aspect_ratio = device->getDisplaySize().width / (float) device->getDisplaySize().height;
  auto horizontal_fov = glm::radians(60.0f);
  auto vertical_fov =
          static_cast<float>(2 * atan((0.5 * device->getDisplaySize().height)
                                      / (0.5 * device->getDisplaySize().width
                                         / tan(horizontal_fov / 2))));
  fov = (aspect_ratio > 1.0f) ? horizontal_fov : vertical_fov;

    VkAttachmentDescription color_description {
        .format = device->getDisplayFormat(),
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentDescription depth_description {
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

    VkRenderPassCreateInfo render_pass_create_info{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .attachmentCount = static_cast<uint32_t>(attachment_descriptions.size()),
        .pAttachments = attachment_descriptions.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass_description,
        .dependencyCount = 0,
        .pDependencies = nullptr,
    };
    CALL_VK(vkCreateRenderPass(device->getDevice(), &render_pass_create_info, nullptr,
                               &render_pass));

    CreateShaderState();

    renderer = new Renderer(*device);

    Timing::timer.time("Mesh Creation", Timing::OTHER, [](){
      tex_files.push_back("textures/sample_texture.png");
      tex_files.push_back("textures/sample_texture2.png");

      CreateTextures();

      CreateMaterials();

      Timing::timer.time("Create Polyhedron", Timing::OTHER, [](){
        meshes.push_back(createPolyhedron(*renderer, baseline_materials[materials_idx], kAllowedPolyFaces[poly_faces_idx]));
      });
    });

    CreateDepthBuffer();

    CreateFrameBuffers(render_pass, depth_buffer.image_view);

    font = new Font(*renderer, *android_app_ctx, FONT_SDF_PATH, FONT_INFO_PATH);

    CreateUserInterface();

  });

  Timing::printEvent(*Timing::timer.getLastMajorEvent());
  return true;
}

bool IsVulkanReady(void) { return device != nullptr && device->isInitialized(); }

void DeleteVulkan(void) {
  vkDeviceWaitIdle(device->getDevice());

  delete renderer;
  for (int x = 0; x < meshes.size(); x++) {
    delete meshes[x];
  }
  delete font;

  shaders.reset();

  for (int i = 0; i < device->getSwapchainLength(); ++i) {
    vkDestroyImageView(device->getDevice(), display_views[i], nullptr);
    vkDestroyFramebuffer(device->getDevice(), framebuffers[i], nullptr);
  }

  vkDestroyImageView(device->getDevice(), depth_buffer.image_view, nullptr);
  vkDestroyImage(device->getDevice(), depth_buffer.image, nullptr);
  vkFreeMemory(device->getDevice(), depth_buffer.device_memory, nullptr);

  vkDestroyRenderPass(device->getDevice(), render_pass, nullptr);

  textures.clear();
  baseline_materials.clear();
  materials.clear();
  Material::cleanup();

  delete device;
  device = nullptr;
}

bool VulkanDrawFrame(Input::Data *input_data) {
  if (window_resized) {
    OnOrientationChange();
  }
  current_time = std::chrono::high_resolution_clock::now();
  frame_time = std::chrono::duration<float>(current_time - last_time).count();
  last_time = current_time;
  total_time += frame_time;

  Timing::timer.time("Handle Input", Timing::OTHER, [input_data]() {
    HandleInput(input_data);
  });

  renderer->beginFrame();
  Timing::timer.time("Start Frame", Timing::START_FRAME, []() {
    Timing::timer.time("PrimaryCommandBufferRecording", Timing::OTHER, []() {
      renderer->beginPrimaryCommandBufferRecording();

      // Now we start a renderpass. Any draw command has to be recorded in a
      // renderpass
      std::array<VkClearValue, 2> clear_values = {};
      clear_values[0].color = {{0.0f, 0.34f, 0.90f, 1.0}};
      clear_values[1].depthStencil = {1.0f, 0};

      VkRenderPassBeginInfo render_pass_begin_info{
          .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
          .pNext = nullptr,
          .renderPass = render_pass,
          .framebuffer = framebuffers[renderer->getCurrentFrame()],
          .renderArea = {.offset =
              {
                  .x = 0, .y = 0,
              },
              .extent = device->getDisplaySize()},
          .clearValueCount = static_cast<uint32_t>(clear_values.size()),
          .pClearValues = clear_values.data(),
      };

      Timing::timer.time("Render Pass", Timing::OTHER, [render_pass_begin_info]() {
        vkCmdBeginRenderPass(renderer->getCurrentCommandBuffer(), &render_pass_begin_info,
                             VK_SUBPASS_CONTENTS_INLINE);

        device->insertDebugMarker(renderer->getCurrentCommandBuffer(),
                                  "TEST MARKER: PIPELINE BINDING",
                                  {1.0f, 0.0f, 1.0f, 0.0f});

        int total_triangles = 0;
        for (int x = 0; x < meshes.size(); x++) {
          meshes[x]->updatePipeline(render_pass);
          meshes[x]->submitDraw(renderer->getCurrentCommandBuffer(), renderer->getCurrentFrame());
          total_triangles += meshes[x]->getTrianglesCount();
        }

        char mesh_info[50];
        const char *mesh_noun = meshes.size() == 1 ? "mesh" : "meshes";
        const char *poly_noun = "faces/polyhedron";
        const char *triangle_noun = total_triangles == 1 ? "triangle" : "triangles";

        sprintf(mesh_info, "%d %s, %d %s, %d %s", (int) meshes.size(), mesh_noun,
                kAllowedPolyFaces[poly_faces_idx], poly_noun, total_triangles, triangle_noun);
        user_interface->updateTextField(mesh_info_handle, mesh_info);

        char fps_string[50];
        int fps;
        float frame_time;
        Timing::timer.getFramerate(500,
                                   Timing::timer.getLastMajorEvent()->number,
                                   &fps,
                                   &frame_time);
        sprintf(fps_string, "%2.d FPS  %.3f ms", fps, frame_time);
        user_interface->updateTextField(fps_handle, fps_string);

        user_interface->drawUserInterface(render_pass);

        vkCmdEndRenderPass(renderer->getCurrentCommandBuffer());
      });
      renderer->endPrimaryCommandBufferRecording();
    });
    Timing::timer.time("End Frame", Timing::OTHER, []() {
      renderer->endFrame();
    });
  });
  return true;
}

void ResizeCallback(ANativeActivity *activity, ANativeWindow *window){
  window_resized = true;
}

void OnOrientationChange() {
  vkDeviceWaitIdle(device->getDevice());

  for (int i = 0; i < device->getSwapchainLength(); ++i) {
    vkDestroyImageView(device->getDevice(), display_views[i], nullptr);
    vkDestroyFramebuffer(device->getDevice(), framebuffers[i], nullptr);
  }
  vkDestroyImageView(device->getDevice(), depth_buffer.image_view, nullptr);
  vkDestroyImage(device->getDevice(), depth_buffer.image, nullptr);
  vkFreeMemory(device->getDevice(), depth_buffer.device_memory, nullptr);

  device->createSwapChain(device->getSwapchain());
  CreateDepthBuffer();
  CreateFrameBuffers(render_pass, depth_buffer.image_view);
  Button::setScreenResolution(device->getDisplaySizeOriented());
  window_resized = false;
}
