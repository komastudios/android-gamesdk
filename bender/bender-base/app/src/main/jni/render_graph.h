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
  Camera camera;
  std::vector<std::shared_ptr<Mesh>> meshes;
  uint32_t materials_idx = 0;
  uint32_t poly_faces_idx = 0;

};

#endif //BENDER_BASE_APP_SRC_MAIN_JNI_RENDER_GRAPH_H_
