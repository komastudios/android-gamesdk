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

#ifndef storage_hpp
#define storage_hpp

#include <GLES3/gl32.h>

#include <glm/glm.hpp>
using namespace glm;

#include <vector>
#include <ancer/util/GLHelpers.hpp>

namespace marching_cubes {

/**
 * Simple vertex type with a 3-component position and 4 component color
 * used by LineSegmentBuffer and skydome rendering
 */
struct VertexP3C4 {
  glm::vec3 pos;
  glm::vec4 color{1};

  enum class AttributeLayout : GLuint {
    Pos = 0,
    Color = 1
  };

  bool operator==(const VertexP3C4 &other) const {
    return pos == other.pos && color == other.color;
  }

  static void bindVertexAttributes();
};

/**
 * GPU storage templated on vertex type. Expects vertex to have static method
 * static void VertexType::bindVertexAttributes(); which sets/enables the right
 * vertex attrib pointers
 */
template<class VertexType>
class VertexStorage {
 private:
  GLenum _mode;
  GLuint _vertexVboId = 0;
  GLuint _vao = 0;
  std::size_t _numVertices = 0;
  std::size_t _vertexStorageSize = 0;
  float _growthFactor;

 public:
  VertexStorage(GLenum mode, float growthFactor = 1.5F)
      : _mode(mode), _growthFactor(growthFactor) {
  }

  VertexStorage(const std::vector<VertexType> &vertices, GLenum mode, float growthFactor = 1.5F)
      : _mode(mode), _growthFactor(growthFactor) {
    update(vertices);
  }

  ~VertexStorage() {
    if (_vao > 0)
      glDeleteVertexArrays(1, &_vao);
    if (_vertexVboId > 0)
      glDeleteBuffers(1, &_vertexVboId);
  }

  void draw() const {
    if (_vao > 0) {
      ancer::glh::CheckGlError("VertexStorage::draw enter");
      glBindVertexArray(_vao);
      glDrawArrays(_mode, 0, static_cast<GLsizei>(_numVertices));
      glBindVertexArray(0);
      ancer::glh::CheckGlError("VertexStorage::draw exit");
    }
  }

  std::size_t numVertices() const { return _numVertices; }
  std::size_t vertexStoreSize() const { return _vertexStorageSize; }

  void update(const std::vector<VertexType> &vertices) {
    if (_vao == 0) {
      glGenVertexArrays(1, &_vao);
    }
    glBindVertexArray(_vao);
    _updateVertices(vertices);
    glBindVertexArray(0);
  }

 private:
  void _updateVertices(const std::vector<VertexType> &vertices) {
    ancer::glh::CheckGlError("VertexStorage::_updateVertices enter");
    if (vertices.size() > _vertexStorageSize) {
      _vertexStorageSize = static_cast<std::size_t>(vertices.size() * _growthFactor);
      _numVertices = vertices.size();

      if (_vertexVboId > 0) {
        glDeleteBuffers(1, &_vertexVboId);
        _vertexVboId = 0;
      }

      glGenBuffers(1, &_vertexVboId);
      glBindBuffer(GL_ARRAY_BUFFER, _vertexVboId);

      glBufferData(
          GL_ARRAY_BUFFER,
          sizeof(VertexType) * _vertexStorageSize,
          nullptr,
          GL_STREAM_DRAW);

      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexType) * _numVertices, vertices.data());

      VertexType::bindVertexAttributes();

    } else {
      // GPU storage suffices, just copy data over
      _numVertices = vertices.size();
      if (_numVertices > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertexVboId);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            sizeof(VertexType) * _numVertices,
            vertices.data());
      }
    }
    ancer::glh::CheckGlError("VertexStorage::_updateVertices exit");
  }
};

//------------------------------------------------------------------------------

template<class VertexType>
struct Triangle {
  VertexType a, b, c;

  Triangle() {
  }

  Triangle(const VertexType &a, const VertexType &b, const VertexType &c)
      : a(a), b(b), c(c) {
  }

  Triangle(const Triangle<VertexType> &other) {
    a = other.a;
    b = other.b;
    c = other.c;
  }

  Triangle &operator=(const Triangle<VertexType> &other) {
    a = other.a;
    b = other.b;
    c = other.c;
    return *this;
  }
};

/*
 Consumes triangles and maintaining a non-indexed vertex storage
 */
template<class VertexType>
class TriangleConsumer {
 private:
  std::vector<VertexType> _vertices;
  VertexStorage<VertexType> _gpuStorage{GL_TRIANGLES};
  size_t _numTriangles = 0;

