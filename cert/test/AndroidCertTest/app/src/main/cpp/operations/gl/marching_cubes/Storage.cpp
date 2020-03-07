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

void VertexP3C4::bindVertexAttributes() {
  glVertexAttribPointer(
      static_cast<GLuint>(AttributeLayout::Pos),
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(VertexP3C4),
      (const GLvoid *) offsetof(VertexP3C4, pos));
  glEnableVertexAttribArray(static_cast<GLuint>(AttributeLayout::Pos));

  glVertexAttribPointer(
      static_cast<GLuint>(AttributeLayout::Color),
      4,
      GL_FLOAT,
      GL_FALSE,
      sizeof(VertexP3C4),
      (const GLvoid *) offsetof(VertexP3C4, color));
  glEnableVertexAttribArray(static_cast<GLuint>(AttributeLayout::Color));
}

} // namespace marching_cubes


