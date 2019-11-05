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


Mesh* createPolyhedron(BenderKit::Device *device,
                       std::shared_ptr<ShaderState>, int faces);

#endif //BENDER_BASE_SHAPE_H
