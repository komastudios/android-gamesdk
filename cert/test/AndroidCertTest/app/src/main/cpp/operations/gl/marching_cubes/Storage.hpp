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

//------------------------------------------------------------------------------

struct Vertex {
    vec3 pos;
    vec3 color { 1 };
    vec3 normal;

    enum class AttributeLayout : GLuint {
        Pos = 0,
        Color = 1,
        Normal = 2
    };

    bool operator==(const Vertex& other) const
    {
        return pos == other.pos && color == other.color && normal == other.normal;
    }

    static void BindVertexAttributes();
};

struct Triangle {
  Vertex a, b, c;

  Triangle() = default;

  Triangle(const Vertex& a, const Vertex& b, const Vertex& c)
      : a(a)
      , b(b)
      , c(c)
  {
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
        : _mode(mode)
        , _growth_factor(growthFactor)
    {
    }

    /*
     * Set up a vertex storage to draw using mode (e.g., GL_TRIANGLES)
     * and to grow by growthFactor when the high-water mark is reached.
     */
    VertexStorage(const std::vector<Vertex>& vertices, GLenum mode, float growthFactor = 1.5F)
        : _mode(mode)
        , _growth_factor(growthFactor)
    {
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

/*
 TriangleConsumer which does nothing but record how many triangles were
 delivered between start() and end();
 meant for profiling, testing, etc.
 */
class CountingTriangleConsumer : public ITriangleConsumer {
 public:
  CountingTriangleConsumer() = default;

  void Start() override
  {
    _triangle_count = 0;
  }

  void AddTriangle(const Triangle& t) override
  {
    _triangle_count++;
  }

  void Finish() override
  {
  }

  void Draw() const override {}

  size_t GetNumTriangles() const { return _triangle_count; }

 private:
  size_t _triangle_count = 0;
};

//------------------------------------------------------------------------------

/*
 Consumes triangles and feeds VertexStorage
 */
class TriangleConsumer : public ITriangleConsumer {
 private:
  std::vector<Vertex> _vertices;
  VertexStorage _gpu_storage { GL_TRIANGLES };

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


#endif /* storage_hpp */
