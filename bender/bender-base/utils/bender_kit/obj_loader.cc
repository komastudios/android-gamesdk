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
               std::vector<float> &vertexData,
               std::unordered_map<glm::vec3, uint32_t> &vertToIndex,
               std::vector<glm::vec3> &position,
               std::vector<glm::vec3> &normal,
               std::vector<glm::vec2> &texCoord) {
  uint32_t index;
  if (vertToIndex.find(currVert) != vertToIndex.end()) {
    currOBJ.indexBuffer.push_back(vertToIndex[currVert]);
    return;
  }

  index = vertToIndex.size();
  vertToIndex[currVert] = index;
  currOBJ.indexBuffer.push_back(vertToIndex[currVert]);
  vertexData.push_back(position[trueIndex(currVert.x, position.size())].x);
  vertexData.push_back(position[trueIndex(currVert.x, position.size())].y);
  vertexData.push_back(position[trueIndex(currVert.x, position.size())].z);

  vertexData.push_back(normal[trueIndex(currVert.z, normal.size())].x);
  vertexData.push_back(normal[trueIndex(currVert.z, normal.size())].y);
  vertexData.push_back(normal[trueIndex(currVert.z, normal.size())].z);

  vertexData.push_back(texCoord[trueIndex(currVert.y, texCoord.size())].x);
  vertexData.push_back(texCoord[trueIndex(currVert.y, texCoord.size())].y);
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

void loadOBJ(AAssetManager *mgr,
             const std::string &fileName,
             std::vector<float> &vertexData,
             std::unordered_map<std::string, MTL> &mtllib,
             std::vector<OBJ> &modelData) {

  std::vector<glm::vec3> position;
  std::vector<glm::vec3> normal;
  std::vector<glm::vec2> texCoord;
  std::unordered_map<glm::vec3, uint32_t> vertToIndex;


  // Read the file:
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
      addVertex(vertex3, modelData.back(), vertexData, vertToIndex, position, normal, texCoord);
      addVertex(vertex2, modelData.back(), vertexData, vertToIndex, position, normal, texCoord);
      addVertex(vertex1, modelData.back(), vertexData, vertToIndex, position, normal, texCoord);
    }
  }
}

}