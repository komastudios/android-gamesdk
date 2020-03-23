/*
 * Copyright 2019 The Android Open Source Project
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

#include "Storage.hpp"
#include <ancer/util/GLHelpers.hpp>

using ancer::glh::CheckGlError;

namespace marching_cubes {

void Vertex::BindVertexAttributes() {
  glVertexAttribPointer(
      static_cast<GLuint>(Vertex::AttributeLayout::Pos),
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex),
      (const GLvoid*) offsetof(Vertex, pos));
  glEnableVertexAttribArray(static_cast<GLuint>(Vertex::AttributeLayout::Pos));

  glVertexAttribPointer(
      static_cast<GLuint>(Vertex::AttributeLayout::Color),
      4,
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex),
      (const GLvoid*) offsetof(Vertex, color));
  glEnableVertexAttribArray(static_cast<GLuint>(Vertex::AttributeLayout::Color));

  glVertexAttribPointer(
      static_cast<GLuint>(Vertex::AttributeLayout::Normal),
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex),
      (const GLvoid*) offsetof(Vertex, normal));
  glEnableVertexAttribArray(static_cast<GLuint>(Vertex::AttributeLayout::Normal));
}

//------------------------------------------------------------------------------

VertexStorage::~VertexStorage() {
  if (_vao > 0)
    glDeleteVertexArrays(1, &_vao);
  if (_vertex_vbo_id > 0)
    glDeleteBuffers(1, &_vertex_vbo_id);
}

void VertexStorage::Draw() const {
  if (_vao > 0) {
    CheckGlError("VertexStorage::draw enter");
    glBindVertexArray(_vao);
    glDrawArrays(_mode, 0, static_cast<GLsizei>(_num_vertices));
    glBindVertexArray(0);
    CheckGlError("VertexStorage::draw exit");
  }
}

void VertexStorage::Update(const std::vector<Vertex>& vertices) {
  if (_vao == 0) {
    glGenVertexArrays(1, &_vao);
  }
  glBindVertexArray(_vao);
  UpdateVertices(vertices);
  glBindVertexArray(0);
}

void VertexStorage::UpdateVertices(const std::vector<Vertex>& vertices) {
  CheckGlError("VertexStorage::_updateVertices enter");
  if (vertices.size() > _vertex_storage_size) {
    _vertex_storage_size =
        static_cast<std::size_t>(vertices.size() * _growth_factor);
    _num_vertices = vertices.size();

    if (_vertex_vbo_id > 0) {
      glDeleteBuffers(1, &_vertex_vbo_id);
      _vertex_vbo_id = 0;
    }

    glGenBuffers(1, &_vertex_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_vbo_id);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(Vertex) * _vertex_storage_size,
        nullptr,
        GL_STREAM_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(Vertex) * _num_vertices,
                    vertices.data());

    Vertex::BindVertexAttributes();

  } else {
    // GPU storage suffices, just copy data over
    _num_vertices = vertices.size();
    if (_num_vertices > 0) {
      glBindBuffer(GL_ARRAY_BUFFER, _vertex_vbo_id);
      glBufferSubData(
          GL_ARRAY_BUFFER,
          0,
          sizeof(Vertex) * _num_vertices,
          vertices.data());
    }
  }
  CheckGlError("VertexStorage::_updateVertices exit");
}

} // namespace marching_cubes


