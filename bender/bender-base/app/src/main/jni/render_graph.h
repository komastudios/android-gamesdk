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
#include "camera.h"

class RenderGraph {
 public:
  void GetVisibleMeshes(std::vector<std::shared_ptr<Mesh>> &meshes) const;
  void GetAllMeshes(std::vector<std::shared_ptr<Mesh>> &meshes) const { meshes = meshes_; }
  std::shared_ptr<Mesh> GetLastMesh() const;

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

  Camera GetCamera() { return camera_; }

  void SetCameraFOV(float fov);
  void SetCameraAspectRatio(float aspect_ratio);
  void SetCameraViewMatrix(glm::mat4 view_mat) { camera_.view = view_mat; }
  void SetCameraProjMatrix(glm::mat4 proj_mat) { camera_.proj = proj_mat; }
  void SetCameraPrerotationMatrix(glm::mat3 prerot ) { camera_.prerotation = prerot; }

 private:
  Camera camera_;
  std::vector<std::shared_ptr<Mesh>> meshes_;
  void UpdateCameraPlanes();
  std::array<Plane, 6> GenerateFrustumPlanes() const;
};

#endif //BENDER_BASE_APP_SRC_MAIN_JNI_RENDER_GRAPH_H_
