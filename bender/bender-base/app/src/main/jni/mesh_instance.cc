//
// Created by Matt Wang on 2020-01-28.
//

#include "mesh_instance.h"

MeshInstance::MeshInstance() {
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

MeshInstance::~MeshInstance() {}

glm::mat4 MeshInstance::GetTransform() const {
}