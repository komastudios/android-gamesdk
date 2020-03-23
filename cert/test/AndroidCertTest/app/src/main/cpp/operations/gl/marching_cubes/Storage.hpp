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

/*
 * The Vertex primitive we'll be using, just position, color and normal
 */
struct Vertex {
  vec3 pos{0};
  vec4 color{1};
  vec3 normal { 0,1,0};

  enum class AttributeLayout : GLuint {
    Pos = 0,
    Color = 1,
    Normal = 2
  };

  bool operator==(const Vertex& other) const {
    return pos == other.pos && color == other.color && normal == other.normal;
  }

  static void BindVertexAttributes();
};

/*
 * A simple triangle composed of 3 vertices
 */
struct Triangle {
  Vertex a, b, c;

  Triangle() = default;

  Triangle(const Vertex& a, const Vertex& b, const Vertex& c)
      : a(a), b(b), c(c) {
  }

  Triangle(const Triangle& other) = default;
  Triangle& operator=(const Triangle& other) = default;
};

//------------------------------------------------------------------------------

/*
 * Growable GPU-backed storage for Vertex. Works like std::vector by
 * growing larger than the requested size to minimize number of allocations.
 */
class VertexStorage {
 private:
  GLenum _mode;
  GLuint _vertex_vbo_id = 0;
  GLuint _vao = 0;
  std::size_t _num_vertices = 0;
  std::size_t _vertex_storage_size = 0;
  float _growth_factor;

 public:
  /*
   * Set up a vertex storage to draw using mode (e.g., GL_TRIANGLES)
   * and to grow by growthFactor when the high-water mark is reached.
   */
  VertexStorage(GLenum mode, float growthFactor = 1.5F)
      : _mode(mode), _growth_factor(growthFactor) {
  }

  /*
   * Set up a vertex storage to draw using mode (e.g., GL_TRIANGLES)
   * and to grow by growthFactor when the high-water mark is reached.
   */
  VertexStorage(const std::vector<Vertex>& vertices,
                GLenum mode,
                float growthFactor = 1.5F)
      : _mode(mode), _growth_factor(growthFactor) {
    Update(vertices);
  }

  ~VertexStorage();

  void Draw() const;

  std::size_t GetNumVertices() const { return _num_vertices; }
  std::size_t GetVertexStoreSize() const { return _vertex_storage_size; }

  void Update(const std::vector<Vertex>& vertices);

 private:
  void UpdateVertices(const std::vector<Vertex>& vertices);
};

//------------------------------------------------------------------------------

/*
 * Base class for things which consume triangles.
 * In our marching cubes code, the marching cubes alg just generates a
 * big honking soup of triangles. We'll use an implementation of
 * ITriangleConsumer to receive those triangles and send them to the
 * GPU via VertexStorage
 */
class ITriangleConsumer {
 public:
  ITriangleConsumer() = default;
  virtual ~ITriangleConsumer() = default;

  /*
   Signal that a batch of triangles will begin. Clears internal storage.
   */
  virtual void Start() = 0;

  /*
   Add a triangle to storage
   */
  virtual void AddTriangle(const Triangle& t) = 0;

  /*
   Signal that batch addition has completed; submits new triangles to GPU
   */
  virtual void Finish() = 0;

  /*
   Draw the internal storage of triangles
   */
  virtual void Draw() const = 0;
};

//------------------------------------------------------------------------------

/*
 Consumes triangles and feeds VertexStorage
 */
class TriangleConsumer : public ITriangleConsumer {
 private:
  std::vector<Vertex> _vertices;
  VertexStorage _gpu_storage{GL_TRIANGLES};

 public:
  TriangleConsumer() = default;
  virtual ~TriangleConsumer() = default;

  void Start() override { _vertices.clear(); }
  void AddTriangle(const Triangle& t) override {
    _vertices.push_back(t.a);
    _vertices.push_back(t.b);
    _vertices.push_back(t.c);
  }
  void Finish() override { _gpu_storage.Update(_vertices); }
  void Draw() const override { _gpu_storage.Draw(); }

  const VertexStorage& GetStorage() const { return _gpu_storage; }
  VertexStorage& GetStorage() { return _gpu_storage; }
};

//------------------------------------------------------------------------------

/*
 * Helper class for drawing line segments, e.g.
 * (a->b), (c->d), (e->f)
 */
class LineSegmentBuffer {
 public:
  LineSegmentBuffer() = default;
  LineSegmentBuffer(const LineSegmentBuffer&) = delete;
  LineSegmentBuffer(const LineSegmentBuffer&&) = delete;
  ~LineSegmentBuffer() = default;

  void Clear() {
    _vertices.clear();
    _dirty = true;
  }

  /*
   * Add a single line segment from a->b
   */
  void Add(const Vertex& a, const Vertex& b) {
    _vertices.push_back(a);
    _vertices.push_back(b);
    _dirty = true;
  }

  /*
   * Generate an appropriate list of linesegments to draw the
   * AABB with the provided color
   */
  template<typename T, glm::precision P>
  inline void Add(const ancer::glh::tAABB<T, P>& bounds, const glm::vec4& color) {
    auto corners = bounds.Corners();

    // trace bottom
    Add(Vertex{corners[0], color}, Vertex{corners[1], color});
    Add(Vertex{corners[1], color}, Vertex{corners[2], color});
    Add(Vertex{corners[2], color}, Vertex{corners[3], color});
    Add(Vertex{corners[3], color}, Vertex{corners[0], color});

    // trace top
    Add(Vertex{corners[4], color}, Vertex{corners[5], color});
    Add(Vertex{corners[5], color}, Vertex{corners[6], color});
    Add(Vertex{corners[6], color}, Vertex{corners[7], color});
    Add(Vertex{corners[7], color}, Vertex{corners[4], color});

    // add bars connecting bottom to top
    Add(Vertex{corners[0], color}, Vertex{corners[4], color});
    Add(Vertex{corners[1], color}, Vertex{corners[5], color});
    Add(Vertex{corners[2], color}, Vertex{corners[6], color});
    Add(Vertex{corners[3], color}, Vertex{corners[7], color});
  }

  void Draw() {
    if (_dirty) {
      _gpuStorage.Update(_vertices);
      _dirty = false;
    }
    _gpuStorage.Draw();
  }

 private:
  bool _dirty = false;
  std::vector<Vertex> _vertices;
  VertexStorage _gpuStorage{GL_LINES};
};

/*
 * Helper class for drawing line strips, e.g.:
 * (a->b->c->d->e)
 */
class LineStripBuffer {
 public:
  LineStripBuffer() = default;
  LineStripBuffer(const LineStripBuffer&) = delete;
  LineStripBuffer(const LineStripBuffer&&) = delete;
  ~LineStripBuffer() = default;

  void Clear() {
    _vertices.clear();
    _dirty = true;
  }

  void Add(const Vertex& a) {
    _vertices.push_back(a);
    _dirty = true;
  }

  void Close() {
    _vertices.push_back(_vertices.front());
    _dirty = true;
  }

  void Draw() {
    if (_dirty) {
      _gpuStorage.Update(_vertices);
      _dirty = false;
    }
    _gpuStorage.Draw();
  }

 private:
  bool _dirty = false;
  std::vector<Vertex> _vertices;
  VertexStorage _gpuStorage{GL_LINE_STRIP};
};

} // namespace marching_cubes

#endif /* storage_hpp */
