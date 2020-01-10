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
#define GLM_FORCE_CXX17
#define GLM_ENABLE_EXPERIMENTAL

#include "vulkan_main.h"

#include "bender_kit.h"
#include "vulkan_wrapper.h"
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
#include "user_interface.h"
#include "render_graph.h"
#include "shader_bindings.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <android_native_app_glue.h>
#include <cassert>
#include <vector>
#include <array>
#include <cstring>
#include <chrono>
#include <set>
#include <obj_loader.h>

using namespace benderkit;
using namespace benderhelpers;

struct AttachmentBuffer {
  VkFormat format;
  VkImage image;
  VkDeviceMemory device_memory;
  VkImageView image_view;
};

android_app *android_app_ctx = nullptr;
Device *device;
Renderer *renderer;
RenderGraph *render_graph;

std::vector<VkImageView> display_views;
std::vector<VkFramebuffer> framebuffers;
VkRenderPass render_pass;
AttachmentBuffer depth_buffer;

Font *font;
std::shared_ptr<ShaderState> shaders;
std::vector<const char *> tex_files;
std::vector<std::shared_ptr<Texture>> textures;
std::vector<std::shared_ptr<Material>> materials;
std::vector<std::shared_ptr<Material>> baseline_materials;
std::vector<std::shared_ptr<Geometry>> geometries;

auto last_time = std::chrono::high_resolution_clock::now();
auto current_time = last_time;
float frame_time;
float total_time;

