//
// Created by theoking on 2/18/2020.
//


#include <unordered_map>
#include <string>
#include <sstream>
#include "glm/glm.hpp"

#ifndef BENDER_BASE_UTILS_SRC_MESH_HELPERS_H_
#define BENDER_BASE_UTILS_SRC_MESH_HELPERS_H_

struct MTL {
  glm::vec3 ambient_;
  glm::vec3 diffuse_;
  glm::vec3 specular_;
  float specular_exponent_;
  float bump_multiplier = 1.0f;

  std::string map_Ka_ = "";
  std::string map_Kd_ = "";
  std::string map_Ke_ = "";
  std::string map_Ks_ = "";
  std::string map_Ns_ = "";
  std::string map_Bump_ = "";
};

void parseMTL(std::iostream &data,
              std::unordered_map<std::string, MTL> &mtllib);

#endif //BENDER_BASE_UTILS_SRC_MESH_HELPERS_H_
