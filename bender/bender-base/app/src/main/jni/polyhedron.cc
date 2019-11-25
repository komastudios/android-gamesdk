//
// Created by Andy Yu on 2019-11-04.
//

#include "polyhedron.h"

namespace  {
  const std::vector<float> vertex_tetrahedron = {
      0.0f, 0.35682208977304947f, 0.9341723589627311f, -2.802718115404023e-14f, 0.3568220897730832f, 0.9341723589627183f, 0.25f, 0.61614f,
      -0.35682208977304947f, -0.9341723589627311f, 0.0f, -0.35682208977308316f, -0.9341723589627182f, -2.8075255221199808e-14f, 0.5f, 0.11614f,
      0.9341723589627311f, 0.0f, -0.35682208977304947f, 0.9341723589627182f, 2.793103301972104e-14f, -0.35682208977308316f, 0.94193f, 0.5f,
      -0.5773502691896257f, 0.5773502691896257f, -0.5773502691896257f, -0.5773502691896257f, 0.5773502691896256f, -0.5773502691896258f, 0.625f, 0.695913f};
  const std::vector<uint16_t> index_tetrahedron = {0, 1, 3, 0, 2, 1, 0, 3, 2, 3, 1, 2};

  const std::vector<float> vertex_cube = {1.0f, -1.0f, -1.0f, 0.3333f, -0.3333f, -0.3333f, 0.875f, 0.304087f,
                                          1.0f, 1.0f, -1.0f, 0.3333f, 0.3333f, -0.3333f, 0.875f, 0.695913f,
                                          -1.0f, 1.0f, -1.0f, -0.3333f, 0.3333f, -0.3333f, 0.625f, 0.695913f,
                                          -1.0f, -1.0f, -1.0f, -0.3333f, -0.3333f, -0.3333f, 0.625f, 0.304087f,
                                          1.0f, -1.0f, 1.0f, 0.3333f, -0.3333f, 0.3333f, 0.125f, 0.304087f,
                                          1.0f, 1.0f, 1.0f, 0.3333f, 0.3333f, 0.3333f, 0.125f, 0.695913f,
                                          -1.0f, 1.0f, 1.0f, -0.3333f, 0.3333f, 0.3333f, 0.375f, 0.695913f,
                                          -1.0f, -1.0f, 1.0f, -0.3333f, -0.3333f, 0.3333f, 0.375f, 0.304087f};

  const std::vector<uint16_t> index_cube = {0, 1, 2, 0, 2, 3, 5, 1, 0, 5, 0, 4,
                                             5, 4, 7, 5, 7, 6, 7, 3, 2, 7, 2, 6,
                                             6, 2, 1, 6, 1, 5, 4, 0, 3, 4, 3, 7};

  const std::vector<float> vertex_octahedron = {-0.30901699437491614f, -0.49999999999998185f, 0.8090169943749705f, -0.30901699437491614f, -0.49999999999998185f, 0.8090169943749705f, 0.30807f, 0.333333f,
                                                0.8090169943749705f, 0.30901699437491614f, 0.49999999999998185f, 0.3090169943749637f, 0.4999999999999638f, -0.8090169943749637f, 0.0881041f, 0.6f,
                                                -0.49999999999998185f, 0.8090169943749705f, 0.30901699437491614f, -0.49999999999998185f, 0.8090169943749705f, 0.30901699437491614f, 0.411896f, 0.8f,
                                                0.49999999999998185f, -0.8090169943749705f, -0.30901699437491614f, 0.49999999999998185f, -0.8090169943749705f, -0.30901699437491614f, 0.911896f, 0.2f,
                                                -0.8090169943749705f, -0.30901699437491614f, -0.49999999999998185f, -0.8090169943749705f, -0.30901699437491614f, -0.49999999999998185f, 0.588104f, 0.4f,
                                                0.30901699437491614f, 0.49999999999998185f, -0.8090169943749705f, 0.3090169943749637f, 0.4999999999999638f, -0.8090169943749637f, 0.80807f, 0.666667f};
  const std::vector<uint16_t> index_octahedron = {0, 2, 1, 0, 1, 3, 0, 3, 4, 0, 4, 2,
                                                   5, 2, 4, 5, 1, 2, 5, 3, 1, 5, 4, 3};

