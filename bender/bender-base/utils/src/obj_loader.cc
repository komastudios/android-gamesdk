//
// Created by theoking on 12/13/2019.
//

#include "obj_loader.h"
#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"
#include <set>

namespace OBJLoader {

void AddVertex(glm::vec3 &currVert,
               OBJ &currOBJ,
               std::vector<glm::vec3> &position,
               std::vector<glm::vec3> &normal,
               std::vector<glm::vec2> &texCoord,
               glm::vec3 tangent,
               glm::vec3 bitangent) {
  if (currOBJ.vert_to_index.find(currVert) != currOBJ.vert_to_index.end()) {
    uint16_t index = currOBJ.vert_to_index[currVert];
    currOBJ.index_buffer.push_back(index);
    currOBJ.vertex_buffer[index * 14 + 6] += tangent.x;
    currOBJ.vertex_buffer[index * 14 + 7] += tangent.y;
    currOBJ.vertex_buffer[index * 14 + 8] += tangent.z;
    currOBJ.vertex_buffer[index * 14 + 9] += bitangent.x;
    currOBJ.vertex_buffer[index * 14 + 10] += bitangent.y;
    currOBJ.vertex_buffer[index * 14 + 11] += bitangent.z;
    return;
  }

  currOBJ.index_buffer.push_back(currOBJ.vert_to_index.size());
  currOBJ.vert_to_index[currVert] = currOBJ.vert_to_index.size();

  glm::vec3 pos = position[TrueIndex(currVert.x, position.size())];
  currOBJ.vertex_buffer.push_back(pos.x);
  currOBJ.vertex_buffer.push_back(pos.y);
  currOBJ.vertex_buffer.push_back(pos.z);

  currOBJ.vertex_buffer.push_back(normal[TrueIndex(currVert.z, normal.size())].x);
  currOBJ.vertex_buffer.push_back(normal[TrueIndex(currVert.z, normal.size())].y);
  currOBJ.vertex_buffer.push_back(normal[TrueIndex(currVert.z, normal.size())].z);

  currOBJ.vertex_buffer.push_back(tangent.x);
  currOBJ.vertex_buffer.push_back(tangent.y);
  currOBJ.vertex_buffer.push_back(tangent.z);

  currOBJ.vertex_buffer.push_back(bitangent.x);
  currOBJ.vertex_buffer.push_back(bitangent.y);
  currOBJ.vertex_buffer.push_back(bitangent.z);

  currOBJ.vertex_buffer.push_back(texCoord[TrueIndex(currVert.y, texCoord.size())].x);
  currOBJ.vertex_buffer.push_back(texCoord[TrueIndex(currVert.y, texCoord.size())].y);

  if (pos.x > currOBJ.box.max.x) currOBJ.box.max.x = pos.x;
  if (pos.x < currOBJ.box.min.x) currOBJ.box.min.x = pos.x;
  if (pos.y > currOBJ.box.max.y) currOBJ.box.max.y = pos.y;
  if (pos.y < currOBJ.box.min.y) currOBJ.box.min.y = pos.y;
  if (pos.z > currOBJ.box.max.z) currOBJ.box.max.z = pos.z;
  if (pos.z < currOBJ.box.min.z) currOBJ.box.min.z = pos.z;
}

void LoadMTL(AAssetManager *mgr,
             const std::string &fileName,
             std::unordered_map<std::string, MTL> &mtllib) {

  AAsset *file = AAssetManager_open(mgr, "models/starship_command_center_triangle.mtl", AASSET_MODE_BUFFER);
  const char *fileContent = static_cast<const char *>(AAsset_getBuffer(file));

  std::stringstream data(std::string(fileContent, AAsset_getLength(file)));

  ParseMTL(data, mtllib);
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

  AAsset *file = AAssetManager_open(mgr, fileName.c_str(), AASSET_MODE_BUFFER);
  const char *fileContent = static_cast<const char *>(AAsset_getBuffer(file));

  std::stringstream data(std::string(fileContent, AAsset_getLength(file)));

  std::string line;
  while (std::getline(data, line) && !data.eof()) {
    std::stringstream lineStream(line);
    std::string label;
    lineStream >> label;
    if (label == "mtllib") {
      std::string mtllibName;
      lineStream >> mtllibName;
      LoadMTL(mgr, mtllibName, mtllib);
    } else if (label == "v") {
      float x, y, z;
      lineStream >> x >> y >> z;
      position.push_back({x, y, z});
    } else if (label == "vt") {
      float x, y;
      lineStream >> x >> y;
      texCoord.push_back({x, 1-y});
    } else if (label == "vn") {
      float x, y, z;
      lineStream >> x >> y >> z;
      normal.push_back({x, y, z});
    } else if (label == "usemtl") {
      std::string materialName;
      lineStream >> materialName;
      modelData.back().box.center = (modelData.back().box.max + modelData.back().box.min) * .5f;
      OBJ curr;
      curr.material_name = materialName;
      modelData.push_back(curr);
    } else if (label == "f") {
      glm::vec3 vertex1, vertex2, vertex3;
      char skip;
      lineStream >> vertex1.x >> skip >> vertex1.y >> skip >> vertex1.z;
      lineStream >> vertex2.x >> skip >> vertex2.y >> skip >> vertex2.z;
      lineStream >> vertex3.x >> skip >> vertex3.y >> skip >> vertex3.z;

      glm::vec3 posVec1 = position[TrueIndex(vertex1.x, position.size())];
      glm::vec3 posVec2 = position[TrueIndex(vertex2.x, position.size())];
      glm::vec3 posVec3 = position[TrueIndex(vertex3.x, position.size())];

      glm::vec2 texVec1 = texCoord[TrueIndex(vertex1.y, texCoord.size())];
      glm::vec2 texVec2 = texCoord[TrueIndex(vertex2.y, texCoord.size())];
      glm::vec2 texVec3 = texCoord[TrueIndex(vertex3.y, texCoord.size())];

      glm::vec3 edge1 = posVec2 - posVec1;
      glm::vec3 edge2 = posVec3 - posVec1;
      glm::vec2 deltaUV1 = texVec2 - texVec1;
      glm::vec2 deltaUV2 = texVec3 - texVec1;

      float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
      glm::vec3 tangent, bitangent;
      tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
      bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

      AddVertex(vertex3, modelData.back(), position, normal, texCoord, tangent, bitangent);
      AddVertex(vertex2, modelData.back(), position, normal, texCoord, tangent, bitangent);
      AddVertex(vertex1, modelData.back(), position, normal, texCoord, tangent, bitangent);

      if (modelData.back().vert_to_index.size() > 65500){
        modelData.back().box.center = (modelData.back().box.max + modelData.back().box.min) * .5f;
        OBJ curr;
        curr.material_name = modelData.back().material_name;
        modelData.push_back(curr);
      }
    }
  }
  AAsset_close(file);
}

void generateTSpacePolyhedra(std::vector<float> vertexData, std::vector<uint16_t> indexData, std::vector<float> &out){
  out.resize((vertexData.size() / 8) * 14);
  std::set<uint16_t> completed;

  for (int x = 0; x < indexData.size(); x += 3){
    uint16_t idx1, idx2, idx3;
    idx1 = indexData[x];
    idx2 = indexData[x+1];
    idx3 = indexData[x+2];

    glm::vec3 posVec1 = {vertexData[idx1 * 8], vertexData[idx1 * 8 + 1], vertexData[idx1 * 8 + 2]};
    glm::vec3 posVec2 = {vertexData[idx2 * 8], vertexData[idx2 * 8 + 1], vertexData[idx2 * 8 + 2]};
    glm::vec3 posVec3 = {vertexData[idx3 * 8], vertexData[idx3 * 8 + 1], vertexData[idx3 * 8 + 2]};

    glm::vec2 texVec1 = {vertexData[idx1 * 8 + 6], vertexData[idx1 * 8 + 7]};
    glm::vec2 texVec2 = {vertexData[idx2 * 8 + 6], vertexData[idx2 * 8 + 7]};
    glm::vec2 texVec3 = {vertexData[idx3 * 8 + 6], vertexData[idx3 * 8 + 7]};

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

    if (completed.find(idx1) == completed.end()){
      out[idx1 * 14] = posVec1.x;
      out[idx1 * 14 + 1] = posVec1.y;
      out[idx1 * 14 + 2] = posVec1.z;
      out[idx1 * 14 + 3] = vertexData[idx1 * 8 + 3];
      out[idx1 * 14 + 4] = vertexData[idx1 * 8 + 4];
      out[idx1 * 14 + 5] = vertexData[idx1 * 8 + 5];
      out[idx1 * 14 + 6] = tangent.x;
      out[idx1 * 14 + 7] = tangent.y;
      out[idx1 * 14 + 8] = tangent.z;
      out[idx1 * 14 + 9] = bitangent.x;
      out[idx1 * 14 + 10] = bitangent.y;
      out[idx1 * 14 + 11] = bitangent.z;
      out[idx1 * 14 + 12] = texVec1.x;
      out[idx1 * 14 + 13] = texVec1.y;
      completed.insert(idx1);
    }

    if (completed.find(idx2) == completed.end()){
      out[idx2 * 14] = posVec2.x;
      out[idx2 * 14 + 1] = posVec2.y;
      out[idx2 * 14 + 2] = posVec2.z;
      out[idx2 * 14 + 3] = vertexData[idx2 * 8 + 3];
      out[idx2 * 14 + 4] = vertexData[idx2 * 8 + 4];
      out[idx2 * 14 + 5] = vertexData[idx2 * 8 + 5];
      out[idx2 * 14 + 6] = tangent.x;
      out[idx2 * 14 + 7] = tangent.y;
      out[idx2 * 14 + 8] = tangent.z;
      out[idx2 * 14 + 9] = bitangent.x;
      out[idx2 * 14 + 10] = bitangent.y;
      out[idx2 * 14 + 11] = bitangent.z;
      out[idx2 * 14 + 12] = texVec2.x;
      out[idx2 * 14 + 13] = texVec2.y;
      completed.insert(idx2);
    }

    if (completed.find(idx3) == completed.end()){
      out[idx3 * 14] = posVec3.x;
      out[idx3 * 14 + 1] = posVec3.y;
      out[idx3 * 14 + 2] = posVec3.z;
      out[idx3 * 14 + 3] = vertexData[idx3 * 8 + 3];
      out[idx3 * 14 + 4] = vertexData[idx3 * 8 + 4];
      out[idx3 * 14 + 5] = vertexData[idx3 * 8 + 5];
      out[idx3 * 14 + 6] = tangent.x;
      out[idx3 * 14 + 7] = tangent.y;
      out[idx3 * 14 + 8] = tangent.z;
      out[idx3 * 14 + 9] = bitangent.x;
      out[idx3 * 14 + 10] = bitangent.y;
      out[idx3 * 14 + 11] = bitangent.z;
      out[idx3 * 14 + 12] = texVec3.x;
      out[idx3 * 14 + 13] = texVec3.y;
      completed.insert(idx3);
    }
  }
}

}