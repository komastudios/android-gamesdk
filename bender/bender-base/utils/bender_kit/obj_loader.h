//
// Created by theoking on 12/13/2019.
//

#define GLM_FORCE_CXX17
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <android/asset_manager.h>

#ifndef BENDER_BASE_UTILS_BENDER_KIT_OBJ_LOADER_H_
#define BENDER_BASE_UTILS_BENDER_KIT_OBJ_LOADER_H_

namespace OBJLoader {

struct MTL {
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float specularExponent;

  std::string map_Ka = "";
  std::string map_Kd = "";
  std::string map_Ks = "";
  std::string map_Ns = "";
  std::string map_bump = "";
};

struct OBJ {
  std::string name;
  std::string materialName;
  std::vector<uint32_t> indexBuffer;
};

void loadOBJ(AAssetManager *mgr,
             const std::string &fileName,
             std::vector<float> &vertexData,
             std::unordered_map<std::string, MTL> &mtllib,
             std::vector<OBJ> &modelData);

};

#endif //BENDER_BASE_UTILS_BENDER_KIT_OBJ_LOADER_H_
