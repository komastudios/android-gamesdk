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
#include <thread>
#include <mutex>
#include <condition_variable>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==============================================================================

namespace {
constexpr Log::Tag TAG{"DepthClearGLES3Operation"};
constexpr GLContextConfig RequiredGLContextConfiguration = {8, 8, 8, 8, 32, 0};

struct rgb {
  GLubyte r = 0;
  GLubyte g = 0;
  GLubyte b = 0;
};

bool operator==(const rgb& lhs, const rgb& rhs) {
  return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}

bool operator!=(const rgb& lhs, const rgb& rhs) {
  return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b;
}

JSON_WRITER(rgb) {
  JSON_REQVAR(r);
  JSON_REQVAR(g);
  JSON_REQVAR(b);
}

//------------------------------------------------------------------------------

struct configuration {
  bool run_for_all_depth_precisions = true;
};

JSON_CONVERTER(configuration) {
  JSON_OPTVAR(run_for_all_depth_precisions);
}

//------------------------------------------------------------------------------

struct datum {
  enum class Kind {
    create_info_message,
    success,
    incorrect_fragment_pass,
    incorrect_fragment_reject
  };

  Kind kind;
  bool has_requested_context = false;
  int depth_bits = 0;
  bool writes_passed_as_expected = false;
  bool error_incorrect_fragment_pass = false;
  bool error_incorrect_fragment_rejection = false;
  float depth_clear_value = 0;
  float fragment_depth = 0;
  rgb expected_rgb_value;
  rgb actual_rgb_value;

  static datum create_ctx_info_message(bool is_requested_ctx, int depth_bits) {
    auto d = datum{Kind::create_info_message};
    d.has_requested_context = is_requested_ctx;
    d.depth_bits = depth_bits;
    return d;
  }

  static datum create_success_message(float clear_value) {
    auto d = datum{Kind::success};
    d.writes_passed_as_expected = true;
    d.depth_clear_value = clear_value;
    return d;
  }

  static datum create_error_incorrect_fragment_pass(float fragment_depth,
                                                    float clear_value,
                                                    rgb expected_value,
                                                    rgb actual_value) {
    auto d = datum{Kind::incorrect_fragment_pass};
    d.writes_passed_as_expected = false;
    d.error_incorrect_fragment_pass = true;
    d.fragment_depth = fragment_depth;
    d.depth_clear_value = clear_value;
    d.expected_rgb_value = expected_value;
    d.actual_rgb_value = actual_value;
    return d;
  }

  static datum create_error_incorrect_fragment_rejection(float fragment_depth,
                                                         float clear_value,
                                                         rgb expected_value,
                                                         rgb actual_value) {
    auto d = datum{Kind::incorrect_fragment_reject};
    d.writes_passed_as_expected = false;
    d.error_incorrect_fragment_rejection = true;
    d.fragment_depth = fragment_depth;
    d.depth_clear_value = clear_value;
    d.expected_rgb_value = expected_value;
    d.actual_rgb_value = actual_value;
    return d;
  }

};

JSON_WRITER(datum) {
  switch (data.kind) {
    case datum::Kind::create_info_message:
      JSON_REQVAR(has_requested_context);
      JSON_REQVAR(depth_bits);
      break;
    case datum::Kind::success:
      JSON_REQVAR(writes_passed_as_expected);
      JSON_REQVAR(depth_clear_value);
      break;
    case datum::Kind::incorrect_fragment_pass:
      JSON_REQVAR(writes_passed_as_expected);
      JSON_REQVAR(error_incorrect_fragment_pass);
      JSON_REQVAR(fragment_depth);
      JSON_REQVAR(depth_clear_value);
      JSON_REQVAR(expected_rgb_value);
      JSON_REQVAR(actual_rgb_value);
      break;
    case datum::Kind::incorrect_fragment_reject:
      JSON_REQVAR(writes_passed_as_expected);
      JSON_REQVAR(error_incorrect_fragment_rejection);
      JSON_REQVAR(fragment_depth);
      JSON_REQVAR(depth_clear_value);
      JSON_REQVAR(expected_rgb_value);
      JSON_REQVAR(actual_rgb_value);
      break;
  }
}

//------------------------------------------------------------------------------

struct Vertex {
  vec3 pos;
  rgb color;
};

enum class Attributes : GLuint {
  Pos = 0,
  Color = 1
};

struct Probe {
  ivec2 pos;
  float clip_z;
  float depth;
  rgb written_value;
};

} // anonymous namespace

class DepthClearGLES3Operation : public BaseGLES3Operation {
 public:

  DepthClearGLES3Operation() = default;

  ~DepthClearGLES3Operation() override {
    Cleanup();
  }

