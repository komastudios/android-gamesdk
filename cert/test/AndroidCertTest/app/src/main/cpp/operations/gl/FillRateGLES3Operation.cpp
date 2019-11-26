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
constexpr Log::Tag TAG{"FillRateGLES3Operation"};
}

//==================================================================================================

namespace {
struct configuration_increment {
  Seconds period;
  int num_quads_increment;
  int quad_size_increment;
};

JSON_CONVERTER(configuration_increment) {
  JSON_REQVAR(period);
  JSON_REQVAR(num_quads_increment);
  JSON_REQVAR(quad_size_increment);
}

//--------------------------------------------------------------------------------------------------

struct base_configuration {
  int num_quads = 0;
  int quad_size = 0;
  bool blending = false;
  int instances_per_renderer = 64;
};

struct configuration : base_configuration {
  configuration_increment increment;
  int min_fps_threshold = 0;
};

JSON_CONVERTER(configuration) {
  JSON_REQVAR(num_quads);
  JSON_REQVAR(quad_size);
  JSON_REQVAR(blending);
  JSON_OPTVAR(instances_per_renderer);
  JSON_OPTVAR(increment);
  JSON_OPTVAR(min_fps_threshold);
}

//--------------------------------------------------------------------------------------------------

struct fill_rate_performance {
  int num_quads = 0;
  int quad_size = 0;
  float pixels_per_second = 0.0f;
  int pixels_per_quad = 0;
};

JSON_WRITER(fill_rate_performance) {
  JSON_REQVAR(num_quads);
  JSON_REQVAR(quad_size);
  JSON_REQVAR(pixels_per_second);
  JSON_REQVAR(pixels_per_quad);
}

struct datum {
  fill_rate_performance fill_rate;
};

JSON_WRITER(datum) {
  JSON_REQVAR(fill_rate);
}
}

//==================================================================================================

namespace {

struct Vertex {
  vec2 pos;
  vec2 tex_coord;
  vec4 rgba;
};

const Vertex QUAD[4] = {
    {{-0.5f, +0.5f}, {0, 0}, {0, 1, 1, 1}},
    {{-0.5f, -0.5f}, {0, 1}, {1, 0, 0, 1}},
    {{+0.5f, +0.5f}, {1, 0}, {1, 0, 1, 1}},
    {{+0.5f, -0.5f}, {1, 1}, {1, 1, 0, 1}}
};

enum class Attributes : GLuint {
  Pos,
  Color,
  Texcoord,
  Offset,
  Scalerot
};

constexpr float LINEAR_VEL = 100.0F;
constexpr float ANGULAR_VEL = static_cast<float>(M_PI);
constexpr const char *OPAQUE_TEXTURE_FILE = "Textures/sphinx.png";
constexpr const char *BLENDING_TEXTURE_FILE = "Textures/dvd.png";

/**
 * Abstract quad renderer that declares some purely virtual rendering life cycle methods.
 */
class QuadRenderer {
 public:

  QuadRenderer(GLuint num_instances, float quad_size) :
      _num_quads(num_instances), _quad_size(quad_size), _egl_context(eglGetCurrentContext()),
      _width(0), _height(0) {}

  virtual ~QuadRenderer() = default;

  int NumQuads() const { return _num_quads; }

  /**
   * Initial state of animation to render.
   */
  virtual void Start() = 0;

  virtual void Resize(int width, int height) {
    _width = width;
    _height = height;
  }

  /**
   * Recomputes the new state for the rendering animation.
   * @param delta_t time in seconds since last call.
   */
  virtual void Step(double delta_t) = 0;

  /**
   * Renders the current animation state for all quads.
   */
  virtual void Draw() = 0;

 protected:

  EGLContext _egl_context = EGL_NO_CONTEXT;
  int _width, _height;
  GLuint _num_quads;
  float _quad_size;
};

/**
 * Actual QuadRenderer implementation used by FillRateGLES3Operation.
 */
class InstancedQuadRenderer : public QuadRenderer {
 public:

  InstancedQuadRenderer(GLuint num_instances, float quad_size) :
      QuadRenderer(num_instances, quad_size), _needs_initialize_positions(true), _vb_state(0) {
    for (unsigned int &i : _vb) { i = 0; }
  }

