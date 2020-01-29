#include <vcg/complex/complex.h>

#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/texture.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric_tex.h>

// input output
#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>

#define GLM_FORCE_CXX11
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>
#include <fstream>
#include "../../../../../utils/bender_kit/mesh_helpers.h"

// The class prototypes.
class MyVertex;
class MyEdge;
class MyFace;

struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>::AsVertexType,
                                           vcg::Use<MyEdge>::AsEdgeType,
                                           vcg::Use<MyFace>::AsFaceType> {
};

class MyVertex : public vcg::Vertex<MyUsedTypes,
                                    vcg::vertex::VFAdj,
                                    vcg::vertex::Coord3f,
                                    vcg::vertex::Normal3f,
                                    vcg::vertex::TexCoord2f,
                                    vcg::vertex::Mark,
                                    vcg::vertex::BitFlags> {
};

class MyEdge : public vcg::Edge<MyUsedTypes> {};

typedef BasicVertexPair<MyVertex> VertexPair;

class MyFace : public vcg::Face<MyUsedTypes,
                                vcg::face::VFAdj,
                                vcg::face::FFAdj,
                                vcg::face::VertexRef,
                                vcg::face::WedgeTexCoord2f,
                                vcg::face::Normal3f,
                                vcg::face::BitFlags> {
};

// the main mesh class
class MyMesh : public vcg::tri::TriMesh<std::vector < MyVertex>, std::vector<MyFace>
> {
};

class MyTriEdgeCollapseQTex : public TriEdgeCollapseQuadricTex<MyMesh,
                                                               VertexPair,
                                                               MyTriEdgeCollapseQTex,
                                                               QuadricTexHelper < MyMesh>
> {
public:
typedef TriEdgeCollapseQuadricTex<MyMesh, VertexPair, MyTriEdgeCollapseQTex, QuadricTexHelper<MyMesh>>
    TECQ;
inline MyTriEdgeCollapseQTex(const VertexPair &p, int i, vcg::BaseParameterClass *pp) : TECQ(p,
                                                                                             i,
                                                                                             pp) {}
};

struct Vert {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec3 bitangent;
  glm::vec2 texture;
};

struct OBJ {
  std::string name;
  std::string material_name;
  std::string mtllib_name;
  std::vector<Vert> vertex_buffer;
  std::vector<int> index_buffer;
  std::unordered_map<glm::vec3, int> vert_to_index;
};

void AddVertex(glm::vec3 &currVert,
               OBJ &currOBJ,
               std::vector<glm::vec3> &position,
               std::vector<glm::vec3> &normal,
               std::vector<glm::vec2> &texCoord) {

  if (currOBJ.vert_to_index.find(currVert) != currOBJ.vert_to_index.end()) {
    int index = currOBJ.vert_to_index[currVert];
    currOBJ.index_buffer.push_back(index);
    return;
  }

  Vert newVert{
      .position = position[TrueIndex(currVert.x, position.size())],
      .normal = normal[TrueIndex(currVert.z, normal.size())],
      .texture = texCoord[TrueIndex(currVert.y, texCoord.size())]
  };

  currOBJ.index_buffer.push_back(currOBJ.vert_to_index.size());
  currOBJ.vert_to_index[currVert] = currOBJ.vert_to_index.size();
  currOBJ.vertex_buffer.push_back(newVert);
}

void LoadMTL(const std::string &fileName,
             std::unordered_map<std::string, MTL> &mtllib) {

  std::fstream data;
  data.open(fileName, std::ifstream::in);

  std::string label;
  std::string currentMaterialName = "";
  std::string skip;

  ParseMTL(data, mtllib);
}

// There is a global vertex buffer for an OBJ file
// It is distributed among the position/normal/texCoord vectors
// Multiple models can be defined in a single OBJ file
// Indices in an OBJ file are relative to the global vertex data vectors
// This means you need to convert those global indices to
// Indices for vertex buffers of individual models
void LoadOBJ(const std::string &fileName,
             std::unordered_map<std::string, MTL> &mtllib,
             std::vector<OBJ> &modelData) {

  std::vector<glm::vec3> position;
  std::vector<glm::vec3> normal;
  std::vector<glm::vec2> texCoord;

  std::ifstream data;
  data.open(fileName, std::ifstream::in);

  std::string label;
  std::string mtllibName;

  while (data >> label) {
    if (label == "mtllib") {
      data >> mtllibName;
      LoadMTL(mtllibName, mtllib);
      mtllibName = mtllibName.substr(0, mtllibName.find_last_of("."));
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
      curr.material_name = materialName;
      curr.mtllib_name = mtllibName;
      modelData.push_back(curr);
    } else if (label == "f") {
      glm::vec3 vertex1, vertex2, vertex3;
      char skip;
      data >> vertex1.x >> skip >> vertex1.y >> skip >> vertex1.z;
      data >> vertex2.x >> skip >> vertex2.y >> skip >> vertex2.z;
      data >> vertex3.x >> skip >> vertex3.y >> skip >> vertex3.z;

      AddVertex(vertex1, modelData.back(), position, normal, texCoord);
      AddVertex(vertex2, modelData.back(), position, normal, texCoord);
      AddVertex(vertex3, modelData.back(), position, normal, texCoord);
    }
  }
}

