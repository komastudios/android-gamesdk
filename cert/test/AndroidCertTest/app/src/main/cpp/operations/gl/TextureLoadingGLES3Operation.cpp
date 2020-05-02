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

/**
 * TODO(dagum): document this operation
 */

#include <chrono>
#include <memory>
#include <random>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Error.hpp>
#include <ancer/util/Json.hpp>

#define SIZEOF_PRIMITIVE_ARRAY(a)    (sizeof(a)/sizeof(a[0]))

using namespace ancer;

//==================================================================================================

namespace {
constexpr Log::Tag TAG{"TextureLoadingGLES3"};
}

//==================================================================================================

namespace {
struct configuration {
};

JSON_CONVERTER(configuration) {
}

//--------------------------------------------------------------------------------------------------

struct throughput {
};

void WriteDatum(report_writers::Struct writer, const throughput &throughput) {
}
}

//==================================================================================================

namespace {
struct Vertex {
  vec2 position;
  vec2 texture_coordinates;

  enum class Attributes : GLuint {
    Position,
    TextureCoordinates
  };
};
}

//==================================================================================================

class TextureLoadingGLES3Operation : public BaseGLES3Operation {
 private:
  class Renderer {
   public:
    Renderer(const int left,
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

    virtual ~Renderer() {
      TeardownGL();
    }

    void Draw() {
      ANCER_SCOPED_TRACE("TextureLoadingGLES3Operation::Renderer::Draw");

      glBindVertexArray(_vertex_buffer_object_state);
      glDrawElements(GL_TRIANGLES, SIZEOF_PRIMITIVE_ARRAY(_indices), GL_UNSIGNED_SHORT, nullptr);
    }

//--------------------------------------------------------------------------------------------------

   private:
    void CreateGLObjects() {
      // Initialize vertex buffers
      ANCER_SCOPED_TRACE("TextureLoadingGLES3Operation::Renderer::Setup");

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
          static_cast<GLuint>(Vertex::Attributes::Position), 2, GL_FLOAT, GL_FALSE,
          sizeof(Vertex),
          (const GLvoid *) offsetof(Vertex, position));

      glVertexAttribPointer(
          static_cast<GLuint>(Vertex::Attributes::TextureCoordinates), 2, GL_FLOAT,
          GL_TRUE,
          sizeof(Vertex),
          (const GLvoid *) offsetof(Vertex, texture_coordinates));

      glh::CheckGlError("setting attrib pointers");

      glEnableVertexAttribArray(static_cast<GLuint>(Vertex::Attributes::Position));
      glEnableVertexAttribArray(static_cast<GLuint>(Vertex::Attributes::TextureCoordinates));

      glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object_id);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer_object_id);

      glDisable(GL_CULL_FACE);
      glDisable(GL_DEPTH_TEST);
    }

    void TeardownGL() {
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

//--------------------------------------------------------------------------------------------------

   private:
    GLuint _vertex_buffer_object_id;
    GLuint _index_buffer_object_id;
    GLuint _vertex_buffer_object_state;

//--------------------------------------------------------------------------------------------------

   private:
    int _width;
    int _height;

    Vertex _vertices[4];
    GLushort _indices[6];
  };