  std::optional<GLContextConfig> GetGLContextConfiguration() const override {
    return RequiredGLContextConfiguration;
  }

  void OnGlContextReady(const GLContextConfig& ctx_config) override {
    Log::D(TAG, "GlContextReady");

    _configuration = GetConfiguration<configuration>();
    auto is_requested_ctx = IsRequestedContext(ctx_config);
    Log::D(TAG, "OnGlContextReady config: " + to_string(ctx_config)
        + " is requested ctx config: "
        + std::to_string(is_requested_ctx));

    Report(datum::create_ctx_info_message(is_requested_ctx,
                                          ctx_config.depth_bits));
    if (is_requested_ctx || _configuration.run_for_all_depth_precisions) {

      _depth_bits = ctx_config.depth_bits;

      // this is fairly arbitrary; we might want to see if we
      // can find values which might exploit weaknesses in the floating point
      // depth buffer representation
      _num_slices = _depth_bits * 3;
      _depth_clear_increment = 1.0F / static_cast<float>(_depth_bits * 7);
      _clear_color = rgb{0, 0, 255};

      CreateProgram();
      SetHeartbeatPeriod(100ms);

    } else {
      Log::D(TAG,
             "OnGlContextReady - did not receive requested depth precision, "
             "and not configured to run in others, terminating test.");
      Stop();
    }
  }

  void OnGlContextResized(int width, int height) override {
    BaseOperation::OnGlContextResized(width, height);
    CreatePattern(_num_slices);
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);

    if (_num_indices == 0) {
      return;
    }

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    vec3 cc = vec3(_clear_color.r, _clear_color.g, _clear_color.b) / 255.0F;
    glClearColor(cc.r, cc.g, cc.b, 1);
    glClearDepthf(_depth_clear_value);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // render pattern
    glUseProgram(_program);
    glBindVertexArray(_vbo_state);
    glDrawElements(GL_TRIANGLES, _num_indices, GL_UNSIGNED_SHORT, nullptr);