void PreprocessMesh(MyMesh &mesh) {
  vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
  vcg::tri::UpdateTexture<MyMesh>::WedgeTexFromVertexTex(mesh);
  vcg::tri::UpdateNormal<MyMesh>::PerFaceFromCurrentVertexNormal(mesh);
  vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh);
}

void Decimate(MyMesh &mesh, TriEdgeCollapseQuadricTexParameter &qparams, float targetMetric = 250.0f) {
  vcg::math::Quadric<double> QZero;
  QZero.SetZero();
  QuadricTexHelper<MyMesh>::QuadricTemp TD3(mesh.vert, QZero);
  QuadricTexHelper<MyMesh>::TDp3() = &TD3;
  std::vector < std::pair < vcg::TexCoord2 < float > , vcg::Quadric5 < double > > > qv;
  QuadricTexHelper<MyMesh>::Quadric5Temp TD(mesh.vert, qv);
  QuadricTexHelper<MyMesh>::TDp() = &TD;
  
  vcg::LocalOptimization<MyMesh> DeciSession(mesh, &qparams);
  DeciSession.Init<MyTriEdgeCollapseQTex>();
  DeciSession.SetTargetMetric(targetMetric);
  DeciSession.DoOptimization();
  DeciSession.Finalize<MyTriEdgeCollapseQTex>();
  vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalized(mesh);
}

void MeshDecimator(const std::string &file_path, const std::string &file_name) {

  std::vector<OBJ> modelData;
  std::unordered_map<std::string, MTL> mtllib;
  LoadOBJ(file_path, mtllib, modelData);

  int z = 0;
  int totalVertBefore = 0;
  int totalVertAfter = 0;
  for (auto objMesh : modelData) {
    MyMesh vcgMesh;
    MyMesh::VertexIterator
        vi = vcg::tri::Allocator<MyMesh>::AddVertices(vcgMesh, objMesh.vertex_buffer.size());
    MyMesh::FaceIterator
        fi = vcg::tri::Allocator<MyMesh>::AddFaces(vcgMesh, objMesh.index_buffer.size() / 3);

    float pX, pY, pZ, nX, nY, nZ, tX, tY;
    for (int i = 0; i < objMesh.vertex_buffer.size(); i++) {
      pX = objMesh.vertex_buffer[i].position.x;
      pY = objMesh.vertex_buffer[i].position.y;
      pZ = objMesh.vertex_buffer[i].position.z;
      nX = objMesh.vertex_buffer[i].normal.x;
      nY = objMesh.vertex_buffer[i].normal.y;
      nZ = objMesh.vertex_buffer[i].normal.z;
      tX = objMesh.vertex_buffer[i].texture.x;
      tY = objMesh.vertex_buffer[i].texture.y;

      vi->P() = MyMesh::CoordType(pX, pY, pZ);
      vi->N() = MyMesh::CoordType(nX, nY, nZ);
      vi->T() = MyMesh::VertexType::TexCoordType(tX, tY);
      vi++;
    }

    for (int i = 0; i < objMesh.index_buffer.size(); i += 3) {
      fi->V(0) = &vcgMesh.vert[objMesh.index_buffer[i]];
      fi->V(1) = &vcgMesh.vert[objMesh.index_buffer[i + 1]];
      fi->V(2) = &vcgMesh.vert[objMesh.index_buffer[i + 2]];
      fi++;
    }

    totalVertBefore += vcgMesh.vn;

    PreprocessMesh(vcgMesh);

    TriEdgeCollapseQuadricTexParameter qparams;
    qparams.SetDefaultParams();
    qparams.NormalCheck = true;
    qparams.QualityQuadric = true;

    Decimate(vcgMesh, qparams, 100.0f);

    totalVertAfter += vcgMesh.vn;


    int mask = vcg::tri::io::Mask::IOM_VERTNORMAL | vcg::tri::io::Mask::IOM_VERTCOORD
        | vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
    vcg::tri::io::ExporterOBJ<MyMesh>::Save(vcgMesh,
                                            ("output/" + file_name + "_" + std::to_string(z++)
                                                + ".obj").c_str(),
                                            mask,
                                            objMesh.material_name.substr(0,
                                                                          objMesh.material_name.find(
                                                                              '.')).c_str(),
                                            objMesh.mtllib_name.c_str());
  }

  std::cout << "Before: " << totalVertBefore << endl;
  std::cout << "After: " << totalVertAfter << endl;
}
