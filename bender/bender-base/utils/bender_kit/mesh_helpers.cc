//
// Created by theoking on 2/18/2020.
//

#include "mesh_helpers.h"

void parseMTL(std::iostream &data,
              std::unordered_map<std::string, MTL> &mtllib) {

  std::string label;
  std::string currentMaterialName = "";
  std::string skip;

  while (data >> label) {
    if (label == "newmtl") {
      data >> currentMaterialName;
      mtllib[currentMaterialName] = MTL();
    } else if (label == "Ns") {
      data >> mtllib[currentMaterialName].specular_exponent_;
    } else if (label == "Ka") {
      data >> mtllib[currentMaterialName].ambient_.x >>
           mtllib[currentMaterialName].ambient_.y >>
           mtllib[currentMaterialName].ambient_.z;
    } else if (label == "Kd") {
      data >> mtllib[currentMaterialName].diffuse_.x >>
           mtllib[currentMaterialName].diffuse_.y >>
           mtllib[currentMaterialName].diffuse_.z;
    } else if (label == "Ks") {
      data >> mtllib[currentMaterialName].specular_.x >>
           mtllib[currentMaterialName].specular_.y >>
           mtllib[currentMaterialName].specular_.z;
    } else if (label == "map_Ka") {
      data >> mtllib[currentMaterialName].map_Ka_;
    } else if (label == "map_Ks") {
      data >> mtllib[currentMaterialName].map_Ks_;
    } else if (label == "map_Ke") {
      data >> mtllib[currentMaterialName].map_Ke_;
    } else if (label == "map_Kd") {
      data >> mtllib[currentMaterialName].map_Kd_;
    } else if (label == "map_Ns") {
      data >> mtllib[currentMaterialName].map_Ns_;
    } else if (label == "map_Bump") {
      skip = data.get();
      skip = data.peek();
      if (skip == "-") {
        data >> skip >> mtllib[currentMaterialName].bump_multiplier
             >> mtllib[currentMaterialName].map_Bump_;
      } else {
        data >> mtllib[currentMaterialName].map_Bump_;
      }
    }
  }
}