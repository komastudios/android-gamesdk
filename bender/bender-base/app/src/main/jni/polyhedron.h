//
// Created by Andy Yu on 2019-11-04.
//

#ifndef BENDER_BASE_SHAPE_H
#define BENDER_BASE_SHAPE_H

#include <vector>
#include <memory>

#import "vulkan_wrapper.h"
#import "bender_kit.h"
#import "mesh.h"
#import "shader_state.h"

bool populatePolyhedron(std::vector<MeshVertex> &vertex_data, std::vector<uint16_t> &index_data, int faces);
void populateTetrahedron(std::vector<MeshVertex> &vertex_data, std::vector<uint16_t> &index_data);
void populateCube(std::vector<MeshVertex> &vertex_data, std::vector<uint16_t> &index_data);
void populateOctahedron(std::vector<MeshVertex> &vertex_data, std::vector<uint16_t> &index_data);
void populateDodecahedron(std::vector<MeshVertex> &vertex_data, std::vector<uint16_t> &index_data);
void populateIcosahedron(std::vector<MeshVertex> &vertex_data, std::vector<uint16_t> &index_data);

extern std::array<int, 5> allowedPolyFaces;
extern std::vector<std::function<void(std::vector<MeshVertex>&, std::vector<uint16_t>&)>> polyhedronGenerators;

#endif //BENDER_BASE_SHAPE_H