  virtual ~InstancedQuadRenderer() {
    if (eglGetCurrentContext()==_egl_context) {
      this->DeleteGlResources();
    }
  };

  static std::tuple<std::string, std::string> GetShaderFiles() {
    return std::make_tuple<std::string, std::string>(
        "Shaders/FillRateGLES3Operation/quad_instanced.vsh",
        "Shaders/FillRateGLES3Operation/quad_instanced.fsh");
  }

  void Start() override {
    //
    //  Initialize animation state
    //

    const int range = 1000;
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, range);

    _positions.resize(_num_quads);
    _velocities.resize(_num_quads);
    _angular_velocities.resize(_num_quads);
    _angles.resize(_num_quads);

    // assign random velocities
    for (auto &v : _velocities) {
      auto value =
          vec2(dist(rng), dist(rng))/vec2(range, range); // we have values from 0,1
      value = value*vec2(2, 2) - vec2(1, 1); // we have values from -1,1
      float vel = (LINEAR_VEL/2) +
          ((LINEAR_VEL/2)*(dist(rng)/static_cast<float>(range)));
      v = glm::normalize(value)*vel;
    }

    // assign random initial angles
    for (float &a : _angles) {
      auto value = static_cast<float>(dist(rng))/range;
      value = value*2 - 1;
      a = static_cast<float>(value*M_PI);
    }

    // assign random angular velocities
    for (float &av : _angular_velocities) {
      auto value = static_cast<float>(dist(rng))/range;
      value = value*2 - 1;
      av = value*ANGULAR_VEL;
    }

    //
    //  Initialize vertex buffers
    //

    {
      ANCER_SCOPED_TRACE("FillRateGLES3Operation::InstancedQuadRenderer::start");

      glGenBuffers(VbCount, _vb);
      glBindBuffer(GL_ARRAY_BUFFER, _vb[VbInstance]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD), &QUAD[0], GL_STATIC_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, _vb[VbScalerot]);
      glBufferData(
          GL_ARRAY_BUFFER,
          static_cast<GLsizeiptr>(_num_quads*4*sizeof(float)),
          nullptr, GL_DYNAMIC_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, _vb[VbPosition]);
      glBufferData(
          GL_ARRAY_BUFFER,
          static_cast<GLsizeiptr>(_num_quads*2*sizeof(float)),
          nullptr, GL_STATIC_DRAW);

      glGenVertexArrays(1, &_vb_state);
      glBindVertexArray(_vb_state);

      glBindBuffer(GL_ARRAY_BUFFER, _vb[VbInstance]);
      glVertexAttribPointer(
          static_cast<GLuint>(Attributes::Pos), 2, GL_FLOAT, GL_FALSE,
          sizeof(Vertex),
          (const GLvoid *) offsetof(Vertex, pos));
      glVertexAttribPointer(
          static_cast<GLuint>(Attributes::Color), 4, GL_FLOAT, GL_TRUE,
          sizeof(Vertex),
          (const GLvoid *) offsetof(Vertex, rgba));
      glVertexAttribPointer(
          static_cast<GLuint>(Attributes::Texcoord), 2, GL_FLOAT,
          GL_TRUE,
          sizeof(Vertex),
          (const GLvoid *) offsetof(Vertex, tex_coord));

      glEnableVertexAttribArray(static_cast<GLuint>(Attributes::Pos));
      glEnableVertexAttribArray(static_cast<GLuint>(Attributes::Color));
      glEnableVertexAttribArray(static_cast<GLuint>(Attributes::Texcoord));

      glBindBuffer(GL_ARRAY_BUFFER, _vb[VbScalerot]);
      glVertexAttribPointer(
          static_cast<GLuint>(Attributes::Scalerot), 4, GL_FLOAT,
          GL_FALSE,
          4*sizeof(float), nullptr);
      glEnableVertexAttribArray(static_cast<GLuint>(Attributes::Scalerot));
      glVertexAttribDivisor(static_cast<GLuint>(Attributes::Scalerot), 1);

