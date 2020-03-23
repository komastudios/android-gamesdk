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
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float specular_exponent;
  float bump_multiplier = 1.0f;

  std::string map_Ka;
  std::string map_Kd;
  std::string map_Ke;
  std::string map_Ks;
  std::string map_Ns;
  std::string map_Bump;
};

void ParseMTL(std::iostream &data,
              std::unordered_map<std::string, MTL> &mtllib);

int TrueIndex(int idx, int size);

#endif //BENDER_BASE_UTILS_SRC_MESH_HELPERS_H_