    if (_perform_test) {

      ReadPattern();

      // update _depth_clear_value
      _depth_clear_value -= _depth_clear_increment;
      if (_depth_clear_value <= 0) {
        _depth_clear_value = 1.0F;
      }

      // wait until next heartbeat
      _perform_test = false;
    }
  }

  void OnHeartbeat(Duration elapsed) override {
    _perform_test = true;
  }

 private:

  bool IsRequestedContext(GLContextConfig ctx_config) const {
    return ctx_config == RequiredGLContextConfiguration;
  }

  void ReadPattern() {
    glFinish();

    bool errors = false;
    for (auto const& s : _probes) {
      std::vector<GLubyte> storage(3, 0); // 3 elements initialized to 0
      glReadPixels(s.pos.x,
                   s.pos.y,
                   1,
                   1,
                   GL_RGB,
                   GL_UNSIGNED_BYTE,
                   storage.data());
      rgb value{storage[0], storage[1], storage[2]};

      if (_depth_clear_value >= s.depth) {
        if (value != s.written_value) {
          // fragment should have been drawn

          errors = true;
          Report(datum::create_error_incorrect_fragment_rejection(
              s.depth, _depth_clear_value, s.written_value, value));

          Log::D(TAG,
                 "fragment should have passed - pos: (%d,%d) expect: (%d,%d,%d) received: (%d,%d,%d)",
                 s.pos.x,
                 s.pos.y,
                 s.written_value.r,
                 s.written_value.g,
                 s.written_value.b,
                 value.r,
                 value.g,
                 value.b);
        }
      } else if (value != _clear_color) {
        // fragment should have been rejected

        errors = true;
        Report(datum::create_error_incorrect_fragment_pass(
            s.depth,_depth_clear_value, s.written_value, value));

        Log::D(TAG,
               "fragment should have been rejected - pos: (%d,%d) expect: (%d,%d,%d) received: (%d,%d,%d)",
               s.pos.x,
               s.pos.y,
               _clear_color.r,
               _clear_color.g,
               _clear_color.b,
               value.r,
               value.g,
               value.b);
      }
    }

    if (!errors) {
      // hooray
      Report(datum::create_success_message(_depth_clear_value));
    }
  }

  void CreateProgram() {
    auto vertex_file = "Shaders/DepthClearGLES3Operation/vert.vsh";
    auto fragment_file = "Shaders/DepthClearGLES3Operation/frag.fsh";
    _program = ::CreateProgram(vertex_file, fragment_file);

    if (!_program) {
      FatalError(TAG, "Unable to load quad program");
    }
  }

  void CreatePattern(int slices) {
    Cleanup();

    // NOTE: We're generating vertices in clip space (-1:1)

    const int width = GetGlContextSize().x;
    const int height = GetGlContextSize().y;
    const float col_width = 1.0F / slices;
    const float slice_depth = 1.0F / slices;
    const float slice_depth_offset = 0.5F / static_cast<float>(slices);

    const float z_near = -1;
    const float z_far = 1;

    std::vector<Vertex> vertices;
    std::vector<GLushort> indices;

    for (int i = 0; i < slices; i++) {
      float depth_n = i * slice_depth + slice_depth_offset;
      float z = z_near + depth_n * (z_far - z_near);
      float left = -1 + 2 * i * col_width;
      float right = left + 2 * col_width;
      float top = 1;
      float bottom = 0;
      auto a = vec3(left, bottom, z);
      auto b = vec3(left, top, z);
      auto c = vec3(right, top, z);
      auto d = vec3(right, bottom, z);
      auto color = rgb{static_cast<GLubyte>(depth_n * 255),
                       static_cast<GLubyte>(depth_n * 255),
                       static_cast<GLubyte>(depth_n * 255)};

      auto offset = vertices.size();

      vertices.push_back(Vertex{a, color});
      vertices.push_back(Vertex{b, color});
      vertices.push_back(Vertex{c, color});
      vertices.push_back(Vertex{d, color});

      indices.push_back(static_cast<unsigned short>(offset + 0));
      indices.push_back(static_cast<unsigned short>(offset + 1));
      indices.push_back(static_cast<unsigned short>(offset + 2));

      indices.push_back(static_cast<unsigned short>(offset + 0));
      indices.push_back(static_cast<unsigned short>(offset + 2));
      indices.push_back(static_cast<unsigned short>(offset + 3));

      // create a probe using the centroid of the quad
      // and truth values for testing
      float middle_x = (left + right) * 0.5F;
      float across_x = (middle_x + 1) * 0.5F; // remap from (-1,1)->(0,1)
      int middle_x_px = static_cast<int>(floorf(across_x * width));

      float middle_y = (top + bottom) * 0.5F;
      float across_y = (middle_y + 1) * 0.5F; // remap from (-1,1)->(0,1)
      int middle_y_px = static_cast<int>(floorf(across_y * height));

      _probes.push_back(Probe{
          ivec2(middle_x_px, middle_y_px),
          z,
          (z - z_near) / (z_far - z_near),
          color
      });
    }

    _num_indices = static_cast<GLsizei>(indices.size());

    glGenBuffers(1, &_vertex_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_vbo_id);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(Vertex) * vertices.size(),
        vertices.data(),
        GL_STREAM_DRAW);

    glGenBuffers(1, &_index_vbo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_vbo_id);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLushort) * indices.size(),
        indices.data(),
        GL_STATIC_DRAW);

    glh::CheckGlError("building buffers");

    glGenVertexArrays(1, &_vbo_state);
    glBindVertexArray(_vbo_state);

    glh::CheckGlError("binding vertex array");

    glVertexAttribPointer(
        static_cast<GLuint>(Attributes::Pos), 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (const GLvoid*) offsetof(Vertex, pos));

    glVertexAttribPointer(
        static_cast<GLuint>(Attributes::Color), 3, GL_UNSIGNED_BYTE, GL_TRUE,
        sizeof(Vertex),
        (const GLvoid*) offsetof(Vertex, color));

    glh::CheckGlError("setting attrib pointers");

    glEnableVertexAttribArray(static_cast<GLuint>(Attributes::Pos));
    glEnableVertexAttribArray(static_cast<GLuint>(Attributes::Color));

    glBindBuffer(GL_ARRAY_BUFFER, _vertex_vbo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_vbo_id);
  }

  void Cleanup() {
    if (_vbo_state > 0) glDeleteVertexArrays(1, &_vbo_state);
    if (_vertex_vbo_id > 0) glDeleteBuffers(1, &_vertex_vbo_id);
    if (_index_vbo_id > 0) glDeleteBuffers(1, &_index_vbo_id);
    _vbo_state = 0;
    _vertex_vbo_id = 0;
    _index_vbo_id = 0;
    _num_indices = 0;
    _probes.clear();
  }

 private:

  configuration _configuration;
  int _depth_bits = 0;
  int _num_slices = 0;
  float _depth_clear_value = 1.0F;
  float _depth_clear_increment = 0;
  bool _perform_test = false;

  std::vector<Probe> _probes;
  rgb _clear_color;

  // GL
  GLuint _program = 0;
  GLuint _vertex_vbo_id = 0;
  GLuint _index_vbo_id = 0;
  GLuint _vbo_state = 0;
  GLsizei _num_indices = 0;
};

EXPORT_ANCER_OPERATION(DepthClearGLES3Operation)