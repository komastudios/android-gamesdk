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
#include <cmath>
#include <random>
#include <memory>
#include <tuple>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Error.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==================================================================================================

namespace {
constexpr Log::Tag TAG{"VertexStreamGLES3Operation"};
}

//==================================================================================================

namespace {
struct configuration_increment {
  Seconds period;
  int increment;
};

JSON_CONVERTER(configuration_increment) {
  JSON_REQVAR(period);
  JSON_REQVAR(increment);
}

//--------------------------------------------------------------------------------------------------

struct configuration {
  int count;
  int rows;
  int cols;
  configuration_increment increment;
  int min_fps_threshold;

  configuration() : count(0), rows(0), cols(0) {}
};

JSON_CONVERTER(configuration) {
  JSON_REQVAR(count);
  JSON_REQVAR(rows);
  JSON_REQVAR(cols);
  JSON_OPTVAR(increment);
  JSON_OPTVAR(min_fps_threshold);
}

//--------------------------------------------------------------------------------------------------

struct vertex_throughput {
  float vertices_per_second;
  int components_per_vertex;
  size_t vertices_per_renderer;
  size_t renderers;
};

JSON_WRITER(vertex_throughput) {
  JSON_REQVAR(vertices_per_second);
  JSON_REQVAR(components_per_vertex);
  JSON_REQVAR(vertices_per_renderer);
  JSON_REQVAR(renderers);
}

struct datum {
  vertex_throughput vertex_throughput;
};

JSON_WRITER(datum) {
  JSON_REQVAR(vertex_throughput);
}
}

//==================================================================================================

namespace {

struct Vertex {
  vec2 pos;
  vec2 tex_coord;
  vec4 rgba;
};

constexpr auto VERTEX_COMPONENTS = 8;

enum class Attributes : GLuint {
  Pos = 0,
  Color = 1,
  Texcoord = 2,
};

constexpr auto MAX_ROWS = 255;
constexpr auto MAX_COLS = 255;
constexpr auto B = M_PI*0.0125;
constexpr auto C = M_PI*0.025;

class StreamRenderer {
 public:

  StreamRenderer(int rows, int cols, int left, int top, int width, int height) :
      _width(width), _height(height), VERTICES_PER_ROW(cols + 1),
      _rows(std::min(rows, MAX_ROWS)), _cols(std::min(cols, MAX_COLS)),
      R(std::min(_width, _height)/64), _time(0), _steps(0),
      _vertex_vbo_id(0), _index_vbo_id(0), _vbo_state(0) {
    //
    //  Build mesh
    //

    int n_verts_across = cols + 1;
    int n_verts_tall = rows + 1;

    for (int row = 0; row < n_verts_tall; ++row) {
      const auto across_row = static_cast<float>(row)/rows;
      for (int col = 0; col < n_verts_across; ++col) {
        const auto across_col = static_cast<float>(col)/cols;

        float x = left + width*across_col;
        float y = top + height*across_row;

        float s = across_col;
        float t = across_row;

        float r = across_col;
        float g = 1 - across_col;
        float b = across_row;
        float a = 1;

        _vertices.push_back(
            Vertex{
                {x, y},
                {s, t},
                {r, g, b, a}
            });

        if (row < rows && col < cols) {
          auto i = col + row*n_verts_across;

          _indices.push_back(static_cast<GLushort>(i));
          _indices.push_back(static_cast<GLushort>(i + 1));
          _indices.push_back(static_cast<GLushort>(i + n_verts_across));

          _indices.push_back(static_cast<GLushort>(i + 1));
          _indices.push_back(static_cast<GLushort>(i + 1 + n_verts_across));
          _indices.push_back(static_cast<GLushort>(i + n_verts_across));
        }
      }
    }

    //
    //  Initialize vertex buffers
    //

    {
      ANCER_SCOPED_TRACE("VertexStreamGLES3Operation::Renderer::ctor");

      glGenBuffers(1, &_vertex_vbo_id);
      glBindBuffer(GL_ARRAY_BUFFER, _vertex_vbo_id);
      glBufferData(
          GL_ARRAY_BUFFER, sizeof(Vertex)*_vertices.size(), nullptr,
          GL_STREAM_DRAW);

      glGenBuffers(1, &_index_vbo_id);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_vbo_id);
      glBufferData(
          GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*_indices.size(),
          &_indices[0],
          GL_STATIC_DRAW);

      glh::CheckGlError("building buffers");

      glGenVertexArrays(1, &_vbo_state);
      glBindVertexArray(_vbo_state);

      glh::CheckGlError("binding vertex array");

      glVertexAttribPointer(
          static_cast<GLuint>(Attributes::Pos), 2, GL_FLOAT, GL_FALSE,
          sizeof(Vertex),
          (const GLvoid *) offsetof(Vertex, pos));

      glVertexAttribPointer(
          static_cast<GLuint>(Attributes::Texcoord), 2, GL_FLOAT,
          GL_TRUE,
          sizeof(Vertex),
          (const GLvoid *) offsetof(Vertex, tex_coord));

      glVertexAttribPointer(
          static_cast<GLuint>(Attributes::Color), 4, GL_FLOAT, GL_TRUE,
          sizeof(Vertex),
          (const GLvoid *) offsetof(Vertex, rgba));

      glh::CheckGlError("setting attrib pointers");

      glEnableVertexAttribArray(static_cast<GLuint>(Attributes::Pos));
      glEnableVertexAttribArray(static_cast<GLuint>(Attributes::Texcoord));
      glEnableVertexAttribArray(static_cast<GLuint>(Attributes::Color));

      glBindBuffer(GL_ARRAY_BUFFER, _vertex_vbo_id);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_vbo_id);