  const std::vector<float> vertex_dodecahedron = {0.0f, 0.35682208977304947f, 0.9341723589627311f, -0.10767686505541395f, 0.5226744823848766f, 0.845705077550998f, 0.25f, 0.61614f,
                                                  0.0f, -0.35682208977304947f, 0.9341723589627311f, -2.8003144120460432e-14f, -0.35682208977311336f, 0.9341723589627067f, 0.25f, 0.38386f,
                                                  0.5773502691896257f, 0.5773502691896257f, 0.5773502691896257f, 0.5843700494711029f, 0.33128830477428234f, 0.7407831696258513f, 0.125f, 0.695913f,
                                                  -0.5773502691896257f, 0.5773502691896257f, 0.5773502691896257f, -0.57735026918964f, 0.5773502691896186f, 0.5773502691896187f, 0.375f, 0.695913f,
                                                  -0.5773502691896257f, -0.5773502691896257f, 0.5773502691896257f, -0.5773502691896187f, -0.57735026918964f, 0.5773502691896187f, 0.375f, 0.304087f,
                                                  0.5773502691896257f, -0.5773502691896257f, 0.5773502691896257f, 0.6439718962563654f, -0.5859049865947376f, 0.49195075314039427f, 0.125f, 0.304087f,
                                                  0.9341723589627311f, 0.0f, 0.35682208977304947f, 0.9455306020516977f, 0.25308174469686456f, 0.20474743242577015f, 0.0580699f, 0.5f,
                                                  -0.9341723589627311f, 0.0f, 0.35682208977304947f, -0.9129824929323077f, -0.08232358003197665f, 0.39960705170182803f, 0.44193f, 0.5f,
                                                  0.35682208977304947f, 0.9341723589627311f, 0.0f, 0.30404228623186974f, 0.948014182488277f, -0.09395423345428182f, 0.0f, 0.88386f,
                                                  -0.35682208977304947f, 0.9341723589627311f, 0.0f, -0.5226744823848863f, 0.8457050775509988f, 0.10767686505536042f, 0.5f, 0.88386f,
                                                  -0.35682208977304947f, -0.9341723589627311f, 0.0f, -0.3040422862318703f, -0.9480141824882784f, 0.09395423345426378f, 0.5f, 0.11614f,
                                                  0.35682208977304947f, -0.9341723589627311f, 0.0f, 0.2628655560595826f, -0.9510565162951482f, -0.16245984811645842f, 0.0f, 0.11614f,
                                                  0.9341723589627311f, 0.0f, -0.35682208977304947f, 0.8457050775510019f, -0.10767686505537773f, -0.5226744823848777f, 0.94193f, 0.5f,
                                                  -0.9341723589627311f, 0.0f, -0.35682208977304947f, -0.8457050775509988f, 0.10767686505536042f, -0.5226744823848862f, 0.55807f, 0.5f,
                                                  0.5773502691896257f, 0.5773502691896257f, -0.5773502691896257f, 0.688190960235569f, 0.5877852522924841f, -0.4253254041760335f, 0.875f, 0.695913f,
                                                  -0.5773502691896257f, 0.5773502691896257f, -0.5773502691896257f, -0.42532540417603354f, 0.688190960235569f, -0.5877852522924841f, 0.625f, 0.695913f,
                                                  -0.5773502691896257f, -0.5773502691896257f, -0.5773502691896257f, -0.688190960235569f, -0.5877852522924841f, -0.4253254041760335f, 0.625f, 0.304087f,
                                                  0.5773502691896257f, -0.5773502691896257f, -0.5773502691896257f, 0.688190960235578f, -0.587785252292488f, -0.42532540417601356f, 0.875f, 0.304087f,
                                                  0.0f, 0.35682208977304947f, -0.9341723589627311f, -1.0720516976587767e-14f, 0.3568220897730853f, -0.9341723589627173f, 0.75f, 0.61614f,
                                                  0.0f, -0.35682208977304947f, -0.9341723589627311f, 1.3681685080609877e-14f, -0.4560634293478342f, -0.8899472728265945f, 0.75f, 0.38386f};
  const std::vector<uint16_t> index_dodecahedron = {16, 19, 13, 19, 18, 13, 18, 15, 13, 14, 18, 12, 18, 19, 12, 19, 17,
                                                     12, 17, 19, 11, 11, 19, 10, 19, 16, 10, 9, 15, 8, 15, 18, 8, 18, 14,
                                                     8, 6, 12, 5, 12, 17, 5, 17, 11, 5, 13, 7, 16, 16, 7, 10, 10, 7, 4,
                                                     15, 9, 13, 13, 9, 7, 7, 9, 3, 14, 12, 6, 8, 14, 6, 8, 6, 2, 11, 10,
                                                     5, 5, 10, 1, 10, 4, 1, 3, 9, 0, 9, 8, 0, 8, 2, 0, 4, 7, 1, 1, 7, 0,
                                                     7, 3, 0, 6, 5, 2, 5, 1, 2, 2, 1, 0};

