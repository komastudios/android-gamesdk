//
// Created by Andy Yu on 2019-11-04.
//

#ifndef BENDER_BASE_SHAPE_H
#define BENDER_BASE_SHAPE_H

#include <vector>

#import "vulkan_wrapper.h"
#import "glm/glm.hpp"
#import "bender_kit.h"
#import "geometry.h"
#import "shader_state.h"

class Shape{
public:
  Shape(int n);
  ~Shape();
  std::vector<float> const getVertexData();
  std::vector<u_int16_t> const getIndexData();
private:
  std::vector<float> vertex_data_;
  std::vector<u_int16_t> index_data_;
};

#endif //BENDER_BASE_SHAPE_H
