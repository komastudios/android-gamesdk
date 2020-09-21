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
#include <thread>
#include <mutex>
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
std::vector<std::string> tex_files;
std::vector<std::shared_ptr<Texture>> textures;
std::vector<std::shared_ptr<Material>> materials;
std::vector<std::shared_ptr<Material>> baseline_materials;
std::vector<std::shared_ptr<Geometry>> geometries;

bool use_astc = true; // This is set true for testing ASTC. TODO: replace this with a toggle.
std::map<std::string, std::shared_ptr<Texture>> loaded_textures;
std::map<std::string, std::shared_ptr<Material>> loaded_materials;

auto last_time = std::chrono::high_resolution_clock::now();
auto current_time = last_time;
float frame_time;
float total_time;

const float kCameraMoveSpeed = 200.0;
const float kCameraStrafeSpeed = 2000.0;
const float kInputThresholdX = .1;
const float kInputThresholdY = .1;

const glm::mat4 kIdentityMat4 = glm::mat4(1.0f);
const glm::mat4 prerotate_90 = glm::rotate(kIdentityMat4,  glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
const glm::mat4 prerotate_270 = glm::rotate(kIdentityMat4, glm::three_over_two_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
const glm::mat4 prerotate_180 = glm::rotate(kIdentityMat4, glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));

bool app_initialized_once = false;
bool is_presenting = false;
std::thread *load_thread;
bool currently_loading = false;
std::mutex load_mutex;
std::mutex render_graph_mutex;
std::mutex loading_info_mutex;

uint32_t poly_faces_idx = 0;
uint32_t materials_idx = 0;
UserInterface *user_interface;
char loading_info[100] = " ";
char mesh_info[50] = " ";
char fps_info[50] = " ";

void MoveForward() {
  glm::vec3
      forward = glm::normalize(render_graph->GetCamera().rotation * glm::vec3(0.0f, 0.0f, -1.0f));
  render_graph->TranslateCamera(forward * kCameraMoveSpeed * frame_time);
}
void MoveBackward() {
  glm::vec3
      forward = glm::normalize(render_graph->GetCamera().rotation * glm::vec3(0.0f, 0.0f, -1.0f));
  render_graph->TranslateCamera(-forward * kCameraMoveSpeed * frame_time);
}
void StrafeLeft() {
  glm::vec3 right = glm::normalize(render_graph->GetCamera().rotation * glm::vec3(1.0f, 0.f, 0.f));
  render_graph->TranslateCamera(-right * (kCameraStrafeSpeed / device->GetDisplaySize().width));
}
void StrafeRight() {
  glm::vec3 right = glm::normalize(render_graph->GetCamera().rotation * glm::vec3(1.0f, 0.f, 0.f));
  render_graph->TranslateCamera(right * (kCameraStrafeSpeed / device->GetDisplaySize().width));
}
void StrafeUp() {
  glm::vec3 up = glm::normalize(render_graph->GetCamera().rotation * glm::vec3(0.0f, 1.0f, 0.0f));
  render_graph->TranslateCamera(up * (kCameraStrafeSpeed / device->GetDisplaySize().height));
}
void StrafeDown() {
  glm::vec3 up = glm::normalize(render_graph->GetCamera().rotation * glm::vec3(0.0f, 1.0f, 0.0f));
  render_graph->TranslateCamera(-up * (kCameraStrafeSpeed / device->GetDisplaySize().height));
}
void CreateInstance() {
  render_graph->AddMesh(std::make_shared<Mesh>(*renderer,
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
  std::vector<std::shared_ptr<Mesh>> all_meshes;
  render_graph->GetAllMeshes(all_meshes);
  for (uint32_t i = 0; i < all_meshes.size(); i++) {
    all_meshes[i] =
        std::make_shared<Mesh>(*all_meshes[i], geometries[poly_faces_idx]);
  }
  render_graph->ClearMeshes();
  render_graph->AddMeshes(all_meshes);
}
void ChangeMaterialComplexity() {
  materials_idx = (materials_idx + 1) % baseline_materials.size();
  std::vector<std::shared_ptr<Mesh>> all_meshes;
  render_graph->GetAllMeshes(all_meshes);
  for (uint32_t i = 0; i < all_meshes.size(); i++) {
    all_meshes[i] =
        std::make_shared<Mesh>(*all_meshes[i], baseline_materials[materials_idx]);
  }
  render_graph->ClearMeshes();
  render_graph->AddMeshes(all_meshes);
}
void ToggleMipmaps() {
  vkDeviceWaitIdle(device->GetDevice());
  renderer->ToggleMipmaps();
  for (auto &texture : loaded_textures) {
    if (texture.second != nullptr) texture.second->ToggleMipmaps();
  }
  for (auto &material : loaded_materials) {
    material.second->UpdateMaterialDescriptorSets();
  }
}

void CreateButtons() {
  Button::SetScreenResolution(device->GetDisplaySizeOriented());

  user_interface->RegisterButton([] (Button& button) {
      button.on_up_ = ToggleMipmaps;
      button.SetLabel("Mipmap Switch");
      button.SetPosition(.5, .2, 0, .2);
  });
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
#ifndef GDC_DEMO
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
#endif
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
  user_interface->RegisterTextField([] (TextField& field) {
      field.text = loading_info;
      field.text_size = 0.6f;
      field.x_corner = -0.98f;
      field.y_corner = -0.8f;
  });
}

void CreateTextures() {
  timing::timer.Time("Texture Creation", timing::OTHER, [] {
    assert(android_app_ctx != nullptr);
    assert(device != nullptr);

    for (uint32_t i = 0; i < tex_files.size(); ++i) {
      textures.push_back(std::make_shared<Texture>(*renderer,
                                                   *android_app_ctx,
                                                   tex_files[i],
                                                   VK_FORMAT_R8G8B8A8_SRGB));
    }
  });
}

void AddTexture(std::string file_name, VkFormat format) {
  if (file_name != "" && loaded_textures.find(file_name) == loaded_textures.end()){
    std::string texture_name = file_name;
    if (use_astc && file_name.find(".png") != -1) {
      size_t basename_len = file_name.rfind('.');
      file_name.replace(basename_len, file_name.size() - basename_len + 1, ".astc");
    }
    loaded_textures[texture_name] = std::make_shared<Texture>(*renderer,
                                                              *android_app_ctx,
                                                              "textures/" + file_name,
                                                              format);
  }
}

void CreateMaterials() {
  timing::timer.Time("Materials Creation", timing::OTHER, [] {
    MaterialAttributes defaultMaterial;
    defaultMaterial.specular = {0.8f, 0.0f, 0.5f, 128.0f};
    defaultMaterial.diffuse = {0.8f, 0.0f, 0.5f};
    defaultMaterial.ambient = {0.8f, 0.0f, 0.5f};
    std::vector<std::shared_ptr<Texture>> materialTextures = {nullptr, nullptr, nullptr, nullptr, nullptr};
    baseline_materials.push_back(std::make_shared<Material>(*renderer, shaders, materialTextures));
    baseline_materials.push_back(std::make_shared<Material>(*renderer,
                                                           shaders,
                                                           materialTextures,
                                                           defaultMaterial));

    materialTextures = {textures[0], nullptr, nullptr, nullptr, nullptr};
    baseline_materials.push_back(std::make_shared<Material>(*renderer, shaders, materialTextures));
    baseline_materials.push_back(std::make_shared<Material>(*renderer,
                                                           shaders,
                                                           materialTextures,
                                                           defaultMaterial));
    for (uint32_t i = 0; i < textures.size(); ++i) {
      materialTextures = {textures[i], nullptr, nullptr, nullptr, nullptr};
      materials.push_back(std::make_shared<Material>(*renderer, shaders, materialTextures));
    }
  });
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

bool InputWithinApp(float last_x, float last_y) {
  uint32_t width = device->GetDisplaySizeOriented().width;
  uint32_t height = device->GetDisplaySizeOriented().height;

  return last_x > width * kInputThresholdX
      && last_x < width * (1 - kInputThresholdX)
      && last_y > height * kInputThresholdY
      && last_y < height * (1 - kInputThresholdY);
}

void UpdateCamera(input::Data *input_data) {
  if (((input_data->last_button != nullptr && input_data->last_input_count > 1)
      || input_data->last_button == nullptr) && InputWithinApp(input_data->last_x, input_data->last_y)) {
    render_graph->RotateCameraGlobal(
        glm::quat(glm::vec3(0.0f, input_data->delta_x / device->GetDisplaySize().width, 0.0f)));
    render_graph->RotateCameraLocal(
        glm::quat(glm::vec3(input_data->delta_y / device->GetDisplaySize().height, 0.0f, 0.0f)));
  }

  Camera camera = render_graph->GetCamera();
  render_graph->SetCameraViewMatrix(glm::inverse(
      glm::translate(glm::mat4(1.0f), camera.position) * glm::mat4(camera.rotation)));

  glm::mat4 proj_matrix = glm::perspective(camera.fov,
                                           camera.aspect_ratio,
                                           camera.near,
                                           camera.far);
  proj_matrix[1][1] *= -1;
  render_graph->SetCameraProjMatrix(proj_matrix);
}

void UpdateInstances() {
  std::vector<std::shared_ptr<Mesh>> all_meshes;
  Camera camera = render_graph->GetCamera();
  render_graph_mutex.lock();
  render_graph->GetAllMeshes(all_meshes);
  render_graph_mutex.unlock();
  for (int i = 0; i < all_meshes.size(); i++) {
#ifndef GDC_DEMO
    all_meshes[i]->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 90 * frame_time);
    all_meshes[i]->Translate(.02f * glm::vec3(std::sin(2 * total_time),
                                              std::sin(i * total_time),
                                              std::cos(2 * total_time)));
#endif

    all_meshes[i]->Update(renderer->GetCurrentImage(), camera);
  }
  renderer->UpdateLights(camera.position);
}

void HandleInput(input::Data *input_data) {
  UpdateCamera(input_data);
  UpdateInstances();
}

void CreateShaderState() {
  // This format matches the format found in ../../utils/packed_types.h
  VertexFormat vertex_format{{
                                 VertexElement::snorm4, // position
                                 VertexElement::snorm4, // qtangent (for tangent space)
                                 VertexElement::unorm2, // texture coordinates
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
      * atan(tan(horizontal_fov / 2) * (1.0 / render_graph->GetCamera().aspect_ratio)));
  render_graph->SetCameraFOV((render_graph->GetCamera().aspect_ratio > 1.0f) ? horizontal_fov
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

void ReadStringFromFile(std::istream &data, std::string &result) {
    uint32_t size;
    data.read((char *)&size, sizeof(size));
    result.resize(size);
    data.read(&result[0], size);
}

void LoadMaterials(std::stringstream &data, int material_count) {
    for (int x = 0; x < material_count; x++) {
        std::string name;
        MaterialAttributes new_mtl;
        std::string map_Ka;
        std::string map_Kd;
        std::string map_Ke;
        std::string map_Ks;
        std::string map_Ns;
        std::string map_Bump;

        data.read((char *)&new_mtl.ambient, sizeof(float) * 3);
        data.read((char *)&new_mtl.diffuse, sizeof(float) * 3);
        data.read((char *)&new_mtl.specular, sizeof(float) * 3);
        data.read((char *)&new_mtl.specular.w, sizeof(float));
        data.read((char *)&new_mtl.bump_multiplier, sizeof(int));
        ReadStringFromFile(data, name);
        ReadStringFromFile(data, map_Ka);
        ReadStringFromFile(data, map_Kd);
        ReadStringFromFile(data, map_Ke);
        ReadStringFromFile(data, map_Ks);
        ReadStringFromFile(data, map_Ns);
        ReadStringFromFile(data, map_Bump);

        AddTexture(map_Ka, VK_FORMAT_R8G8B8A8_SRGB);
        AddTexture(map_Kd, VK_FORMAT_R8G8B8A8_SRGB);
        AddTexture(map_Ke, VK_FORMAT_R8G8B8A8_SRGB);
        AddTexture(map_Ks, VK_FORMAT_R8G8B8A8_SRGB);
        AddTexture(map_Ns, VK_FORMAT_R8G8B8A8_UNORM);
        AddTexture(map_Bump, VK_FORMAT_R8G8B8A8_UNORM);

        std::vector<std::shared_ptr<Texture>> my_textures = {
                loaded_textures[map_Kd],
                loaded_textures[map_Ks],
                loaded_textures[map_Ke],
                loaded_textures[map_Ns],
                loaded_textures[map_Bump]
        };
        loaded_materials[name] = std::make_shared<Material>(*renderer, shaders, my_textures, new_mtl);
    }
}

void LoadMeshes(std::stringstream &data, int mesh_count) {
    for (int x = 0; x < mesh_count; x++){
        std::string name, material_name;
        BoundingBox bounding_box;
        glm::vec3 scale_factor;
        uint32_t id, vertex_count, index_count, vertex_offset, index_offset;

        data.read((char *)&id, sizeof(uint32_t));
        data.read((char *)&bounding_box.min, sizeof(float) * 3);
        data.read((char *)&bounding_box.max, sizeof(float) * 3);
        data.read((char *)&bounding_box.center, sizeof(float) * 3);
        data.read((char *)&scale_factor, sizeof(float) * 3);
        data.read((char *)&vertex_count, sizeof(uint32_t));
        data.read((char *)&index_count, sizeof(uint32_t));
        data.read((char *)&vertex_offset, sizeof(uint32_t));
        data.read((char *)&index_offset, sizeof(uint32_t));
        ReadStringFromFile(data, name);
        ReadStringFromFile(data, material_name);
        geometries.push_back(std::make_shared<Geometry>(vertex_count, index_count, vertex_offset, index_offset, bounding_box, scale_factor));
        render_graph->AddMesh(std::make_shared<Mesh>(*renderer, loaded_materials[material_name], geometries.back()));
    }
}


void LoadDemoModels(AAsset *file) {
    render_graph_mutex.lock();
    const char *fileContent = static_cast<const char *>(AAsset_getBuffer(file));
    std::stringstream data(std::string(fileContent, AAsset_getLength(file)));

    uint32_t material_count, mesh_count, mesh_offset, total_vertex_count, vertex_data_offset, total_index_count, index_data_offset;
    data.read((char *)&material_count, sizeof(uint32_t));
    data.read((char *)&mesh_count, sizeof(uint32_t));
    data.read((char *)&mesh_offset, sizeof(uint32_t));
    data.read((char *)&total_vertex_count, sizeof(uint32_t));
    data.read((char *)&vertex_data_offset, sizeof(uint32_t));
    data.read((char *)&total_index_count, sizeof(uint32_t));
    data.read((char *)&index_data_offset, sizeof(uint32_t));

    LoadMaterials(data, material_count);

    LoadMeshes(data, mesh_count);

    Geometry::FillVertexBuffer<packed_vertex>(*device, total_vertex_count * sizeof(packed_vertex),
                                              [&data](packed_vertex *vertex_memory, size_t length) {
                                                  data.read((char *) vertex_memory, length);
                                              });

    Geometry::FillIndexBuffer(*device, total_index_count * sizeof(uint16_t),
                              [&data](uint16_t *index_memory, size_t length) {
                                  data.read((char *) index_memory, length);
                              });
    render_graph_mutex.unlock();
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

    CreateDepthBuffer();

    CreateFramebuffers(render_pass, depth_buffer.image_view);

    font = new Font(*renderer, *android_app_ctx, kFontSDFPath, kFontInfoPath);

    CreateUserInterface();

#ifndef GDC_DEMO
    timing::timer.Time("Mesh Creation", timing::OTHER, [] {
        tex_files.push_back("textures/sample_texture.png");

        CreateTextures();

        CreateMaterials();

        AAsset *file = AAssetManager_open(android_app_ctx->activity->assetManager, "polygon2.soup", AASSET_MODE_BUFFER);
        LoadDemoModels(file);
    });
#else
    currently_loading = true;
    load_thread = new std::thread([] {
        AAsset *file = AAssetManager_open(android_app_ctx->activity->assetManager, "polygon.soup", AASSET_MODE_BUFFER);
        LoadDemoModels(file);
        currently_loading = false;
    });

#endif
    app_initialized_once = true;
    is_presenting = true;
  });
  return true;
}

bool ResumeVulkan(android_app *app) {
  timing::timer.ResetEvents();
  timing::timer.Time("Resume Vulkan", timing::OTHER, [app] {
    vkDeviceWaitIdle(device->GetDevice());
    for (int i = 0; i < device->GetSwapchainLength(); i++) {
      vkDestroyImageView(device->GetDevice(), display_views[i], nullptr);
      vkDestroyFramebuffer(device->GetDevice(), framebuffers[i], nullptr);
    }
    android_app_ctx = app;
    device->CreateSurface(app->window);

    UpdateCameraParameters();

    CreateFramebuffers(render_pass, depth_buffer.image_view);
    is_presenting = true;
  });
  return true;
}

void StartVulkan(android_app *app) {
  if (!app_initialized_once) {
    InitVulkan(app);
  } else {
    ResumeVulkan(app);
    load_mutex.unlock();
  }
}

bool IsVulkanReady(void) { return device != nullptr && device->IsInitialized() && is_presenting; }

void DestroyWindow(void) {
  is_presenting = false;
  load_mutex.lock();
  device->DestroySurface();
}

void DeleteVulkan(void) {
  load_mutex.try_lock();
  if (currently_loading) {
    load_thread->detach();
    delete load_thread;
  }
  load_thread = nullptr;
  load_mutex.unlock();

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

  render_graph->ClearMeshes();
  loaded_textures.clear();
  textures.clear();
  loaded_materials.clear();
  materials.clear();
  baseline_materials.clear();
  geometries.clear();
  Geometry::CleanupStatic(*device);
  shaders.reset();

  delete device;

  app_initialized_once = false;
}

bool VulkanDrawFrame(input::Data *input_data) {
  if (device->GetWindowResized()) {
    OnOrientationChange();
  }
  if (!currently_loading) {
    load_thread = nullptr;
  }
  current_time = std::chrono::high_resolution_clock::now();
  frame_time = std::chrono::duration<float>(current_time - last_time).count();
  last_time = current_time;
  total_time += frame_time;

  timing::timer.Time("Start Frame", timing::START_FRAME, [&input_data] {
    renderer->BeginFrame();
    timing::timer.Time("Handle Input", timing::OTHER, [&input_data] {
        HandleInput(input_data);
        user_interface->RunHeldButtons();
    });

    timing::timer.Time("PrimaryCommandBufferRecording", timing::OTHER, [] {
      renderer->BeginPrimaryCommandBufferRecording();

      // Now we start a renderpass. Any draw command has to be recorded in a
      // renderpass
      std::array<VkClearValue, 2> clear_values = {};
      clear_values[0].color = {{0.0f, 0.0f, 0.0f, 1.0}};
      clear_values[1].depthStencil = {1.0f, 0};

      VkRenderPassBeginInfo render_pass_begin_info{
          .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
          .pNext = nullptr,
          .renderPass = render_pass,
          .framebuffer = framebuffers[renderer->GetCurrentImage()],
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
        std::vector<std::shared_ptr<Mesh>> all_meshes;
        render_graph_mutex.lock();
        render_graph->GetVisibleMeshes(all_meshes);
        render_graph_mutex.unlock();
        for (uint32_t i = 0; i < all_meshes.size(); i++) {
          all_meshes[i]->UpdatePipeline(render_pass);
          all_meshes[i]->SubmitDraw(renderer->GetCurrentCommandBuffer(),
                                    renderer->GetCurrentImage());
          total_triangles += all_meshes[i]->GetTrianglesCount();
        }

        const char *kMeshNoun = all_meshes.size() == 1 ? "mesh" : "meshes";
        const char *kPolyNoun = "faces/polyhedron";
        const char *kTriangleNoun = total_triangles == 1 ? "triangle" : "triangles";

        sprintf(mesh_info, "%d %s, %d %s, %d %s", (int) all_meshes.size(), kMeshNoun,
                allowedPolyFaces[poly_faces_idx], kPolyNoun, total_triangles, kTriangleNoun);

        int fps;
        float avg_frame_time;
        timing::timer.GetFramerate(100, &fps, &avg_frame_time);
        sprintf(fps_info,
                "%2.d FPS  %.3f ms mipmaps: %s",
                fps,
                avg_frame_time,
                renderer->MipmapsEnabled() ? "enabled" : "disabled");

        loading_info_mutex.lock();
        user_interface->DrawUserInterface(render_pass);
        loading_info_mutex.unlock();

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

  device->CreateSwapChain(device->GetSwapchain());
  CreateFramebuffers(render_pass, depth_buffer.image_view);
  Button::SetScreenResolution(device->GetDisplaySizeOriented());
  UpdateCameraParameters();
  device->SetWindowResized(false);
}
