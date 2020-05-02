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

#include "GLTexImage2DRenderer.hpp"

#include <ancer/BaseOperation.hpp>
#include <ancer/System.Gpu.hpp>
#include <ancer/util/GLHelpers.hpp>

#define SIZEOF_PRIMITIVE_ARRAY(a)    (sizeof(a)/sizeof(a[0]))

using namespace ancer;

namespace {
/**
 * The two vertex attributes relevant here: position and texture coordinates.
 */
enum class VertexAttribute : GLuint {
  Position,
  TextureCoordinates
};
}

//==================================================================================================

GLTexImage2DRenderer::GLTexImage2DRenderer(const int left,
                                           const int top,
                                           const int width,
                                           const int height) :
    _width(width), _height(height),
    _vertex_buffer_object_id(0), _index_buffer_object_id(0), _vertex_buffer_object_state(0),
    _vertices{{{left, top}, {0, 0}},
              {{left + width, top}, {1, 0}},
              {{left, top + height}, {0, 1}},
              {{left + width, top + height}, {1, 1}}
    },
    _indices{0, 1, 2, 1, 3, 2} {
  CreateGLObjects();
}

GLTexImage2DRenderer::~GLTexImage2DRenderer() {
  TeardownGL();
}

void GLTexImage2DRenderer::Draw() {
  ANCER_SCOPED_TRACE("GLTexImage2DRenderer::Draw");

  glBindVertexArray(_vertex_buffer_object_state);
  glDrawElements(GL_TRIANGLES, SIZEOF_PRIMITIVE_ARRAY(_indices), GL_UNSIGNED_SHORT, nullptr);
}

void GLTexImage2DRenderer::CreateGLObjects() {
  // Initialize vertex buffers
  ANCER_SCOPED_TRACE("GLTexImage2DRenderer::Setup");

  glGenBuffers(1, &_vertex_buffer_object_id);
  glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &_index_buffer_object_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer_object_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), _indices, GL_STATIC_DRAW);

  glh::CheckGlError("building buffers");

  glGenVertexArrays(1, &_vertex_buffer_object_state);
  glBindVertexArray(_vertex_buffer_object_state);

  glh::CheckGlError("binding vertex array");

  glVertexAttribPointer(
      static_cast<GLuint>(VertexAttribute::Position), 2, GL_FLOAT, GL_FALSE,
      sizeof(Vertex),
      (const GLvoid *) offsetof(Vertex, position));

  glVertexAttribPointer(
      static_cast<GLuint>(VertexAttribute::TextureCoordinates), 2, GL_FLOAT,
      GL_TRUE,
      sizeof(Vertex),
      (const GLvoid *) offsetof(Vertex, texture_coordinates));

  glh::CheckGlError("setting attrib pointers");

  glEnableVertexAttribArray(static_cast<GLuint>(VertexAttribute::Position));
  glEnableVertexAttribArray(static_cast<GLuint>(VertexAttribute::TextureCoordinates));

  glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer_object_id);

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
}

void GLTexImage2DRenderer::TeardownGL() {
  if (_vertex_buffer_object_state) {
    glDeleteVertexArrays(1, &_vertex_buffer_object_state);
  }

  if (_vertex_buffer_object_id) {
    glDeleteBuffers(1, &_vertex_buffer_object_id);
  }

  if (_index_buffer_object_id) {
    glDeleteBuffers(1, &_index_buffer_object_id);
  }
}