      glBindBuffer(GL_ARRAY_BUFFER, _vb[VbPosition]);
      glVertexAttribPointer(
          static_cast<GLuint>(Attributes::Offset), 2, GL_FLOAT,
          GL_FALSE,
          2*sizeof(float), nullptr);
      glEnableVertexAttribArray(static_cast<GLuint>(Attributes::Offset));
      glVertexAttribDivisor(static_cast<GLuint>(Attributes::Offset), 1);
    }
  }

  void Resize(int width, int height) override {
    QuadRenderer::Resize(width, height);
    _needs_initialize_positions = true;
  }

  void Step(double delta_t) override {
    constexpr auto sqrt2 = static_cast<float>(M_SQRT2);
    const float delta_tf = static_cast<float>(delta_t);
    const float min_x = sqrt2*_quad_size/2;
    const float max_x = _width - sqrt2*_quad_size/2;
    const float min_y = sqrt2*_quad_size/2;
    const float max_y = _height - sqrt2*_quad_size/2;

    // write to offsets

    if (_needs_initialize_positions) {
      // randomly position quads
      std::random_device dev;
      std::mt19937 rng(dev());
      std::uniform_int_distribution<std::mt19937::result_type> xDist(
          0,
          static_cast<unsigned int>(_width));
      std::uniform_int_distribution<std::mt19937::result_type> yDist(
          0,
          static_cast<unsigned int>(_height));

      for (int i = 0; i < _num_quads; i++) {
        _positions[i] = vec2(xDist(rng), yDist(rng));
      }

      _needs_initialize_positions = false;
    }

    ANCER_SCOPED_TRACE("FillRateGLES3Operation::InstancedQuadRenderer::step");

    auto positions = MapPositionsBuf();

    // update positions
    for (int i = 0; i < _num_quads; i++) {
      auto pos = _positions[i];

      float x = pos.x;
      float y = pos.y;

      if (x > max_x) {
        _velocities[i] *= vec2(-1, 1);
        pos = vec2(max_x, y);
      }

      if (x < min_x) {
        _velocities[i] *= vec2(-1, 1);
        pos = vec2(min_x, y);
      }

      if (y > max_y) {
        _velocities[i] *= vec2(1, -1);
        pos = vec2(x, max_y);
      }

      if (y < min_y) {
        _velocities[i] *= vec2(1, -1);
        pos = vec2(x, min_y);
      }

      pos += _velocities[i]*delta_tf;
      _positions[i] = pos;

      positions[i] = pos;
    }

    UnmapPositionsBuf();

    auto transforms = MapTransformBuf();
    for (int i = 0; i < _num_quads; i++) {
      float a = _angles[i];
      float s = sinf(a);
      float c = cosf(a);
      _angles[i] += _angular_velocities[i]*delta_tf;

      transforms[i] = glm::vec4(
          c*_quad_size,
          s*_quad_size,
          -s*_quad_size,
          c*_quad_size
      );
    }

    UnmapTransformBuf();
  }

  void Draw() override {
    ANCER_SCOPED_TRACE("FillRateGLES3Operation::InstancedQuadRenderer::draw");
    glBindVertexArray(_vb_state);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, _num_quads);
  }

 private:

  vec2 *MapPositionsBuf() {
    glBindBuffer(GL_ARRAY_BUFFER, _vb[VbPosition]);
    return static_cast<vec2 *>(glMapBufferRange(
        GL_ARRAY_BUFFER,
        0, static_cast<GLsizeiptr>(_num_quads*2*
            sizeof(float)),
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
  }

  void UnmapPositionsBuf() {
    glUnmapBuffer(GL_ARRAY_BUFFER);
  }

  vec4 *MapTransformBuf() {
    glBindBuffer(GL_ARRAY_BUFFER, _vb[VbScalerot]);
    return static_cast<vec4 *>(glMapBufferRange(
        GL_ARRAY_BUFFER,
        0, static_cast<GLsizeiptr >(_num_quads*4*
            sizeof(float)),
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
  }

  void UnmapTransformBuf() {
    glUnmapBuffer(GL_ARRAY_BUFFER);
  }

  /**
   * Releases all allocated GL resources. The destructor calls this function if the EGL context is
   * the same than the one at the moment of construction.
   */
  void DeleteGlResources() {
    glDeleteVertexArrays(1, &_vb_state);
    glDeleteBuffers(VbCount, _vb);
  }

 private:

  enum {
    VbInstance, VbScalerot, VbPosition, VbCount
  };

  bool _needs_initialize_positions;

  // animation state
  std::vector<vec2> _positions;
  std::vector<vec2> _velocities;
  std::vector<float> _angular_velocities;
  std::vector<float> _angles;

  // opengl
  GLuint _vb[VbCount];
  GLuint _vb_state;
};
}

#define RENDERER InstancedQuadRenderer

/**
 * This is the operation that measures
 */
class FillRateGLES3Operation : public BaseGLES3Operation {
 public:

  FillRateGLES3Operation() = default;

  ~FillRateGLES3Operation() override {
    glDeleteProgram(_program);
    glDeleteTextures(1, &_tex_id);
  }

  void OnGlContextReady(const GLContextConfig &ctx_config) override {
    Setup(GetConfiguration<configuration>());
    SetHeartbeatPeriod(2000ms);

    Log::I(
        TAG, "glContextReady, configuration: %s - loading shaders, textures, etc",
        Json(_configuration).dump().c_str());

    if (eglGetCurrentContext()==EGL_NO_CONTEXT) {
      FatalError(TAG, "No EGL context available");
    }

    int tex_width = 0;
    int tex_height = 0;
    _tex_id = LoadTexture(
        _configuration.blending ? BLENDING_TEXTURE_FILE : OPAQUE_TEXTURE_FILE,
        &tex_width, &tex_height, nullptr);
    if (tex_width==0 || tex_height==0) {
      FatalError(TAG, "Unable to load texture");
    }

    if (_configuration.blending) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
      glDisable(GL_BLEND);
    }

    //
    //  Build the shader program and get uniform locations
    //

    std::string vertex_file;
    std::string fragment_file;
    std::tie(vertex_file, fragment_file) = RENDERER::GetShaderFiles();
    _program = CreateProgram(vertex_file.c_str(), fragment_file.c_str());

    if (!_program) {
      FatalError(TAG, "Unable to load quad program");
    }

    _tex_id_uniform_loc = glGetUniformLocation(_program, "uTex");
    glh::CheckGlError("looking up uTex");

    _projection_uniform_loc = glGetUniformLocation(_program, "uProjection");
    glh::CheckGlError("looking up uProjection");

    //
    //  build renderers with current config
    //

    BuildRenderers(_current_configuration);
  }

  void OnGlContextResized(int width, int height) override {
    BaseGLES3Operation::OnGlContextResized(width, height);

    _projection = glh::Ortho2d(0, 0, width, height);

    for (auto &r : _renderers) r->Resize(width, height);
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);

    glh::CheckGlError("FillRateGLES3Operation::draw() - Start");

    // bind program and assign projection and texture uniforms
    glUseProgram(_program);
    glh::CheckGlError("FillRateGLES3Operation::draw() - glUseProgram");

    glUniformMatrix4fv(_projection_uniform_loc, 1, GL_FALSE, glm::value_ptr(_projection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tex_id);
    glUniform1i(_tex_id_uniform_loc, 0);

    for (auto &r : _renderers) {
      r->Step(delta_seconds);
      r->Draw();
      _quads_rendered_since_last_fps_timestamp += r->NumQuads();
    }

    _frames_rendered_since_last_fps_timestamp++;
  }

  void OnHeartbeat(Duration elapsed) override {
    if (GetMode()==Mode::DataGatherer) {

      //
      //  Record a perf datum
      //


      auto seconds_elapsed = duration_cast<SecondsAs<float>>(elapsed);
      auto quads_per_sec = static_cast<float>(_quads_rendered_since_last_fps_timestamp
          /seconds_elapsed.count());
      auto pixels_per_quad = _current_configuration.quad_size*_current_configuration.quad_size;
      Report(datum{{
                       _current_configuration.num_quads,
                       _current_configuration.quad_size,
                       quads_per_sec,
                       pixels_per_quad
                   }});
    }

    //
    //  Check for configuration change timeout
    //

    _time_since_configuration_change += elapsed;
    if ((_configuration.increment.period > Duration::zero()) &&
        (_time_since_configuration_change >= _configuration.increment.period)) {
      ChangeConfiguration();
      _time_since_configuration_change = Duration::zero();
    }

    _frames_rendered_since_last_fps_timestamp = 0;
    _quads_rendered_since_last_fps_timestamp = 0;
  }

 private:

  /**
   * Based on the total operation run duration, calculates the mid-point in order to set a "change
   * threshold". For the first half of this operation run, every time that the configuration
   * changes, both the number of quads and the size of each quad increase. At mid-point, the number
   * of quads returns to the beginning. Throughout the second half, each configuration change will
   * increase the number of quads while decreasing the size of each.
   *
   * @see FillRateGLES3Operation::ChangeConfiguration().
   */
  void Setup(const configuration) {
    _configuration = GetConfiguration<configuration>();
    _current_configuration = _configuration;

    std::chrono::duration<float> total_duration = GetDuration();
    std::chrono::duration<float> half_duration = total_duration/2;

    _change_threshold = static_cast<uint>(
        std::ceil(half_duration/_configuration.increment.period)
    );

    _configuration_changes = 0;
  }

  /**
   * During this operation running, at regular intervals this function is called to change the
   * configuration of rendered quads (number of them and size of each).
   * At each call, this function will increase both number of quads and quad size until the elapsed
   * execution time is half the total test time.
   * At that time, the number of quads will go back to the beginning. Thereafter, at each call to
   * this function, the number of quads will increase while the size of each quad will start
   * decreasing from the maximum achieved at mid-point.
   *
   * This way, we cover the four quadrants:
   * - Low number of small quads (first execution quarter).
   * - High number of big quads (second execution quarter).
   * - Low number of big quads (third execution quarter).
   * - High number of small quads (fourth execution quarter).
   */
  void ChangeConfiguration() {
    ++_configuration_changes;

    if (_configuration_changes==_change_threshold) {
      _current_configuration.num_quads = _configuration.num_quads;
    } else if ((_configuration_changes < _change_threshold)) {
      _current_configuration.quad_size += _configuration.increment.quad_size_increment;
      _current_configuration.num_quads += _configuration.increment.num_quads_increment;
    } else {
      _current_configuration.quad_size -= _configuration.increment.quad_size_increment;
      _current_configuration.num_quads += _configuration.increment.num_quads_increment;
    }

    Log::I(
        TAG, "Changing configuration to total quads: %d; quad size: %d",
        _current_configuration.num_quads, _current_configuration.quad_size);

    ancer::GetFpsCalculator().Ignore(
        [this]() {
          BuildRenderers(_current_configuration);
        });
  }

  void BuildRenderers(base_configuration configuration) {
    // clean up previous renderers, if any
    _renderers.clear();

    int remaining = configuration.num_quads;
    while (remaining > 0) {
      int instances = std::min(remaining, configuration.instances_per_renderer);
      remaining -= instances;

      _renderers.push_back(std::make_shared<RENDERER>(instances, configuration.quad_size));
      _renderers.back()->Start();

      auto size = GetGlContextSize();
      if (size.x > 0 && size.y > 0) {
        _renderers.back()->Resize(size.x, size.y);
      }
    }
  }

 private:

  configuration _configuration;
  base_configuration _current_configuration;
  int64_t _frames_rendered_since_last_fps_timestamp = 0;
  int64_t _quads_rendered_since_last_fps_timestamp = 0;
  Duration _time_since_configuration_change;
  uint _change_threshold;
  uint _configuration_changes;

  // opengl
  GLuint _program = 0;
  GLuint _tex_id = 0;
  GLint _tex_id_uniform_loc = 0;
  GLint _projection_uniform_loc = 0;
  mat4 _projection;

  // renderers
  std::vector<std::shared_ptr<QuadRenderer>> _renderers;
};

EXPORT_ANCER_OPERATION(FillRateGLES3Operation)