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
#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"
#include <unordered_map>
#include <fstream>

#define NUM_VERTEX 20000

// The class prototypes.
class MyVertex;
class MyEdge;
class MyFace;

struct MyUsedTypes: public vcg::UsedTypes<vcg::Use<MyVertex>::AsVertexType, vcg::Use<MyEdge>::AsEdgeType, vcg::Use<MyFace>::AsFaceType>{};

class MyVertex  : public vcg::Vertex< MyUsedTypes,
  vcg::vertex::VFAdj,
  vcg::vertex::Coord3f,
  vcg::vertex::Normal3f,
  vcg::vertex::TexCoord2f,
  vcg::vertex::Mark,
  vcg::vertex::BitFlags  >{ };

class MyEdge : public vcg::Edge< MyUsedTypes> {};

typedef BasicVertexPair<MyVertex> VertexPair;

class MyFace    : public vcg::Face< MyUsedTypes,
  vcg::face::VFAdj,
  vcg::face::FFAdj,
  vcg::face::VertexRef,
  vcg::face::WedgeTexCoord2f,
  vcg::face::Normal3f,
  vcg::face::BitFlags> {};

// the main mesh class
class MyMesh    : public vcg::tri::TriMesh<std::vector<MyVertex>, std::vector<MyFace> > {};

class MyTriEdgeCollapseQTex: public TriEdgeCollapseQuadricTex< MyMesh, VertexPair, MyTriEdgeCollapseQTex, QuadricTexHelper<MyMesh> > {
            public:
            typedef  TriEdgeCollapseQuadricTex< MyMesh,  VertexPair, MyTriEdgeCollapseQTex, QuadricTexHelper<MyMesh> > TECQ;
            inline MyTriEdgeCollapseQTex(  const VertexPair &p, int i,vcg::BaseParameterClass *pp) :TECQ(p,i,pp){}
};

struct MTL {
  glm::vec3 ambient_;
  glm::vec3 diffuse_;
  glm::vec3 specular_;
  float specular_exponent_;
  float bump_multiplier = 1.0f;

  std::string map_Ka_ = "";
  std::string map_Kd_ = "";
  std::string map_Ke_ = "";
  std::string map_Ks_ = "";
  std::string map_Ns_ = "";
  std::string map_Bump_ = "";
};

struct vert{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec3 bitangent;
  glm::vec2 texture;
};

struct OBJ {
  std::string name_;
  std::string material_name_;
  std::vector<vert> vertex_buffer_;
  std::vector<int> index_buffer_;
  std::unordered_map<glm::vec3, int> vert_to_index_;
};

int trueIndex(int idx, int size) {
  if (idx < 0) {
    return size + idx;
  } else {
    return idx - 1;
  }
}

void addVertex(glm::vec3 &currVert,
               OBJ &currOBJ,
               std::vector<glm::vec3> &position,
               std::vector<glm::vec3> &normal,
               std::vector<glm::vec2> &texCoord) {
  
  if (currOBJ.vert_to_index_.find(currVert) != currOBJ.vert_to_index_.end()) {
    int index = currOBJ.vert_to_index_[currVert];
    currOBJ.index_buffer_.push_back(index);
    return;
  }

  vert newVert{
    .position = position[trueIndex(currVert.x, position.size())],
    .normal = normal[trueIndex(currVert.z, normal.size())],
    .texture = texCoord[trueIndex(currVert.y, texCoord.size())]
  };

  currOBJ.index_buffer_.push_back(currOBJ.vert_to_index_.size());
  currOBJ.vert_to_index_[currVert] = currOBJ.vert_to_index_.size();
  currOBJ.vertex_buffer_.push_back(newVert);
}

void LoadMTL(const std::string &fileName,
             std::unordered_map<std::string, MTL> &mtllib) {

  std::ifstream data;
  data.open(fileName, std::ifstream::in);

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
      if (skip == "-"){
        data >> skip >> mtllib[currentMaterialName].bump_multiplier
             >> mtllib[currentMaterialName].map_Bump_;
      }
      else{
        data >> mtllib[currentMaterialName].map_Bump_;
      }
    }
  }
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

  while (data >> label) {
    if (label == "mtllib") {
      std::string mtllibName;
      data >> mtllibName;
      LoadMTL(mtllibName, mtllib);
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

      addVertex(vertex1, modelData.back(), position, normal, texCoord);
      addVertex(vertex2, modelData.back(), position, normal, texCoord);
      addVertex(vertex3, modelData.back(), position, normal, texCoord);
    }
  }
}