 public:
  using vertex_type = VertexType;

  TriangleConsumer() = default;
  virtual ~TriangleConsumer() = default;

  void start() {
    _vertices.clear();
    _numTriangles = 0;
  }

  void addTriangle(const Triangle<VertexType> &t) {
    _vertices.push_back(t.a);
    _vertices.push_back(t.b);
    _vertices.push_back(t.c);
    _numTriangles++;
  }

  void finish() {
    _gpuStorage.update(_vertices);
  }

  size_t numTriangles() const { return _numTriangles; }

  void draw() const {
    _gpuStorage.draw();
  }

  void clear() {
    _vertices.clear();
    _gpuStorage.update({});
    _numTriangles = 0;
  }

  const auto &storage() const { return _gpuStorage; }
  auto &storage() { return _gpuStorage; }
};

//------------------------------------------------------------------------------

class LineSegmentBuffer {
 public:
  using vertex_type = VertexP3C4;

  LineSegmentBuffer() = default;
  LineSegmentBuffer(const LineSegmentBuffer &) = delete;
  LineSegmentBuffer(const LineSegmentBuffer &&) = delete;
  ~LineSegmentBuffer() = default;

  void clear() {
    _vertices.clear();
    _dirty = true;
  }

  void add(const vertex_type &a, const vertex_type &b) {
    _vertices.push_back(a);
    _vertices.push_back(b);
    _dirty = true;
  }

  template<typename T, glm::precision P>
  inline void add(const ancer::glh::tAABB<T, P> &bounds, const glm::vec4 &color) {
    auto corners = bounds.Corners();

    // trace bottom
    add(vertex_type{corners[0], color}, vertex_type{corners[1], color});
    add(vertex_type{corners[1], color}, vertex_type{corners[2], color});
    add(vertex_type{corners[2], color}, vertex_type{corners[3], color});
    add(vertex_type{corners[3], color}, vertex_type{corners[0], color});

    // trace top
    add(vertex_type{corners[4], color}, vertex_type{corners[5], color});
    add(vertex_type{corners[5], color}, vertex_type{corners[6], color});
    add(vertex_type{corners[6], color}, vertex_type{corners[7], color});
    add(vertex_type{corners[7], color}, vertex_type{corners[4], color});

    // add bars connecting bottom to top
    add(vertex_type{corners[0], color}, vertex_type{corners[4], color});
    add(vertex_type{corners[1], color}, vertex_type{corners[5], color});
    add(vertex_type{corners[2], color}, vertex_type{corners[6], color});
    add(vertex_type{corners[3], color}, vertex_type{corners[7], color});
  }

  void addAxis(const glm::mat4 &basis, float size) {
    const auto X = glm::vec3{basis[0][0], basis[1][0], basis[2][0]};
    const auto Y = glm::vec3{basis[0][1], basis[1][1], basis[2][1]};
    const auto Z = glm::vec3{basis[0][2], basis[1][2], basis[2][2]};
    const auto pos = glm::vec3{basis[3][0], basis[3][1], basis[3][2]};
    const auto red = glm::vec4{1, 0, 0, 1};
    const auto green = glm::vec4{0, 1, 0, 1};
    const auto blue = glm::vec4{0, 0, 1, 1};
    add(vertex_type{pos, red}, vertex_type{pos + X * size, red});
    add(vertex_type{pos, green}, vertex_type{pos + Y * size, green});
    add(vertex_type{pos, blue}, vertex_type{pos + Z * size, blue});
  }

  void addMarker(const glm::vec3 &pos, float size, const glm::vec4 &color) {
    add(vertex_type{pos, color}, vertex_type{pos + glm::vec3(-size, 0, 0), color});
    add(vertex_type{pos, color}, vertex_type{pos + glm::vec3(+size, 0, 0), color});
    add(vertex_type{pos, color}, vertex_type{pos + glm::vec3(0, -size, 0), color});
    add(vertex_type{pos, color}, vertex_type{pos + glm::vec3(0, +size, 0), color});
    add(vertex_type{pos, color}, vertex_type{pos + glm::vec3(0, 0, -size), color});
    add(vertex_type{pos, color}, vertex_type{pos + glm::vec3(0, 0, +size), color});
  }

  void draw() {
    if (_dirty) {
      _gpuStorage.update(_vertices);
      _dirty = false;
    }
    _gpuStorage.draw();
  }

 private:
  bool _dirty = false;
  std::vector<vertex_type> _vertices;
  VertexStorage<vertex_type> _gpuStorage{GL_LINES};
};

} // namespace marching_cubes

#endif /* storage_hpp */
