/*
 * Copyright 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _GL_TEX_IMAGE_2D_RENDERER_HPP
#define _GL_TEX_IMAGE_2D_RENDERER_HPP

#include <GLES3/gl32.h>
#include <glm/common.hpp>

namespace ancer {

/**
 * Useful GLES3 renderer for textures like .png, .jpg or even compressed files (.astc, .pkm,
 * .basis).
 *
 * Whenever there's a need to render a texture, pic or any other image on screen, this renderer is
 * ready to use.
 */
class GLTexImage2DRenderer {
 public:
  /**
   * Constructor. Receives the screen coordinates that define the 2D rectangle where render happens.
   */
  GLTexImage2DRenderer(const int left, const int top, const int width, const int height);

  virtual ~GLTexImage2DRenderer();

  /**
   * To be called from BaseGLES3Operation::Draw() overridden functions.
   */
  void Draw();

//--------------------------------------------------------------------------------------------------

 private:
  void CreateGLObjects();

  void TeardownGL();

//--------------------------------------------------------------------------------------------------

 private:
  GLuint _vertex_buffer_object_id;
  GLuint _index_buffer_object_id;
  GLuint _vertex_buffer_object_state;

//--------------------------------------------------------------------------------------------------

 private:
  int _width;
  int _height;

  struct Vertex {
    glm::vec2 position;
    glm::vec2 texture_coordinates;
  };

  Vertex _vertices[4];
  GLushort _indices[6];
};

}

#endif // _GL_TEX_IMAGE_2D_RENDERER_HPP