//
// Created by theoking on 12/9/2019.
//

#ifndef BENDER_BASE_APP_SRC_MAIN_JNI_RENDER_GRAPH_H_
#define BENDER_BASE_APP_SRC_MAIN_JNI_RENDER_GRAPH_H_

#include "shader_state.h"
#include "texture.h"
#include "material.h"
#include "geometry.h"
#include "mesh_renderer.h"
#include "device.h"
#include "renderer.h"
#include "input.h"

class RenderGraph {
  struct Camera {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    float aspect_ratio;
    float fov;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 prerotation;
  };

  struct MeshInstance {
    const std::shared_ptr<Material> material;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
  };

  struct MeshUnit {
    std::shared_ptr<Geometry> geometry;
    MeshRenderer mesh_renderer;
    std::vector<MeshInstance> instances;
  };

 public:
  std::vector<std::shared_ptr<MeshRenderer>> GetAllMeshes() { return meshes_; }
  std::shared_ptr<MeshRenderer> GetLastMesh() {
    if (meshes_.size() > 0) {
      return meshes_.back();
    } else {
      return nullptr;
    }
  }

  // TODO: consolidate camera, view, proj into a camera object
  void Update(uint_t frame_index, glm::vec3 camera, glm::mat4 view, glm::mat4 proj, glm::mat4 prerotation) {
    for (uint32_t i = 0; i < size; i++) {
      glm::mat4 model = GetTransform(instances[i]);
      glm::mat4 mvp = proj * view * model;

      mesh_buffer_->Update(frame_index, [&mvp, &model, &prerotation](auto &ubo) {
          ubo.mvp = prerotation * mvp;
          ubo.model = model;
          ubo.inv_transpose = glm::transpose(glm::inverse(model));
      });
    }
  }

  void AddMesh(std::shared_ptr<MeshRenderer> new_mesh) { meshes_.push_back(new_mesh); }
  void AddMeshes(std::vector<std::shared_ptr<MeshRenderer>> new_meshes) {
    meshes_.insert(meshes_.end(), new_meshes.begin(), new_meshes.end());
  }

  void RemoveLastMesh() { if (meshes_.size() > 0) meshes_.pop_back(); }
  void ClearMeshes() { meshes_.clear(); }

  void TranslateCamera(glm::vec3 translation) { camera_.position += translation; }
  void RotateCameraLocal(glm::quat rotation) {
    camera_.rotation = normalize(camera_.rotation * rotation);
  }
  void RotateCameraGlobal(glm::quat rotation) {
    camera_.rotation = normalize(rotation * camera_.rotation);
  }

  glm::vec3 GetCameraPosition() { return camera_.position; }
  glm::quat GetCameraRotation() { return camera_.rotation; }
  glm::mat4 GetCameraViewMatrix() { return camera_.view; }
  glm::mat4 GetCameraProjMatrix() { return camera_.proj; }
  glm::mat4 GetCameraPrerotationMatrix() { return camera_.prerotation; }
  float GetCameraAspectRatio() { return camera_.aspect_ratio; };
  float GetCameraFOV() { return camera_.fov; }

  void SetCameraFOV(float fov) { camera_.fov = fov; }
  void SetCameraAspectRatio(float aspect_ratio) { camera_.aspect_ratio = aspect_ratio; }
  void SetCameraViewMatrix(glm::mat4 view_mat) { camera_.view = view_mat; }
  void SetCameraProjMatrix(glm::mat4 proj_mat) { camera_.proj = proj_mat; }
  void SetCameraPrerotationMatrix(glm::mat3 prerot ) { camera_.prerotation = prerot; }

 private:
  Camera camera_;
  std::vector<std::shared_ptr<MeshRenderer>> meshes_;

  glm::mat4 GetTransform(MeshInstance instance) const {
    glm::mat4 position = glm::translate(glm::mat4(1.0), instance.position);
    glm::mat4 scale = glm::scale(glm::mat4(1.0), instance.scale);
    return position * glm::mat4(instance.rotation) * scale;
  }

  BoundingBox GetBoundingBoxWorldSpace(std::shared_ptr<Geometry> geometry, MeshInstance instance) const {
  glm::mat4 finalTransform = GetTransform(instance);
  BoundingBox originalBox = geometry->GetBoundingBox();

  glm::vec3 xMin = finalTransform[0] * (originalBox.min.x);
  glm::vec3 xMax = finalTransform[0] * (originalBox.max.x);

  glm::vec3 yMin = finalTransform[1] * (originalBox.min.y);
  glm::vec3 yMax = finalTransform[1] * (originalBox.max.y);

  glm::vec3 zMin = finalTransform[2] * (originalBox.min.z);
  glm::vec3 zMax = finalTransform[2] * (originalBox.max.z);

  return BoundingBox {
      .min = glm::min(xMin, xMax) + glm::min(yMin ,yMax) + glm::min(zMin, zMax) + glm::vec3(finalTransform[3]),
      .max = glm::max(xMin, xMax) + glm::max(yMin ,yMax) + glm::max(zMin, zMax) + glm::vec3(finalTransform[3]),
    };
  }
};

#endif //BENDER_BASE_APP_SRC_MAIN_JNI_RENDER_GRAPH_H_
