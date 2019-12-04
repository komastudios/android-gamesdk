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

/*
Usage Example:

        std::vector<float> vertexData;
        std::vector<OBJLoader::OBJ> modelData;
        std::unordered_map<std::string, OBJLoader::MTL> mtllib;
        OBJLoader::loadOBJ(app->activity->assetManager,
                           "models/millenium-falcon-blender.obj",
                           vertexData,
                           mtllib,
                           modelData);

        for (auto currMTL : mtllib) {
          addTexture(currMTL.second.map_Ka);
          addTexture(currMTL.second.map_Kd);
          addTexture(currMTL.second.map_Ks);
          addTexture(currMTL.second.map_Ns);
          addTexture(currMTL.second.map_bump);
          MaterialAttributes newMTL;
          newMTL.ambient = currMTL.second.ambient;
          newMTL.specular = currMTL.second.specular;
          newMTL.diffuse = currMTL.second.diffuse;
          newMTL.specularExponent = currMTL.second.specularExponent;
          testMaterials[currMTL.first] = std::make_shared<Material>(renderer,
                                                                      shaders,
                                                                      loadedTextures[currMTL.second.map_Kd],
                                                                      newMTL);
        }
        for (auto obj : modelData) {
          render_graph->addMesh(std::make_shared<Mesh>(renderer,
                                    testMaterials[obj.materialName],
                                    std::make_shared<Geometry>(*device,
                                                               vertexData,
                                                               obj.indexBuffer)));


 */

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
