//
// Created by theoking on 12/13/2019.
//

#include "obj_loader.h"
#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"

namespace OBJLoader {

int trueIndex(int idx, int size) {
  if (idx < 0) {
    return size + idx;
  } else {
    return idx - 1;
  }
}

void addVertex(glm::vec3 &currVert,
               OBJ &currOBJ,
               std::unordered_map<glm::vec3, uint32_t> &fileVertToIndex,
               std::vector<glm::vec3> &position,
               std::vector<glm::vec3> &normal,
               std::vector<glm::vec2> &texCoord) {
  if (fileVertToIndex.find(currVert) != fileVertToIndex.end()) {
    currOBJ.indexBuffer.push_back(currOBJ.vertToIndex[currVert]);
    return;
  }

  fileVertToIndex[currVert] = fileVertToIndex.size();
  currOBJ.indexBuffer.push_back(currOBJ.vertToIndex.size());
  currOBJ.vertToIndex[currVert] = currOBJ.vertToIndex.size();

  currOBJ.vertexBuffer.push_back(position[trueIndex(currVert.x, position.size())].x);
  currOBJ.vertexBuffer.push_back(position[trueIndex(currVert.x, position.size())].y);
  currOBJ.vertexBuffer.push_back(position[trueIndex(currVert.x, position.size())].z);

  currOBJ.vertexBuffer.push_back(normal[trueIndex(currVert.z, normal.size())].x);
  currOBJ.vertexBuffer.push_back(normal[trueIndex(currVert.z, normal.size())].y);
  currOBJ.vertexBuffer.push_back(normal[trueIndex(currVert.z, normal.size())].z);

  currOBJ.vertexBuffer.push_back(texCoord[trueIndex(currVert.y, texCoord.size())].x);
  currOBJ.vertexBuffer.push_back(texCoord[trueIndex(currVert.y, texCoord.size())].y);
}

void loadMTL(AAssetManager *mgr,
             const std::string &fileName,
             std::unordered_map<std::string, MTL> &mtllib) {

  AAsset *file = AAssetManager_open(mgr, ("models/" + fileName).c_str(), AASSET_MODE_BUFFER);
  size_t fileLength = AAsset_getLength(file);

  char *fileContent = new char[fileLength];
  AAsset_read(file, fileContent, fileLength);

  std::stringstream data;
  data << fileContent;
  std::string label;
  std::string currentMaterialName = "";

  while (data >> label) {
    if (label == "newmtl") {
      data >> currentMaterialName;
      mtllib[currentMaterialName] = MTL();
    } else if (label == "Ns") {
      data >> mtllib[currentMaterialName].specularExponent;
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
    } else if (label == "map_Kd") {
      data >> mtllib[currentMaterialName].map_Kd;
    } else if (label == "map_Ns") {
      data >> mtllib[currentMaterialName].map_Ns;
    } else if (label == "map_bump") {
      data >> mtllib[currentMaterialName].map_bump;
    }
  }
}

// There is a global vertex buffer for an OBJ file
// It is distributed among the position/normal/texCoord vectors
// Multiple models can be defined in a single OBJ file
// Indices in an OBJ file a relative to the global vertex data vectors
// This means you need to convert those global indices
void loadOBJ(AAssetManager *mgr,
             const std::string &fileName,
             std::unordered_map<std::string, MTL> &mtllib,
             std::vector<OBJ> &modelData) {

  std::vector<glm::vec3> position;
  std::vector<glm::vec3> normal;
  std::vector<glm::vec2> texCoord;
  std::unordered_map<glm::vec3, uint32_t> fileVertToIndex;

  AAsset *file = AAssetManager_open(mgr, fileName.c_str(), AASSET_MODE_BUFFER);
  size_t fileLength = AAsset_getLength(file);

  char *fileContent = new char[fileLength];
  AAsset_read(file, fileContent, fileLength);

  std::stringstream data;
  data << fileContent;
  std::string label;

  while (data >> label) {
    if (label == "mtllib") {
      std::string mtllibName;
      data >> mtllibName;
      loadMTL(mgr, mtllibName, mtllib);
    } else if (label == "v") {
      float x, y, z;
      data >> x >> y >> z;
      position.push_back({x, y, z});
    } else if (label == "vt") {
      float x, y;
      data >> x >> y;
      texCoord.push_back({x, y});
    } else if (label == "vn") {
      float x, y, z;
      data >> x >> y >> z;
      normal.push_back({x, y, z});
    } else if (label == "usemtl") {
      std::string materialName;
      data >> materialName;
      OBJ curr;
      curr.materialName = materialName;
      modelData.push_back(curr);
    } else if (label == "f") {
      glm::vec3 vertex1, vertex2, vertex3;
      char skip;
      data >> vertex1.x >> skip >> vertex1.y >> skip >> vertex1.z;
      data >> vertex2.x >> skip >> vertex2.y >> skip >> vertex2.z;
      data >> vertex3.x >> skip >> vertex3.y >> skip >> vertex3.z;
      addVertex(vertex3, modelData.back(), fileVertToIndex, position, normal, texCoord);
      addVertex(vertex2, modelData.back(), fileVertToIndex, position, normal, texCoord);
      addVertex(vertex1, modelData.back(), fileVertToIndex, position, normal, texCoord);
    }
  }
}

}