const glm::mat4 kIdentityMat4 = glm::mat4(1.0f);
const glm::mat4 prerotate_90 = glm::rotate(kIdentityMat4,  glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
const glm::mat4 prerotate_270 = glm::rotate(kIdentityMat4, glm::three_over_two_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
const glm::mat4 prerotate_180 = glm::rotate(kIdentityMat4, glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
bool app_initialized_once = false;
uint32_t poly_faces_idx = 0;
uint32_t materials_idx = 0;
UserInterface *user_interface;
char mesh_info[50];
char fps_info[50];

void MoveForward() {
  glm::vec3
      forward = glm::normalize(render_graph->GetCameraRotation() * glm::vec3(0.0f, 0.0f, -1.0f));
  render_graph->TranslateCamera(forward * 2.0f * frame_time);
}
void MoveBackward() {
  glm::vec3
      forward = glm::normalize(render_graph->GetCameraRotation() * glm::vec3(0.0f, 0.0f, -1.0f));
  render_graph->TranslateCamera(-forward * 2.0f * frame_time);
}
void StrafeLeft() {
  glm::vec3 right = glm::normalize(render_graph->GetCameraRotation() * glm::vec3(1.0f, 0.0f, 0.0f));
  render_graph->TranslateCamera(-right * (20.0f / device->GetDisplaySize().width));
}
void StrafeRight() {
  glm::vec3 right = glm::normalize(render_graph->GetCameraRotation() * glm::vec3(1.0f, 0.0f, 0.0f));
  render_graph->TranslateCamera(right * (20.0f / device->GetDisplaySize().width));
}
void StrafeUp() {
  glm::vec3 up = glm::normalize(render_graph->GetCameraRotation() * glm::vec3(0.0f, 1.0f, 0.0f));
  render_graph->TranslateCamera(up * (20.0f / device->GetDisplaySize().height));
}
void StrafeDown() {
  glm::vec3 up = glm::normalize(render_graph->GetCameraRotation() * glm::vec3(0.0f, 1.0f, 0.0f));
  render_graph->TranslateCamera(-up * (20.0f / device->GetDisplaySize().height));
}
void CreateInstance() {
  render_graph->AddMesh(std::make_shared<Mesh>(renderer,
                                               baseline_materials[materials_idx],
                                               geometries[poly_faces_idx]));
  render_graph->GetLastMesh()->Translate(glm::vec3(rand() % 3,
                                                   rand() % 3,
                                                   rand() % 3));
}
void DeleteInstance() {
  render_graph->RemoveLastMesh();
}
void ChangePolyhedralComplexity() {
  poly_faces_idx = (poly_faces_idx + 1) % allowedPolyFaces.size();
  auto all_meshes = render_graph->GetAllMeshes();
  for (uint32_t i = 0; i < all_meshes.size(); i++) {
    all_meshes[i] =
        std::make_shared<Mesh>(*all_meshes[i], geometries[poly_faces_idx]);
  }
  render_graph->ClearMeshes();
  render_graph->AddMeshes(all_meshes);
}
void ChangeMaterialComplexity() {
  materials_idx = (materials_idx + 1) % baseline_materials.size();
  auto all_meshes = render_graph->GetAllMeshes();
  for (uint32_t i = 0; i < all_meshes.size(); i++) {
    all_meshes[i] =
        std::make_shared<Mesh>(*all_meshes[i], baseline_materials[materials_idx]);
  }
  render_graph->ClearMeshes();
  render_graph->AddMeshes(all_meshes);
}

void generate(std::vector<float> vertexData, std::vector<uint16_t> indexData, std::vector<float> &out){
  out.resize((vertexData.size() / 8) * 14);
  std::set<uint16_t> completed;

  for (int x = 0; x < indexData.size(); x += 3){
    uint16_t idx1, idx2, idx3;
    idx1 = indexData[x];
    idx2 = indexData[x+1];
    idx3 = indexData[x+2];

    glm::vec3 posVec1 = {vertexData[idx1 * 8], vertexData[idx1 * 8 + 1], vertexData[idx1 * 8 + 2]};
    glm::vec3 posVec2 = {vertexData[idx2 * 8], vertexData[idx2 * 8 + 1], vertexData[idx2 * 8 + 2]};
    glm::vec3 posVec3 = {vertexData[idx3 * 8], vertexData[idx3 * 8 + 1], vertexData[idx3 * 8 + 2]};

    glm::vec2 texVec1 = {vertexData[idx1 * 8 + 6], vertexData[idx1 * 8 + 7]};
    glm::vec2 texVec2 = {vertexData[idx2 * 8 + 6], vertexData[idx2 * 8 + 7]};
    glm::vec2 texVec3 = {vertexData[idx3 * 8 + 6], vertexData[idx3 * 8 + 7]};

    glm::vec3 edge1 = posVec2 - posVec1;
    glm::vec3 edge2 = posVec3 - posVec1;
    glm::vec2 deltaUV1 = texVec2 - texVec1;
    glm::vec2 deltaUV2 = texVec3 - texVec1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    glm::vec3 tangent, bitangent;
    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent = glm::normalize(tangent);

    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent = glm::normalize(bitangent);

    if (completed.find(idx1) == completed.end()){
      out[idx1 * 14] = posVec1.x;
      out[idx1 * 14 + 1] = posVec1.y;
      out[idx1 * 14 + 2] = posVec1.z;
      out[idx1 * 14 + 3] = vertexData[idx1 * 8 + 3];
      out[idx1 * 14 + 4] = vertexData[idx1 * 8 + 4];
      out[idx1 * 14 + 5] = vertexData[idx1 * 8 + 5];
      out[idx1 * 14 + 6] = tangent.x;
      out[idx1 * 14 + 7] = tangent.y;
      out[idx1 * 14 + 8] = tangent.z;
      out[idx1 * 14 + 9] = bitangent.x;
      out[idx1 * 14 + 10] = bitangent.y;
      out[idx1 * 14 + 11] = bitangent.z;
      out[idx1 * 14 + 12] = texVec1.x;
      out[idx1 * 14 + 13] = texVec1.y;
      completed.insert(idx1);
    }

    if (completed.find(idx2) == completed.end()){
      out[idx2 * 14] = posVec2.x;
      out[idx2 * 14 + 1] = posVec2.y;
      out[idx2 * 14 + 2] = posVec2.z;
      out[idx2 * 14 + 3] = vertexData[idx2 * 8 + 3];
      out[idx2 * 14 + 4] = vertexData[idx2 * 8 + 4];
      out[idx2 * 14 + 5] = vertexData[idx2 * 8 + 5];
      out[idx2 * 14 + 6] = tangent.x;
      out[idx2 * 14 + 7] = tangent.y;
      out[idx2 * 14 + 8] = tangent.z;
      out[idx2 * 14 + 9] = bitangent.x;
      out[idx2 * 14 + 10] = bitangent.y;
      out[idx2 * 14 + 11] = bitangent.z;
      out[idx2 * 14 + 12] = texVec2.x;
      out[idx2 * 14 + 13] = texVec2.y;
      completed.insert(idx2);
    }

    if (completed.find(idx3) == completed.end()){
      out[idx3 * 14] = posVec3.x;
      out[idx3 * 14 + 1] = posVec3.y;
      out[idx3 * 14 + 2] = posVec3.z;
      out[idx3 * 14 + 3] = vertexData[idx3 * 8 + 3];
      out[idx3 * 14 + 4] = vertexData[idx3 * 8 + 4];
      out[idx3 * 14 + 5] = vertexData[idx3 * 8 + 5];
      out[idx3 * 14 + 6] = tangent.x;
      out[idx3 * 14 + 7] = tangent.y;
      out[idx3 * 14 + 8] = tangent.z;
      out[idx3 * 14 + 9] = bitangent.x;
      out[idx3 * 14 + 10] = bitangent.y;
      out[idx3 * 14 + 11] = bitangent.z;
      out[idx3 * 14 + 12] = texVec3.x;
      out[idx3 * 14 + 13] = texVec3.y;
      completed.insert(idx3);
    }
  }
}

std::map<std::string, std::shared_ptr<Texture>> loadedTextures;
std::map<std::string, std::shared_ptr<Material>> loadedMaterials;

void addTexture(std::string fileName){
  if (loadedTextures.find(fileName) == loadedTextures.end()){
    loadedTextures[fileName] = std::make_shared<Texture>(*device,
                                                         *android_app_ctx,
                                                         fileName.c_str(),
                                                         VK_FORMAT_R8G8B8A8_SRGB);
  }
}



void CreateButtons() {
  Button::SetScreenResolution(device->GetDisplaySizeOriented());

  user_interface->RegisterButton([] (Button& button) {
      button.on_hold_ = StrafeLeft;
      button.SetLabel("<--");
      button.SetPosition(-.7, .2, .7, .2);
  });
  user_interface->RegisterButton([] (Button& button) {
    button.on_hold_ = StrafeRight;
    button.SetLabel("-->");
    button.SetPosition(-.2, .2, .7, .2);
  });
  user_interface->RegisterButton([] (Button& button) {
    button.on_hold_ = StrafeUp;
    button.SetLabel("^");
    button.SetPosition(-.47, .2, .6, .2);
  });
  user_interface->RegisterButton([] (Button& button) {
    button.on_hold_ = StrafeDown;
    button.SetLabel("0");
    button.SetPosition(-.47, .2, .85, .2);
  });
  user_interface->RegisterButton([] (Button& button) {
    button.on_hold_ = MoveForward;
    button.SetLabel("Forward");
    button.SetPosition(.43, .2, .65, .2);
  });
  user_interface->RegisterButton([] (Button& button) {
    button.on_hold_ = MoveBackward;
    button.SetLabel("Backward");
    button.SetPosition(.43, .2, .85, .2);
  });
  user_interface->RegisterButton([] (Button& button) {
    button.on_up_ = CreateInstance;
    button.SetLabel("+1 Mesh");
    button.SetPosition(-.2, .2, .4, .2);
  });
  user_interface->RegisterButton([] (Button& button) {
    button.on_up_ = DeleteInstance;
    button.SetLabel("-1 Mesh");
    button.SetPosition(-.7, .2, .4, .2);
  });
  user_interface->RegisterButton([] (Button& button) {
    button.on_up_ = ChangePolyhedralComplexity;
    button.SetLabel("Poly Switch");
    button.SetPosition(.5, .2, .2, .2);
  });
  user_interface->RegisterButton([] (Button& button) {
    button.on_up_ = ChangeMaterialComplexity;
    button.SetLabel("Tex Switch");
    button.SetPosition(.5, .2, .4, .2);
  });
}

void CreateUserInterface() {
  user_interface = new UserInterface(renderer, font);

  CreateButtons();
  user_interface->RegisterTextField([] (TextField& field) {
      field.text = mesh_info;
      field.text_size = 1.0f;
      field.x_corner = -.98f;
      field.y_corner = -.98f;
  });
  user_interface->RegisterTextField([] (TextField& field) {
      field.text = fps_info;
      field.text_size = 1.0f;
      field.x_corner = -0.98f;
      field.y_corner = -0.88f;
  });
}

void CreateTextures() {
  timing::timer.Time("Texture Creation", timing::OTHER, [] {
    assert(android_app_ctx != nullptr);
    assert(device != nullptr);

    for (uint32_t i = 0; i < tex_files.size(); ++i) {
      textures.push_back(std::make_shared<Texture>(*device,
                                                   *android_app_ctx,
                                                   tex_files[i],
                                                   VK_FORMAT_R8G8B8A8_SRGB));
    }
  });
}

void CreateMaterials() {
  timing::timer.Time("Materials Creation", timing::OTHER, [] {
    MaterialAttributes defaultMaterial;
    defaultMaterial.specular = {0.8f, 0.0f, 0.5f, 128.0f};
    defaultMaterial.diffuse = {0.8f, 0.0f, 0.5f};
    defaultMaterial.ambient = {0.8f, 0.0f, 0.5f};
    std::vector<std::shared_ptr<Texture>> materialTextures = {nullptr};
    baseline_materials.push_back(std::make_shared<Material>(renderer, shaders, materialTextures));
    baseline_materials.push_back(std::make_shared<Material>(renderer,
                                                           shaders,
                                                           materialTextures,
                                                           defaultMaterial));

    materialTextures = {textures[0]};
    baseline_materials.push_back(std::make_shared<Material>(renderer, shaders, materialTextures));
    baseline_materials.push_back(std::make_shared<Material>(renderer,
                                                           shaders,
                                                           materialTextures,
                                                           defaultMaterial));
    for (uint32_t i = 0; i < textures.size(); ++i) {
      materialTextures = {textures[i]};
      materials.push_back(std::make_shared<Material>(renderer, shaders, materialTextures));
    }
  });
}

void CreateGeometries() {
  for (uint32_t i = 0; i < allowedPolyFaces.size(); i++) {
    std::vector<float> vertex_data;
    std::vector<uint16_t> index_data;
    polyhedronGenerators[i](vertex_data, index_data);
    geometries.push_back(std::make_shared<Geometry>(*device,
                                                    vertex_data,
                                                    index_data,
                                                    polyhedronGenerators[i]));
  }
}

void CreateFramebuffers(VkRenderPass &render_pass,
                        VkImageView depth_view = VK_NULL_HANDLE) {
  display_views.resize(device->GetDisplayImages().size());
  for (uint32_t i = 0; i < device->GetDisplayImages().size(); i++) {
    VkImageViewCreateInfo view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .image = device->GetDisplayImages()[i],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = device->GetDisplayFormat(),
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
    CALL_VK(vkCreateImageView(device->GetDevice(), &view_create_info, nullptr,
                              &display_views[i]));
  }

  framebuffers.resize(device->GetSwapchainLength());
  for (uint32_t i = 0; i < device->GetSwapchainLength(); i++) {
    VkImageView attachments[2] = {
        display_views[i], depth_view,
    };
    VkFramebufferCreateInfo framebuffer_create_info{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .renderPass = render_pass,
        .layers = 1,
        .attachmentCount = 2,  // 2 if using depth
        .pAttachments = attachments,
        .width = static_cast<uint32_t>(device->GetDisplaySize().width),
        .height = static_cast<uint32_t>(device->GetDisplaySize().height),
    };

    CALL_VK(vkCreateFramebuffer(device->GetDevice(), &framebuffer_create_info, nullptr,
                                &framebuffers[i]));
  }
}

void UpdateCamera(input::Data *input_data) {
  if ((input_data->last_button != nullptr && input_data->last_input_count > 1)
      || input_data->last_button == nullptr) {
    render_graph->RotateCameraGlobal(
        glm::quat(glm::vec3(0.0f, input_data->delta_x / device->GetDisplaySize().width, 0.0f)));
    render_graph->RotateCameraLocal(
        glm::quat(glm::vec3(input_data->delta_y / device->GetDisplaySize().height, 0.0f, 0.0f)));
  }

  render_graph->SetCameraViewMatrix(glm::inverse(
      glm::translate(glm::mat4(1.0f), render_graph->GetCameraPosition())
          * glm::mat4(render_graph->GetCameraRotation())));

  glm::mat4 proj_matrix = glm::perspective(render_graph->GetCameraFOV(),
                                          render_graph->GetCameraAspectRatio(),
                                          0.1f,
                                          100.0f);
  proj_matrix[1][1] *= -1;
  render_graph->SetCameraProjMatrix(proj_matrix);
}

void UpdateInstances(input::Data *input_data) {
  auto all_meshes = render_graph->GetAllMeshes();
  for (int i = 0; i < all_meshes.size(); i++) {
    all_meshes[i]->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 90 * frame_time);
//    all_meshes[i]->Translate(.02f * glm::vec3(std::sin(2 * total_time),
//                                             std::sin(i * total_time),
//                                             std::cos(2 * total_time)));

    all_meshes[i]->Update(renderer->GetCurrentFrame(),
                         render_graph->GetCameraPosition(),
                         render_graph->GetCameraViewMatrix(),
                         render_graph->GetCameraProjMatrix(),
                         render_graph->GetCameraPrerotationMatrix());
  }
  renderer->UpdateLights(render_graph->GetCameraPosition());
}

void HandleInput(input::Data *input_data) {
  UpdateCamera(input_data);
  UpdateInstances(input_data);
}

void CreateShaderState() {
  VertexFormat vertex_format{{
                                 VertexElement::float3,
                                 VertexElement::float3,
                                 VertexElement::float3,
                                 VertexElement::float3,
                                 VertexElement::float2,
                             },
  };

  shaders =
      std::make_shared<ShaderState>("mesh", vertex_format, *android_app_ctx, device->GetDevice());
}

void CreateDepthBuffer() {
  depth_buffer.format = benderhelpers::FindDepthFormat(device);
  VkImageCreateInfo image_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .extent.width = device->GetDisplaySize().width,
      .extent.height = device->GetDisplaySize().height,
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

  CALL_VK(vkCreateImage(device->GetDevice(), &image_info, nullptr, &depth_buffer.image));

  VkMemoryRequirements mem_requirements;
  vkGetImageMemoryRequirements(device->GetDevice(), depth_buffer.image, &mem_requirements);

  VkMemoryAllocateInfo alloc_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = mem_requirements.size,
      .memoryTypeIndex = benderhelpers::FindMemoryType(mem_requirements.memoryTypeBits,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                       device->GetPhysicalDevice()),
  };

  CALL_VK(vkAllocateMemory(device->GetDevice(), &alloc_info, nullptr, &depth_buffer.device_memory))

  vkBindImageMemory(device->GetDevice(), depth_buffer.image, depth_buffer.device_memory, 0);

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

  CALL_VK(vkCreateImageView(device->GetDevice(), &view_info, nullptr, &depth_buffer.image_view));

}

void UpdateCameraParameters(){
  render_graph->SetCameraAspectRatio(
      device->GetDisplaySizeOriented().width / (float) device->GetDisplaySizeOriented().height);

  auto horizontal_fov = glm::radians(60.0f);
  auto vertical_fov = static_cast<float>(2
      * atan(tan(horizontal_fov / 2) * (1.0 / render_graph->GetCameraAspectRatio())));
  render_graph->SetCameraFOV((render_graph->GetCameraAspectRatio() > 1.0f) ? horizontal_fov
                                                                           : vertical_fov);
  switch (device->GetPretransformFlag()) {
    case VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR:
      render_graph->SetCameraPrerotationMatrix(prerotate_90);
      break;
    case VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR:
      render_graph->SetCameraPrerotationMatrix(prerotate_180);
      break;
    case VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR:
      render_graph->SetCameraPrerotationMatrix(prerotate_270);
      break;
    default:
      render_graph->SetCameraPrerotationMatrix(kIdentityMat4);
      break;
  }
}

bool InitVulkan(android_app *app) {
  timing::timer.Time("Initialization", timing::OTHER, [app] {
    android_app_ctx = app;

    device = new Device(app->window);
    assert(device->IsInitialized());
    device->SetObjectName(reinterpret_cast<uint64_t>(device->GetDevice()),
                          VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT, "TEST NAME: VULKAN DEVICE");

    render_graph = new RenderGraph();

    UpdateCameraParameters();

    VkAttachmentDescription color_description{
        .format = device->GetDisplayFormat(),
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentDescription depth_description{
        .format = benderhelpers::FindDepthFormat(device),
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
    CALL_VK(vkCreateRenderPass(device->GetDevice(), &render_pass_create_info, nullptr,
                               &render_pass));

    CreateShaderState();

    renderer = new Renderer(*device);

    timing::timer.Time("Mesh Creation", timing::OTHER, [app] {
      std::vector<OBJLoader::OBJ> modelData;
      std::unordered_map<std::string, OBJLoader::MTL> mtllib;
      OBJLoader::LoadOBJ(app->activity->assetManager,
                         "models/sphere.obj",
                         mtllib,
                         modelData);

      for (auto currMTL : mtllib) {
        addTexture(currMTL.second.map_Ka_);
        addTexture(currMTL.second.map_Kd_);
        addTexture(currMTL.second.map_Ks_);
        addTexture(currMTL.second.map_Ns_);
        addTexture(currMTL.second.map_Ke_);
        addTexture(currMTL.second.map_Bump_);
        MaterialAttributes newMTL;
        newMTL.ambient = currMTL.second.ambient_;
        newMTL.specular = glm::vec4(currMTL.second.specular_, currMTL.second.specular_exponent_);
        newMTL.diffuse = currMTL.second.diffuse_;
        newMTL.bump_multiplier = currMTL.second.bump_multiplier;
        std::vector<std::shared_ptr<Texture>> mTextures = {loadedTextures[currMTL.second.map_Kd_],
                                                           loadedTextures[currMTL.second.map_Ks_],
                                                           loadedTextures[currMTL.second.map_Ke_],
                                                           loadedTextures[currMTL.second.map_Bump_]};
        loadedMaterials[currMTL.first] = std::make_shared<Material>(renderer,
                                                                    shaders,
                                                                    mTextures,
                                                                    newMTL);
      }
      for (auto obj : modelData) {
        render_graph->AddMesh(std::make_shared<Mesh>(renderer,
                                                     loadedMaterials[obj.material_name_],
                                                     std::make_shared<Geometry>(*device,
                                                                                obj.vertex_buffer_,
                                                                                obj.index_buffer_)));
      }
    });

    CreateDepthBuffer();

    CreateFramebuffers(render_pass, depth_buffer.image_view);

    font = new Font(*renderer, *android_app_ctx, FONT_SDF_PATH, FONT_INFO_PATH);

    CreateUserInterface();
  });

  timing::PrintEvent(*timing::timer.GetLastMajorEvent());
  app_initialized_once = true;
  return true;
}

bool ResumeVulkan(android_app *app) {
  timing::timer.Time("Initialization", timing::OTHER, [app] {
    android_app_ctx = app;

    device = new Device(app->window);
    assert(device->IsInitialized());
    device->SetObjectName(reinterpret_cast<uint64_t>(device->GetDevice()),
                          VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT, "TEST NAME: VULKAN DEVICE");

    UpdateCameraParameters();

    VkAttachmentDescription color_description{
        .format = device->GetDisplayFormat(),
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentDescription depth_description{
        .format = benderhelpers::FindDepthFormat(device),
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
    CALL_VK(vkCreateRenderPass(device->GetDevice(), &render_pass_create_info, nullptr,
                               &render_pass));

    shaders->OnResume(device->GetDevice());

    renderer = new Renderer(*device);

    timing::timer.Time("Mesh Creation", timing::OTHER, [app] {
      for (auto &texture : textures) {
        texture->OnResume(*device, android_app_ctx);
      }
      Material::OnResumeStatic(*device, app);

      for (auto &material : materials) {
        material->OnResume(renderer);
      }

      for (auto &material : baseline_materials) {
        material->OnResume(renderer);
      }

      for (auto &geometry : geometries) {
        geometry->OnResume(*device);
      }

      for (auto &mesh : render_graph->GetAllMeshes()) {
        mesh->OnResume(renderer);
      }
    });

    CreateDepthBuffer();

    CreateFramebuffers(render_pass, depth_buffer.image_view);

    font = new Font(*renderer, *android_app_ctx, FONT_SDF_PATH, FONT_INFO_PATH);
    user_interface->OnResume(renderer, font);
  });

  timing::PrintEvent(*timing::timer.GetLastMajorEvent());
  return true;
}

void StartVulkan(android_app *app) {
  if (!app_initialized_once) {
    InitVulkan(app);
  } else {
    ResumeVulkan(app);
  }
}

bool IsVulkanReady(void) { return device != nullptr && device->IsInitialized(); }

void DeleteVulkan(void) {
  vkDeviceWaitIdle(device->GetDevice());

  delete renderer;
  delete font;

  for (int i = 0; i < device->GetSwapchainLength(); i++) {
    vkDestroyImageView(device->GetDevice(), display_views[i], nullptr);
    vkDestroyFramebuffer(device->GetDevice(), framebuffers[i], nullptr);
  }

  vkDestroyImageView(device->GetDevice(), depth_buffer.image_view, nullptr);
  vkDestroyImage(device->GetDevice(), depth_buffer.image, nullptr);
  vkFreeMemory(device->GetDevice(), depth_buffer.device_memory, nullptr);

  vkDestroyRenderPass(device->GetDevice(), render_pass, nullptr);

  for (auto &mesh : render_graph->GetAllMeshes()) {
    mesh->Cleanup();
  }
  for (auto &texture : textures) {
    texture->Cleanup();
  }
  for (auto &material : materials) {
    material->Cleanup();
  }
  for (auto &material : baseline_materials) {
    material->Cleanup();
  }
  for (auto &geometry : geometries) {
    geometry->Cleanup();
  }
  shaders->Cleanup();
  Material::CleanupStatic();

  delete device;
  device = nullptr;
}

bool VulkanDrawFrame(input::Data *input_data) {
  if (device->GetWindowResized()) {
    OnOrientationChange();
  }
  current_time = std::chrono::high_resolution_clock::now();
  frame_time = std::chrono::duration<float>(current_time - last_time).count();
  last_time = current_time;
  total_time += frame_time;

  timing::timer.Time("Handle Input", timing::OTHER, [input_data] {
    HandleInput(input_data);
  });

  renderer->BeginFrame();
  timing::timer.Time("Start Frame", timing::START_FRAME, [] {
    timing::timer.Time("PrimaryCommandBufferRecording", timing::OTHER, [] {
      renderer->BeginPrimaryCommandBufferRecording();

      // Now we start a renderpass. Any draw command has to be recorded in a
      // renderpass
      std::array<VkClearValue, 2> clear_values = {};
      clear_values[0].color = {{0.0f, 0.34f, 0.90f, 1.0}};
      clear_values[1].depthStencil = {1.0f, 0};

      VkRenderPassBeginInfo render_pass_begin_info{
          .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
          .pNext = nullptr,
          .renderPass = render_pass,
          .framebuffer = framebuffers[renderer->GetCurrentFrame()],
          .renderArea = {.offset =
              {
                  .x = 0, .y = 0,
              },
              .extent = device->GetDisplaySize()},
          .clearValueCount = static_cast<uint32_t>(clear_values.size()),
          .pClearValues = clear_values.data(),
      };

      timing::timer.Time("Render Pass", timing::OTHER, [render_pass_begin_info] {
        vkCmdBeginRenderPass(renderer->GetCurrentCommandBuffer(), &render_pass_begin_info,
                             VK_SUBPASS_CONTENTS_INLINE);

        device->InsertDebugMarker(renderer->GetCurrentCommandBuffer(),
                                  "TEST MARKER: PIPELINE BINDING",
                                  {1.0f, 0.0f, 1.0f, 0.0f});

        int total_triangles = 0;
        auto all_meshes = render_graph->GetAllMeshes();
        for (uint32_t i = 0; i < all_meshes.size(); i++) {
          all_meshes[i]->UpdatePipeline(render_pass);
          all_meshes[i]->SubmitDraw(renderer->GetCurrentCommandBuffer(),
                                   renderer->GetCurrentFrame());
          total_triangles += all_meshes[i]->GetTrianglesCount();
        }

        const char *kMeshNoun = all_meshes.size() == 1 ? "mesh" : "meshes";
        const char *kPolyNoun = "faces/polyhedron";
        const char *kTriangleNoun = total_triangles == 1 ? "triangle" : "triangles";

        sprintf(mesh_info, "%d %s, %d %s, %d %s", (int) all_meshes.size(), kMeshNoun,
                allowedPolyFaces[poly_faces_idx], kPolyNoun, total_triangles, kTriangleNoun);

        int fps;
        float frame_time;
        timing::timer.GetFramerate(500,
                                   timing::timer.GetLastMajorEvent()->number,
                                   &fps,
                                   &frame_time);
        sprintf(fps_info, "%2.d FPS  %.3f ms", fps, frame_time);

        user_interface->DrawUserInterface(render_pass);

        vkCmdEndRenderPass(renderer->GetCurrentCommandBuffer());
      });
      renderer->EndPrimaryCommandBufferRecording();
    });
    timing::timer.Time("End Frame", timing::OTHER, [] {
      renderer->EndFrame();
    });
  });
  return true;
}

void OnOrientationChange() {
  vkDeviceWaitIdle(device->GetDevice());

  for (int i = 0; i < device->GetSwapchainLength(); i++) {
    vkDestroyImageView(device->GetDevice(), display_views[i], nullptr);
    vkDestroyFramebuffer(device->GetDevice(), framebuffers[i], nullptr);
  }
  vkDestroyImageView(device->GetDevice(), depth_buffer.image_view, nullptr);
  vkDestroyImage(device->GetDevice(), depth_buffer.image, nullptr);
  vkFreeMemory(device->GetDevice(), depth_buffer.device_memory, nullptr);

  device->CreateSwapChain(device->GetSwapchain());
  CreateDepthBuffer();
  CreateFramebuffers(render_pass, depth_buffer.image_view);
  Button::SetScreenResolution(device->GetDisplaySizeOriented());
  UpdateCameraParameters();
  device->SetWindowResized(false);
}