      glDisable(GL_CULL_FACE);
      glDisable(GL_DEPTH_TEST);
    }
  }

  ~StreamRenderer() {
    if (_vbo_state > 0) glDeleteVertexArrays(1, &_vbo_state);
    if (_vertex_vbo_id > 0) glDeleteBuffers(1, &_vertex_vbo_id);
    if (_index_vbo_id > 0) glDeleteBuffers(1, &_index_vbo_id);
  }

  void Step(double delta_t) {
    ANCER_SCOPED_TRACE("VertexStreamGLES3Operation::Renderer::step");

    _time += delta_t;

    auto vertices = MapVerticesBuffer();
    auto vertex_iterator = vertices;
    size_t iteration_order = 0;

    for (const auto &v : _vertices) {
      auto vv = v;

      vv.pos += CalculatePositionDelta(iteration_order);
      vv.tex_coord = TwistTextureCoordinates(v);
      vv.rgba = TwistColorComposition(v);

      *vertex_iterator = vv;
      ++vertex_iterator;
      ++iteration_order;
    }

    // modulate vertex positions
    UnmapVerticesBuffer();
    ++_steps;
  }

  void Draw() {
    glBindVertexArray(_vbo_state);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_SHORT, nullptr);
  }

  size_t VertexCount() const { return _vertices.size(); }

 private:

  Vertex *MapVerticesBuffer() {
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_vbo_id);
    return static_cast<Vertex *>(glMapBufferRange(
        GL_ARRAY_BUFFER,
        0, _vertices.size()*sizeof(Vertex),
        GL_MAP_WRITE_BIT |
            GL_MAP_INVALIDATE_BUFFER_BIT));
  }

  void UnmapVerticesBuffer() {
    glUnmapBuffer(GL_ARRAY_BUFFER);
  }

  /**
   * Given a vertex iteration order, calculates some slight position offset.
   */
  vec2 CalculatePositionDelta(const size_t order) {
    int row = order/VERTICES_PER_ROW;
    int col = static_cast<int>(floor(order - (row*VERTICES_PER_ROW)));

    const auto A = _time*M_PI*4 + row*B - col*C;

    return vec2(static_cast<float>(std::cos(A)*R), static_cast<float>(std::sin(A)*R));
  }

  /**
   * At regular frame steps, this function swaps the texture coordinates of the original vertex.
   */
  vec2 TwistTextureCoordinates(const Vertex &v) {
    const auto gap = _steps%20;
    if (gap < 5) {
      return vec2(v.tex_coord.x, v.tex_coord.y);
    } else if (gap <10) {
      return vec2(v.tex_coord.y, v.tex_coord.x);
    } else if (gap <15) {
      return vec2(-v.tex_coord.x, -v.tex_coord.y);
    } else {
      return vec2(-v.tex_coord.y, -v.tex_coord.x);
    }
  }

  /**
   * At regular frame steps, this function exchanges the color components of the original vertex.
   */
  vec4 TwistColorComposition(const Vertex &v) {
    switch (_steps%3) {
      case 1:return vec4(v.rgba.y, v.rgba.z, v.rgba.x, v.rgba.w);

      case 2:return vec4(v.rgba.z, v.rgba.x, v.rgba.y, v.rgba.w);

      default:return vec4(v.rgba.x, v.rgba.y, v.rgba.z, v.rgba.w);
    }
  }

 private:

  GLuint _vertex_vbo_id;
  GLuint _index_vbo_id;
  GLuint _vbo_state;

  // internal state
  int _width, _height, _rows, _cols;
  const int VERTICES_PER_ROW;
  const double R;
  double _time;
  int _steps;
  std::vector<Vertex> _vertices;
  std::vector<GLushort> _indices;
};
}

/**
 * This operation draws an incremental number of indexed vertices. Its goal is to measure the device
 * capacity to handle such incremental workload without getting throughput (e.g., fps) to drop below
 * acceptable thresholds.
 *
 * This operation delegates on a local Renderer the task of drawing a single tile. That way,
 * increasing workload at regular intervals is a matter of adding more renderers. Every rendered
 * frame is the result of all available renderers rendering their respective part.
 */
class VertexStreamGLES3Operation : public BaseGLES3Operation {
 public:

  VertexStreamGLES3Operation() = default;

  ~VertexStreamGLES3Operation() override {
    if (eglGetCurrentContext()==_egl_context) {
      glDeleteProgram(_program);
      glDeleteTextures(1, &_tex_id);
    }
  }