void MeshLoader(const std::string &file_path, const std::string &file_name)
{

    std::vector<OBJ> modelData;
    std::unordered_map<std::string, MTL> mtllib;
    LoadOBJ(file_path, mtllib, modelData);

    int z = 0;
    int totalVertBefore = 0;
    int totalVertAfter = 0;
    for (auto objMesh : modelData){
      MyMesh vcgMesh;
      MyMesh::VertexIterator vi = vcg::tri::Allocator<MyMesh>::AddVertices(vcgMesh, objMesh.vertex_buffer_.size());
      MyMesh::FaceIterator fi = vcg::tri::Allocator<MyMesh>::AddFaces(vcgMesh, objMesh.index_buffer_.size() / 3);

      float pX, pY, pZ, nX, nY, nZ, tX, tY;
      for (int x = 0; x < objMesh.vertex_buffer_.size(); x++){
        pX = objMesh.vertex_buffer_[x].position.x;
        pY = objMesh.vertex_buffer_[x].position.y;
        pZ = objMesh.vertex_buffer_[x].position.z;
        nX = objMesh.vertex_buffer_[x].normal.x;
        nY = objMesh.vertex_buffer_[x].normal.y;
        nZ = objMesh.vertex_buffer_[x].normal.z;
        tX = objMesh.vertex_buffer_[x].texture.x;
        tY = objMesh.vertex_buffer_[x].texture.y;

        vi->P()=MyMesh::CoordType ( pX, pY, pZ);
        vi->N()=MyMesh::CoordType ( nX, nY, nZ);
        vi->T()=MyMesh::VertexType::TexCoordType( tX, tY);
        vi++;
      }

      for (int x = 0; x < objMesh.index_buffer_.size(); x += 3){
        fi->V(0)=&vcgMesh.vert[objMesh.index_buffer_[x]];
        fi->V(1)=&vcgMesh.vert[objMesh.index_buffer_[x+1]];
        fi->V(2)=&vcgMesh.vert[objMesh.index_buffer_[x+2]];
        fi++;
      }

      totalVertBefore += vcgMesh.vn;

      vcg::tri::UpdateBounding<MyMesh>::Box(vcgMesh);
      vcg::tri::UpdateTexture<MyMesh>::WedgeTexFromVertexTex(vcgMesh);
      vcg::tri::UpdateNormal<MyMesh>::PerFaceFromCurrentVertexNormal(vcgMesh);
      vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(vcgMesh);

      TriEdgeCollapseQuadricTexParameter qparams;
      qparams.SetDefaultParams();
      qparams.NormalCheck = true;
      qparams.QualityQuadric = true;

      vcg::math::Quadric<double> QZero;
      QZero.SetZero();
      QuadricTexHelper<MyMesh>::QuadricTemp TD3(vcgMesh.vert,QZero);
      QuadricTexHelper<MyMesh>::TDp3()=&TD3;
      std::vector<std::pair<vcg::TexCoord2<float>,vcg::Quadric5<double> > > qv;
      QuadricTexHelper<MyMesh>::Quadric5Temp TD(vcgMesh.vert,qv);
      QuadricTexHelper<MyMesh>::TDp()=&TD;

      vcg::LocalOptimization<MyMesh> DeciSession(vcgMesh, &qparams);
      DeciSession.Init<MyTriEdgeCollapseQTex>();
      DeciSession.SetTargetMetric(100.0f);
      DeciSession.DoOptimization();
      DeciSession.Finalize<MyTriEdgeCollapseQTex>();

      totalVertAfter += vcgMesh.vn;

      vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalized(vcgMesh);

      int mask = vcg::tri::io::Mask::IOM_VERTNORMAL | vcg::tri::io::Mask::IOM_VERTCOORD | vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
      vcg::tri::io::ExporterOBJ<MyMesh>::Save(vcgMesh, ("output/" + file_name + "_" + std::to_string(z++) + ".obj").c_str(), mask, objMesh.material_name_.substr(0, objMesh.material_name_.find('.')).c_str(), "starship_command_center_triangle");
    }

    std::cout << "Before: " << totalVertBefore << endl;
    std::cout << "After: " << totalVertAfter << endl;
}
