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
    glm::mat4 view;
    glm::mat4 proj;
  };

 public:
  std::vector<std::shared_ptr<Mesh>> getAllMeshes() { return meshes; }
  std::shared_ptr<Mesh> getLastMesh() {
    if (meshes.size() > 0) {
      return meshes.back();
    } else {
      return nullptr;
    }
  }

  void addMesh(std::shared_ptr<Mesh> newMesh) { meshes.push_back(newMesh); }
  void addMeshes(std::vector<std::shared_ptr<Mesh>> newMeshes) {
    meshes.insert(meshes.end(), newMeshes.begin(), newMeshes.end());
  }

  void removeLastMesh() { if (meshes.size() > 0) meshes.pop_back(); }
  void clearMeshes() { meshes.clear(); }

  uint32_t getMaterialIdx() { return materials_idx; }
  uint32_t getPolyFacesIdx() { return poly_faces_idx; }
  void setPolyFacesIdx(int newIdx, size_t totalSize) { poly_faces_idx = newIdx % totalSize; }
  void setMaterialsIdx(int newIdx, size_t totalSize) { materials_idx = newIdx % totalSize; }

  void translateCamera(glm::vec3 translation) { camera.position += translation; }
  void rotateCameraLocal(glm::quat rotation) {
    camera.rotation = normalize(camera.rotation * rotation);
  }
  void rotateCameraGlobal(glm::quat rotation) {
    camera.rotation = normalize(rotation * camera.rotation);
  }

  glm::vec3 getCameraPosition() { return camera.position; }
  glm::quat getCameraRotation() { return camera.rotation; }
  glm::mat4 getCameraViewMatrix() { return camera.view; }
  glm::mat4 getCameraProjMatrix() { return camera.proj; }
  float getCameraAspectRatio() { return camera.aspect_ratio; };
  float getCameraFOV() { return camera.fov; }

  void setCameraFOV(float fov) { camera.fov = fov; }
  void setCameraAspectRatio(float aspect_ratio) { camera.aspect_ratio = aspect_ratio; }
  void setCameraViewMatrix(glm::mat4 viewMat) { camera.view = viewMat; }
  void setCameraProjMatrix(glm::mat4 projMat) { camera.proj = projMat; }

 private:
  Camera camera;
  std::vector<std::shared_ptr<Mesh>> meshes;
  uint32_t materials_idx = 0;
  uint32_t poly_faces_idx = 0;

};

#endif //BENDER_BASE_APP_SRC_MAIN_JNI_RENDER_GRAPH_H_