  void OnGlContextReady(const GLContextConfig &ctx_config) override {
    SetHeartbeatPeriod(250ms);

    _configuration = GetConfiguration<configuration>();
    _renderer_count = _configuration.count;

    Log::I(
        TAG, "glContextReady, configuration: %s - loading shaders, textures, etc",
        Json(_configuration).dump().c_str());

    _egl_context = eglGetCurrentContext();
    if (_egl_context==EGL_NO_CONTEXT) {
      FatalError(TAG, "No EGL context available");
    }

    _tex_id = SetupTexture();

    glDisable(GL_BLEND);

    _program = SetupProgram();

    _context_ready = true;
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);

    glUseProgram(_program);

    glUniformMatrix4fv(_projection_uniform_loc, 1, GL_FALSE, glm::value_ptr(_projection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tex_id);
    glUniform1i(_tex_id_uniform_loc, 0);

    // The operation drawing actually delegates onto its current renderers to draw themselves.
    for (const auto &r : _renderers) {
      r->Step(delta_seconds);
      r->Draw();
    }
  }

  void OnGlContextResized(int width, int height) override {
    BaseGLES3Operation::OnGlContextResized(width, height);

    _projection = glh::Ortho2d(0, 0, width, height);

    if (_context_ready) {
      BuildRenderers();
    }
  }

  void OnHeartbeat(Duration elapsed) override {
    if (GetMode()==Mode::DataGatherer) {
      ReportVertexThroughput(duration_cast<SecondsAs<float >>(elapsed));
    }

    _heartbeat_time += elapsed;
    if ((_configuration.increment.increment > 0) &&
        (_configuration.increment.period > Duration::zero()) &&
        (_heartbeat_time >= _configuration.increment.period)) {
      _heartbeat_time -= _configuration.increment.period;
      _renderer_count += _configuration.increment.increment;

      GetFpsCalculator().Ignore([this]() { BuildRenderers(); });
    }
  }

 private:

  /**
   * Loads a texture, aborting the execution in case of failure.
   * @return the texture ID.
   */
  GLuint SetupTexture() {
    int tex_width = 0;
    int tex_height = 0;

    GLuint tex_id = LoadTexture(
        "Textures/sphinx.png",
        &tex_width, &tex_height, nullptr);
    if (tex_width==0 || tex_height==0) {
      FatalError(TAG, "Unable to load texture");
    }

    return tex_id;
  }

  /**
   * Creates a shader program based on app assets.
   * @return the program id.
   */
  GLuint SetupProgram() {
    auto vertex_file = "Shaders/VertexStreamGLES3Operation/simple.vsh";
    auto fragment_file = "Shaders/VertexStreamGLES3Operation/simple.fsh";

    GLuint program_id = CreateProgram(vertex_file, fragment_file);

    if (!program_id) {
      FatalError(TAG, "Unable to create shader program");
    }

    _tex_id_uniform_loc = glGetUniformLocation(program_id, "uTex");
    glh::CheckGlError("looking up uTex");

    _projection_uniform_loc = glGetUniformLocation(program_id, "uProjection");
    glh::CheckGlError("looking up uProjection");

    return program_id;
  }

  void ReportVertexThroughput(SecondsAs<float> elapsed_seconds) {
    size_t total_vertices = 0;
    for (const auto &r : _renderers) { total_vertices += r->VertexCount(); }
    auto vps = total_vertices/elapsed_seconds.count();

    Report(datum{{vps, VERTEX_COMPONENTS, _renderers[_renderer_count-1]->VertexCount(), _renderer_count}});
  }

  /**
   * Depending on the total of renderers to build, it will set an NxN grid where N is the minimum
   * layout dimension that can hold all renderers.
   */
  void BuildRenderers() {
    _renderers.clear();

    while (_renderer_count > _layout_dimension*_layout_dimension) {
      ++_layout_dimension;
    }

    auto size = GetGlContextSize();
    int shader_width = size.x/_layout_dimension;
    int shader_height = size.y/_layout_dimension;
    int inset = 16;

    for (int i = 0; i < _renderer_count; ++i) {
      int row = i/_layout_dimension;
      int col = i - (row*_layout_dimension);

      int left = col*shader_width + inset;
      int top = row*shader_height + inset;
      int width = shader_width - 2*inset;
      int height = shader_height - 2*inset;

      _renderers.push_back(
          std::make_unique<StreamRenderer>(
              _configuration.rows, _configuration.cols,
              left, top, width, height
          ));
    }
  }

 private:

  configuration _configuration;

  // opengl
  EGLContext _egl_context = nullptr;
  GLuint _program = 0;
  GLuint _tex_id = 0;
  GLint _tex_id_uniform_loc = -1;
  GLint _projection_uniform_loc = -1;
  mat4 _projection;

  // internal state
  bool _context_ready = false;
  Duration _heartbeat_time;
  std::vector<std::unique_ptr<StreamRenderer>> _renderers;
  int _layout_dimension = 1;
  size_t _renderer_count = 1;
};

EXPORT_ANCER_OPERATION(VertexStreamGLES3Operation)
