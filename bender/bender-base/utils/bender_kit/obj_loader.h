//
// Created by theoking on 12/13/2019.
//

#define GLM_FORCE_CXX17
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"
#include "mesh_helpers.h"
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <android/asset_manager.h>

#ifndef BENDER_BASE_UTILS_BENDER_KIT_OBJ_LOADER_H_
#define BENDER_BASE_UTILS_BENDER_KIT_OBJ_LOADER_H_

/*
Usage Example:

std::map<std::string, std::shared_ptr<Texture>> loaded_textures;
std::map<std::string, std::shared_ptr<Material>> loaded_materials;

void addTexture(std::string fileName){
  if (fileName != "" && loaded_textures.find(fileName) == loaded_textures.end()){
    loaded_textures[fileName] = std::make_shared<Texture>(*device,
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
  addTexture(currMTL.second.map_Ka);
  addTexture(currMTL.second.map_Kd);
  addTexture(currMTL.second.map_Ks);
  addTexture(currMTL.second.map_Ns);
  addTexture(currMTL.second.map_Bump);
  MaterialAttributes newMTL;
  newMTL.ambient = currMTL.second.ambient;
  newMTL.specular = glm::vec4(currMTL.second.specular, currMTL.second.specular_exponent);
  newMTL.diffuse = currMTL.second.diffuse;
  loaded_materials[currMTL.first] = std::make_shared<Material>(renderer,
                                                              shaders,
                                                              loaded_textures[currMTL.second.map_Kd],
                                                              newMTL);
}
for (auto obj : modelData) {
  render_graph->addMesh(std::make_shared<Mesh>(renderer,
                                               loaded_materials[obj.material_name],
                                               std::make_shared<Geometry>(*device,
                                                                          obj.vertex_buffer,
                                                                          obj.index_buffer)));
}
 */

namespace OBJLoader {

struct OBJ {
  std::string name;
  std::string material_name;
  std::vector<float> vertex_buffer;
  std::vector<uint16_t> index_buffer;
  std::unordered_map<glm::vec3, uint16_t> vert_to_index;
};

void LoadOBJ(AAssetManager *mgr,
             const std::string &fileName,
             std::unordered_map<std::string, MTL> &mtllib,
             std::vector<OBJ> &modelData);

};

#endif //BENDER_BASE_UTILS_BENDER_KIT_OBJ_LOADER_H_
