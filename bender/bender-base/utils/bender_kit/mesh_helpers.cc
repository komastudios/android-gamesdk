//
// Created by theoking on 2/18/2020.
//

#include "mesh_helpers.h"

int TrueIndex(int idx, int size) {
  if (idx < 0) {
    return size + idx;
  } else {
    return idx - 1;
  }
}

void ParseMTL(std::iostream &data,
              std::unordered_map<std::string, MTL> &mtllib) {

  std::string label;
  std::string currentMaterialName = "";
  std::string skip;

  while (data >> label) {
    if (label == "newmtl") {
      data >> currentMaterialName;
      mtllib[currentMaterialName] = MTL();
    } else if (label == "Ns") {
      data >> mtllib[currentMaterialName].specular_exponent;
    } else if (label == "Ka") {
      data >> mtllib[currentMaterialName].ambient.x >>
           mtllib[currentMaterialName].ambient.y >>
           mtllib[currentMaterialName].ambient.z;
    } else if (label == "Kd") {
      data >> mtllib[currentMaterialName].diffuse.x >>
           mtllib[currentMaterialName].diffuse.y >>
           mtllib[currentMaterialName].diffuse.z;
    } else if (label == "Ks") {
      data >> mtllib[currentMaterialName].specular.x >>
           mtllib[currentMaterialName].specular.y >>
           mtllib[currentMaterialName].specular.z;
    } else if (label == "map_Ka") {
      data >> mtllib[currentMaterialName].map_Ka;
    } else if (label == "map_Ks") {
      data >> mtllib[currentMaterialName].map_Ks;
    } else if (label == "map_Ke") {
      data >> mtllib[currentMaterialName].map_Ke;
    } else if (label == "map_Kd") {
      data >> mtllib[currentMaterialName].map_Kd;
    } else if (label == "map_Ns") {
      data >> mtllib[currentMaterialName].map_Ns;
    } else if (label == "map_Bump") {
      skip = data.get();
      skip = data.peek();
      if (skip == "-") {
        data >> skip >> mtllib[currentMaterialName].bump_multiplier
             >> mtllib[currentMaterialName].map_Bump;
      } else {
        data >> mtllib[currentMaterialName].map_Bump;
      }
    }
  }
}