//--------------------------------------------------------------------------------------------------

 public:
  TextureLoadingGLES3Operation() = default;

  ~TextureLoadingGLES3Operation() override {
    if (eglGetCurrentContext()==_egl_context) {
      glDeleteProgram(_program);
      glDeleteTextures(1, &_tex_id);
    }
  }

  void OnGlContextReady(const GLContextConfig &ctx_config) override {
    SetHeartbeatPeriod(250ms);

    _configuration = GetConfiguration<configuration>();

    _egl_context = eglGetCurrentContext();
    if (_egl_context==EGL_NO_CONTEXT) {
      FatalError(TAG, "No EGL context available");
    }

    _tex_id = SetupTexture();

    glDisable(GL_BLEND);

    _program = SetupProgram();

    _generator = SetupRandomMultiplierGenerator();

    _random_multiplier_uniform_loc = glGetUniformLocation(_program, "uMultiplier");
    _indirections_uniform_loc = glGetUniformLocation(_program, "uIndirections");

    _fps_calculator = &(GetFpsCalculator());

    _context_ready = true;
  }

  // Depending on screen sizes, tries to fit the projection as a 1:1 square right in the middle.
  void OnGlContextResized(int width, int height) override {
    BaseGLES3Operation::OnGlContextResized(width, height);
    int left{0}, top{0}, right{0}, bottom{0};

    if (width > height) {
      left = (height - width)/2;
      right = width - left;
      bottom = height;
    } else {
      top = (width - height)/2;
      right = width;
      bottom = height - top;
    }

    _projection = glh::Ortho2d(left, top, right, bottom);

    if (_context_ready) {
      SetupRenderer();
    }
  }

  void OnHeartbeat(Duration elapsed) override {
    if (GetMode()==Mode::DataGatherer) {
      _report_timer += elapsed;
      if (_configuration.report_interval > Duration::zero() &&
          _report_timer >= _configuration.report_interval) {
        _report_timer -= _configuration.report_interval;
        Report(throughput{static_cast<uint >(_indirections),
                          _fps_calculator->GetAverageFps(),
                          _fps_calculator->GetMaxFrameTime()
        });
      }
    }

    _indirection_timer += elapsed;
    if (_configuration.indirection_interval > Duration::zero() &&
        _indirection_timer >= _configuration.indirection_interval) {
      _indirection_timer -= _configuration.indirection_interval;
      ++_indirections;
    }
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);
    glh::CheckGlError("TextureLoadingGLES3Operation::Draw(delta_seconds) - superclass");

    glUseProgram(_program);
    glh::CheckGlError("TextureLoadingGLES3Operation::Draw(delta_seconds) - glUseProgram");

    glUniformMatrix4fv(_projection_uniform_loc, 1, GL_FALSE, glm::value_ptr(_projection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tex_id);
    glUniform1i(_tex_id_uniform_loc, 0);

    glUniform1f(_random_multiplier_uniform_loc, static_cast<GLfloat>(_distribution(_generator)));
    glUniform1i(_indirections_uniform_loc, static_cast<GLint>(_indirections));

    if (_renderer) {
      _renderer->Draw();
    }
  }

//--------------------------------------------------------------------------------------------------

 private:
  /**
   * Creates a shader program based on app assets.
   * @return the program id.
   */
  GLuint SetupProgram() {
    auto vertex_file = "Shaders/TextureLoadingGLES3Operation/pass_through.vsh";
    auto fragment_file = "Shaders/TextureLoadingGLES3Operation/indirect_texture_read.fsh";

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

  /**
   * Loads a texture, aborting the execution in case of failure.
   * @return the texture ID.
   */
  static GLuint SetupTexture() {
    int tex_width = 0;
    int tex_height = 0;

    GLuint tex_id = LoadTexture("Textures/sphinx.png", &tex_width, &tex_height, nullptr);
    if (tex_width==0 || tex_height==0) {
      FatalError(TAG, "Unable to load texture");
    }

    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return tex_id;
  }

  /**
   * Creates a true random multiplier generator based on the clock.
   * @return the random multiplier generator.
   */
  static inline std::mt19937 SetupRandomMultiplierGenerator() {
    return std::mt19937(static_cast<unsigned long>(SteadyClock::now().time_since_epoch().count()));
  }

  /**
   * Depending on the total of renderers to build, it will set an NxN grid where N is the minimum
   * layout dimension that can hold all renderers.
   */
  void SetupRenderer() {
    int left = 0;
    int top = 0;

    auto size = GetGlContextSize();
    int width = size.x;
    int height = size.y;

    _renderer = std::make_unique<Renderer>(
        left, top, width, height
    );
  }

//--------------------------------------------------------------------------------------------------

 private:
  EGLContext _egl_context = nullptr;
  GLuint _program = 0;

  GLuint _tex_id = 0;
  GLint _tex_id_uniform_loc = -1;

  mat4 _projection;
  GLint _projection_uniform_loc = -1;

  GLint _indirections_uniform_loc = -1;

  GLint _random_multiplier_uniform_loc = -1;

//--------------------------------------------------------------------------------------------------

 private:
  configuration _configuration{};
  bool _context_ready = false;

  std::unique_ptr<Renderer> _renderer;
  FpsCalculator *_fps_calculator;

  Duration _report_timer;

  std::mt19937 _generator;
  std::uniform_real_distribution<float> _distribution{0.0, 1.0};
  int _indirections;
  Duration _indirection_timer;

};

EXPORT_ANCER_OPERATION(TextureLoadingGLES3Operation)