  const std::vector<float> vertex_icosahedron = {0.525731112119109f, 0.0f, 0.8506508083520552f, 0.5257311121191336f, 0.0f, 0.8506508083520398f, 0.161896f, 0.5f,
                                                 -0.525731112119109f, 0.0f, 0.8506508083520552f, -0.5257311121191336f, 0.0f, 0.8506508083520398f, 0.338104f, 0.5f,
                                                 0.0f, 0.8506508083520552f, 0.525731112119109f, 0.0f, 0.8506508083520398f, 0.5257311121191336f, 0.25f, 0.823792f,
                                                 0.0f, -0.8506508083520552f, 0.525731112119109f, 0.0f, -0.8506508083520398f, 0.5257311121191336f, 0.25f, 0.176208f,
                                                 0.8506508083520552f, 0.525731112119109f, 0.0f, 0.8506508083520398f, 0.5257311121191336f, 0.0f, 0.0f, 0.676208f,
                                                 -0.8506508083520552f, 0.525731112119109f, 0.0f, -0.8506508083520398f, 0.5257311121191336f, 1.4591969635911295e-17f, 0.5f, 0.676208f,
                                                 -0.8506508083520552f, -0.525731112119109f, 0.0f, -0.8506508083520398f, -0.5257311121191336f, 0.0f, 0.5f, 0.323792f,
                                                 0.8506508083520552f, -0.525731112119109f, 0.0f, 0.8506508083520398f, -0.5257311121191336f, 0.0f, 0.0f, 0.323792f,
                                                 0.0f, 0.8506508083520552f, -0.525731112119109f, 0.0f, 0.8506508083520398f, -0.5257311121191336f, 0.75f, 0.823792f,
                                                 0.0f, -0.8506508083520552f, -0.525731112119109f, 0.0f, -0.8506508083520398f, -0.5257311121191336f, 0.75f, 0.176208f,
                                                 0.525731112119109f, 0.0f, -0.8506508083520552f, 0.5257311121191336f, 0.0f, -0.8506508083520398f, 0.838104f, 0.5f,
                                                 -0.525731112119109f, 0.0f, -0.8506508083520552f, -0.5257311121191336f, 0.0f, -0.8506508083520398f, 0.661896f, 0.5f};
  const std::vector<uint16_t> index_icosahedron = {6, 9, 11, 5, 6, 11, 8, 5, 11, 10, 8, 11, 9, 10, 11, 10, 9, 7,
                                                    7, 9, 3, 9, 6, 3, 3, 6, 1, 6, 5, 1, 1, 5, 2, 5, 8, 2, 2, 8, 4,
                                                    8, 10, 4, 4, 10, 7, 2, 4, 0, 4, 7, 0, 7, 3, 0, 1, 2, 0, 3, 1, 0};
}

bool populatePolyhedron(std::vector<float>& vertex_data, std::vector<uint16_t>& index_data, int faces) {
  if (faces == 4) {
    vertex_data.resize(vertex_tetrahedron.size());
    index_data.resize(index_tetrahedron.size());
    memcpy((void *) &vertex_data, (const void *) &vertex_tetrahedron, sizeof(vertex_tetrahedron));
    memcpy((void *) &index_data, (const void *) &index_tetrahedron, sizeof(index_tetrahedron));
  } else if (faces == 6) {
    vertex_data.resize(vertex_cube.size());
    index_data.resize(index_cube.size());
    memcpy((void *) &vertex_data, (const void *) &vertex_cube, sizeof(vertex_cube));
    memcpy((void *) &index_data, (const void *) &index_cube, sizeof(index_cube));
  } else if (faces == 8) {
    vertex_data.resize(vertex_octahedron.size());
    index_data.resize(index_octahedron.size());
    memcpy((void *) &vertex_data, (const void *) &vertex_octahedron, sizeof(vertex_octahedron));
    memcpy((void *) &index_data, (const void *) &index_octahedron, sizeof(index_octahedron));
  } else if (faces == 12) {
    vertex_data.resize(vertex_dodecahedron.size());
    index_data.resize(index_dodecahedron.size());
    memcpy((void *) &vertex_data, (const void *) &vertex_dodecahedron, sizeof(vertex_dodecahedron));
    memcpy((void *) &index_data, (const void *) &index_dodecahedron, sizeof(index_dodecahedron));
  } else if (faces == 20) {
    vertex_data.resize(vertex_icosahedron.size());
    index_data.resize(index_icosahedron.size());
    memcpy((void *) &vertex_data, (const void *) &vertex_icosahedron, sizeof(vertex_icosahedron));
    memcpy((void *) &index_data, (const void *) &index_icosahedron, sizeof(index_icosahedron));
  } else {
    return false;
  }
  return true;
}

Mesh* createPolyhedron(Renderer &renderer, Material &material, int faces) {
  std::vector<float> *vertex_data = new std::vector<float>();
  std::vector<uint16_t> *index_data = new std::vector<uint16_t>();

  if (populatePolyhedron(*vertex_data, *index_data, faces) == false) {
    return nullptr;
  }

  return new Mesh(renderer, material, *vertex_data, *index_data);
}

void swapPolyhedron(Mesh& mesh, int faces) {
  std::vector<float> *vertex_data = new std::vector<float>();
  std::vector<uint16_t> *index_data = new std::vector<uint16_t>();

  if (populatePolyhedron(*vertex_data, *index_data, faces) == false) {
    return;
  }

  mesh.swapGeometry(*vertex_data, *index_data);
}
