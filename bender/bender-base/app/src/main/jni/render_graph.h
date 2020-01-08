//
// Created by theoking on 12/9/2019.
//

#ifndef BENDER_BASE_APP_SRC_MAIN_JNI_RENDER_GRAPH_H_
#define BENDER_BASE_APP_SRC_MAIN_JNI_RENDER_GRAPH_H_

#include "shader_state.h"
#include "texture.h"
#include "material.h"
#include "geometry.h"
#include "mesh.h"
#include "device.h"
#include "renderer.h"
#include "input.h"

class RenderGraph {
  struct Camera {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    float aspect_ratio;
    float fov;
    float near = .1f;
    float far = 100.0f;
    float nearPlaneWidth, nearPlaneHeight, farPlaneWidth, farPlaneHeight;
    glm::mat4 view;
    glm::mat4 proj;
    bool prerotated = false;
  };
  struct Plane {
    glm::vec3 point;
    glm::vec3 normal;
  };
  enum FrustumPlanes {
    TOP = 0, BOTTOM, LEFT,
    RIGHT, NEARP, FARP
  };

 public:
  void GetVisibleMeshes(std::vector<std::shared_ptr<Mesh>> &meshes) const;
  void GetAllMeshes(std::vector<std::shared_ptr<Mesh>> &meshes) const { meshes = meshes_; }
  std::shared_ptr<Mesh> GetLastMesh() {
    if (meshes_.size() > 0) {
      return meshes_.back();
    } else {
      return nullptr;
    }
  }

  void AddMesh(std::shared_ptr<Mesh> new_mesh) { meshes_.push_back(new_mesh); }
  void AddMeshes(std::vector<std::shared_ptr<Mesh>> new_meshes) {
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
  float GetCameraAspectRatio() { return camera_.aspect_ratio; };
  float GetCameraFOV() { return camera_.fov; }
  float GetCameraNearPlane() { return camera_.near; }
  float GetCameraFarPlane() { return camera_.far; }

  void SetCameraPrerotated(bool prerotated);
  void SetCameraFOV(float fov);
  void SetCameraAspectRatio(float aspect_ratio);
  void SetCameraViewMatrix(glm::mat4 view_mat) { camera_.view = view_mat; }
  void SetCameraProjMatrix(glm::mat4 proj_mat) { camera_.proj = proj_mat; }

 private:
  Camera camera_;
  std::vector<std::shared_ptr<Mesh>> meshes_;
  void UpdateCameraPlanes();
};

#endif //BENDER_BASE_APP_SRC_MAIN_JNI_RENDER_GRAPH_H_
