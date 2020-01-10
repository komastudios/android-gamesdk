//
// Created by theoking on 12/13/2019.
//

#include "obj_loader.h"
#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"
#include "vgc/vcg/complex/algorithms/clustering.h"

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
               std::unordered_map<glm::vec3, uint16_t> &fileVertToIndex,
               std::vector<glm::vec3> &position,
               std::vector<glm::vec3> &normal,
               std::vector<glm::vec2> &texCoord,
               glm::vec3 tangent,
               glm::vec3 bitangent) {
  if (fileVertToIndex.find(currVert) != fileVertToIndex.end()) {
    currOBJ.index_buffer_.push_back(currOBJ.vert_to_index_[currVert]);
    return;
  }

  fileVertToIndex[currVert] = fileVertToIndex.size();
  currOBJ.index_buffer_.push_back(currOBJ.vert_to_index_.size());
  currOBJ.vert_to_index_[currVert] = currOBJ.vert_to_index_.size();

  currOBJ.vertex_buffer_.push_back(position[trueIndex(currVert.x, position.size())].x);
  currOBJ.vertex_buffer_.push_back(position[trueIndex(currVert.x, position.size())].y);
  currOBJ.vertex_buffer_.push_back(position[trueIndex(currVert.x, position.size())].z);

  currOBJ.vertex_buffer_.push_back(normal[trueIndex(currVert.z, normal.size())].x);
  currOBJ.vertex_buffer_.push_back(normal[trueIndex(currVert.z, normal.size())].y);
  currOBJ.vertex_buffer_.push_back(normal[trueIndex(currVert.z, normal.size())].z);

  currOBJ.vertex_buffer_.push_back(tangent.x);
  currOBJ.vertex_buffer_.push_back(tangent.y);
  currOBJ.vertex_buffer_.push_back(tangent.z);

  currOBJ.vertex_buffer_.push_back(bitangent.x);
  currOBJ.vertex_buffer_.push_back(bitangent.y);
  currOBJ.vertex_buffer_.push_back(bitangent.z);

  currOBJ.vertex_buffer_.push_back(texCoord[trueIndex(currVert.y, texCoord.size())].x);
  currOBJ.vertex_buffer_.push_back(texCoord[trueIndex(currVert.y, texCoord.size())].y);
}

void LoadMTL(AAssetManager *mgr,
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
    } else if (label == "map_bump") {
      data >> skip;
      data >> mtllib[currentMaterialName].bump_multiplier;
      data >> mtllib[currentMaterialName].map_bump_;
    }
  }
}

// There is a global vertex buffer for an OBJ file
// It is distributed among the position/normal/texCoord vectors
// Multiple models can be defined in a single OBJ file
// Indices in an OBJ file are relative to the global vertex data vectors
// This means you need to convert those global indices to
// Indices for vertex buffers of individual models
void LoadOBJ(AAssetManager *mgr,
             const std::string &fileName,
             std::unordered_map<std::string, MTL> &mtllib,
             std::vector<OBJ> &modelData) {

  std::vector<glm::vec3> position;
  std::vector<glm::vec3> normal;
  std::vector<glm::vec2> texCoord;
  std::unordered_map<glm::vec3, uint16_t> fileVertToIndex;

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
      LoadMTL(mgr, mtllibName, mtllib);
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
      curr.material_name_ = materialName;
      modelData.push_back(curr);
    } else if (label == "f") {
      glm::vec3 vertex1, vertex2, vertex3;
      char skip;
      data >> vertex1.x >> skip >> vertex1.y >> skip >> vertex1.z;
      data >> vertex2.x >> skip >> vertex2.y >> skip >> vertex2.z;
      data >> vertex3.x >> skip >> vertex3.y >> skip >> vertex3.z;

      glm::vec3 posVec1 = position[trueIndex(vertex1.x, position.size())];
      glm::vec3 posVec2 = position[trueIndex(vertex2.x, position.size())];
      glm::vec3 posVec3 = position[trueIndex(vertex3.x, position.size())];

      glm::vec2 texVec1 = texCoord[trueIndex(vertex1.y, texCoord.size())];
      glm::vec2 texVec2 = texCoord[trueIndex(vertex2.y, texCoord.size())];
      glm::vec2 texVec3 = texCoord[trueIndex(vertex3.y, texCoord.size())];

      glm::vec3 edge1 = posVec2 - posVec1;
      glm::vec3 edge2 = posVec3 - posVec1;
      glm::vec2 deltaUV1 = texVec2 - texVec1;
      glm::vec2 deltaUV2 = texVec3 - texVec1;

      float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
      glm::vec3 tangent, bitangent;

      tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
      tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
      tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
      tangent = glm::normalize(tangent);

      bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
      bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
      bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
      bitangent = glm::normalize(bitangent);

      addVertex(vertex3, modelData.back(), fileVertToIndex, position, normal, texCoord, tangent, bitangent);
      addVertex(vertex2, modelData.back(), fileVertToIndex, position, normal, texCoord, tangent, bitangent);
      addVertex(vertex1, modelData.back(), fileVertToIndex, position, normal, texCoord, tangent, bitangent);
    }
  }
}

}