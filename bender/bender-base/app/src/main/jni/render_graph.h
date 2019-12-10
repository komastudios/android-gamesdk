//
// Created by theoking on 12/9/2019.
//

#ifndef BENDER_BASE_APP_SRC_MAIN_JNI_RENDER_GRAPH_H_
#define BENDER_BASE_APP_SRC_MAIN_JNI_RENDER_GRAPH_H_

#include "shader_state.h"
#include "mesh.h"
#include "font.h"
#include <chrono>

struct Camera {
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  float aspect_ratio;
  float fov;
  glm::mat4 view;
  glm::mat4 proj;
};

class RenderGraph {

 public:

  void onFocus();

 private:
  bool isInitialized = false;
  Camera camera;

  std::vector<Mesh *> meshes;
  Font *font;

  auto lastTime = std::chrono::high_resolution_clock::now();
  auto currentTime = lastTime;
  float frameTime;
  float totalTime;

  std::vector<const char *> texFiles;
  std::vector<std::shared_ptr<Texture>> textures;
  std::vector<std::shared_ptr<Material>> materials;

  std::vector<std::shared_ptr<Material>> baselineMaterials;
  uint32_t materialsIdx = 0;

  static const std::array<int, 5> allowedPolyFaces = {4, 6, 8, 12, 20};
  uint32_t polyFacesIdx = 0;

  bool windowResized = false;

};

#endif //BENDER_BASE_APP_SRC_MAIN_JNI_RENDER_GRAPH_H_
