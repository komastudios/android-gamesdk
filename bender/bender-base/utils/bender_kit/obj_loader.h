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

std::map<std::string, std::shared_ptr<Texture>> loadedTextures;
std::map<std::string, std::shared_ptr<Material>> loadedMaterials;

void addTexture(std::string fileName){
  if (fileName != "" && loadedTextures.find(fileName) == loadedTextures.end()){
    loadedTextures[fileName] = std::make_shared<Texture>(*device,
                                                         *androidAppCtx,
                                                         ("textures/" + fileName).c_str(),
                                                         VK_FORMAT_R8G8B8A8_SRGB);
  }
}

...

std::vector<OBJLoader::OBJ> modelData;
std::unordered_map<std::string, OBJLoader::MTL> mtllib;
OBJLoader::LoadOBJ(app->activity->assetManager,
                   "models/millenium-falcon-blender.obj",
                   mtllib,
                   modelData);

for (auto currMTL : mtllib) {
  addTexture(currMTL.second.map_Ka_);
  addTexture(currMTL.second.map_Kd_);
  addTexture(currMTL.second.map_Ks_);
  addTexture(currMTL.second.map_Ns_);
  addTexture(currMTL.second.map_bump_);
  MaterialAttributes newMTL;
  newMTL.ambient_ = currMTL.second.ambient_;
  newMTL.specular_ = glm::vec4(currMTL.second.specular_, currMTL.second.specular_exponent_);
  newMTL.diffuse_ = currMTL.second.diffuse_;
  loadedMaterials[currMTL.first] = std::make_shared<Material>(renderer,
                                                              shaders,
                                                              loadedTextures[currMTL.second.map_Kd_],
                                                              newMTL);
}
for (auto obj : modelData) {
  render_graph->addMesh(std::make_shared<Mesh>(renderer,
                                               loadedMaterials[obj.material_name_],
                                               std::make_shared<Geometry>(*device,
                                                                          obj.vertex_buffer_,
                                                                          obj.index_buffer_)));
}
 */

namespace OBJLoader {

struct MTL {
  glm::vec3 ambient_;
  glm::vec3 diffuse_;
  glm::vec3 specular_;
  float specular_exponent_;

  std::string map_Ka_ = "";
  std::string map_Kd_ = "";
  std::string map_Ks_ = "";
  std::string map_Ns_ = "";
  std::string map_bump_ = "";
};

struct OBJ {
  std::string name_;
  std::string material_name_;
  std::vector<float> vertex_buffer_;
  std::vector<uint16_t> index_buffer_;
  std::unordered_map<glm::vec3, uint16_t> vert_to_index_;
};

void LoadOBJ(AAssetManager *mgr,
             const std::string &fileName,
             std::unordered_map<std::string, MTL> &mtllib,
             std::vector<OBJ> &modelData);

};

#endif //BENDER_BASE_UTILS_BENDER_KIT_OBJ_LOADER_H_
