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
#include <utility>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Error.hpp>
#include <ancer/util/Json.hpp>

// raw teapot vertex/index/etc data
#include "Teapot.inl"

using namespace ancer;


//==================================================================================================

namespace {
    constexpr Log::Tag TAG{"TeapotRendererGLES3Operation"};
}

//==================================================================================================

namespace {
    struct configuration {
        int count = 0;
        int min_fps_threshold = 0;
    };

    JSON_READER(configuration) {
        JSON_REQVAR(count);
        JSON_REQVAR(min_fps_threshold);
    }

//--------------------------------------------------------------------------------------------------

    struct datum {
        int model_count = 0;
    };

    JSON_WRITER(datum) {
        JSON_REQVAR(model_count);
    }
}

//==================================================================================================

struct ProgramState {
    GLuint program = 0;
    GLint projection_mat_4 = 0;
    GLint model_mat4 = 0;
    GLint view_mat4 = 0;
    GLint color_mat4 = 0;

    bool Init(GLuint program) {
        this->program = program;

        projection_mat_4 = glGetUniformLocation(program, "uProjection");
        glh::CheckGlError("looking up uProjection");

        model_mat4 = glGetUniformLocation(program, "uModel");
        glh::CheckGlError("looking up uModel");

        view_mat4 = glGetUniformLocation(program, "uView");
        glh::CheckGlError("looking up uView");

        color_mat4 = glGetUniformLocation(program, "uColor");
        glh::CheckGlError("looking up uColor");

        return (projection_mat_4 >= 0 && model_mat4 >= 0 &&
                view_mat4 >= 0 && color_mat4 >= 0);
    }
};

class TeapotRenderer {
public:

    struct Vertex {
        vec3 pos;
        vec3 normal;
    };

    enum Attrs {
        Pos,
        Normal
    };

    struct AABB {
        vec3 min = {
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max()
        };

        vec3 max = {
                std::numeric_limits<float>::min(),
                std::numeric_limits<float>::min(),
                std::numeric_limits<float>::min()
        };

        AABB() = default;

        void intersect(const vec3& v) {
            min.x = std::min(v.x, min.x);
            min.y = std::min(v.y, min.y);
            min.z = std::min(v.y, min.y);

            max.x = std::max(v.x, max.x);
            max.y = std::max(v.y, max.y);
            max.z = std::max(v.y, max.y);
        }
    };

public:

    TeapotRenderer() {
        ANCER_SCOPED_TRACE("TeapotRenderer::ctor");

        _num_indices = sizeof(teapot::indices) / sizeof(teapot::indices[0]);
        glGenBuffers(1, &_index_vbo_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_vbo_id);
        glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, sizeof(teapot::indices), teapot::indices,
                GL_STATIC_DRAW);


        _num_vertices = (sizeof(teapot::positions) / sizeof(teapot::positions[0])) / 3;
        int stride = sizeof(Vertex);
        int index = 0;


        auto positions = new Vertex[_num_vertices];
        for ( int32_t i = 0 ; i < _num_vertices ; ++i ) {
            positions[i].pos = vec3(
                    teapot::positions[index], teapot::positions[index + 1],
                    teapot::positions[index + 2]);
            positions[i].normal = vec3(
                    teapot::normals[index], teapot::normals[index + 1],
                    teapot::normals[index + 2]);
            _bounds.intersect(positions[i].pos);
            index += 3;
        }

        glGenBuffers(1, &_vertex_vbo_id);
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, stride * _num_vertices, positions, GL_STATIC_DRAW);

        delete[] positions;


        glh::CheckGlError("building buffers");

        glGenVertexArrays(1, &_vbo_state);
        glBindVertexArray(_vbo_state);

        glh::CheckGlError("binding vertex array");

        glVertexAttribPointer(
                static_cast<GLuint>(Attrs::Pos), 3, GL_FLOAT, GL_FALSE,
                sizeof(Vertex),
                (const GLvoid*)offsetof(Vertex, pos));

        glVertexAttribPointer(
                static_cast<GLuint>(Attrs::Normal), 3, GL_FLOAT, GL_FALSE,
                sizeof(Vertex),
                (const GLvoid*)offsetof(Vertex, normal));

        glh::CheckGlError("setting attrib pointers");

        glEnableVertexAttribArray(static_cast<GLuint>(Attrs::Pos));
        glEnableVertexAttribArray(static_cast<GLuint>(Attrs::Normal));

        glBindBuffer(GL_ARRAY_BUFFER, _vertex_vbo_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_vbo_id);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glBindVertexArray(0);
    }

    ~TeapotRenderer() {
        if ( _vbo_state > 0 ) glDeleteVertexArrays(1, &_vbo_state);
        if ( _vertex_vbo_id > 0 ) glDeleteBuffers(1, &_vertex_vbo_id);
        if ( _index_vbo_id > 0 ) glDeleteBuffers(1, &_index_vbo_id);
    }

    const AABB& GetObjectSpaceBounds() const { return _bounds; }

    void Render(const ProgramState& program, mat4& model, vec4& color) {
        ANCER_SCOPED_TRACE("TeapotRenderer::draw");

        glh::CheckGlError("render() - Start");
        glUniformMatrix4fv(program.model_mat4, 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(program.color_mat4, 1, glm::value_ptr(color));

        glBindVertexArray(_vbo_state);
        glDrawElements(GL_TRIANGLES, _num_indices, GL_UNSIGNED_SHORT, nullptr);
        glh::CheckGlError("render() - end");
    }

private:

    GLuint _vertex_vbo_id = 0;
    GLuint _index_vbo_id = 0;
    GLuint _vbo_state = 0;

    int _num_indices = 0;
    int _num_vertices = 0;
    AABB _bounds;
};

class TeapotRendererGLES3Operation : public BaseGLES3Operation {
public:

    TeapotRendererGLES3Operation() = default;

    void OnGlContextReady(const GLContextConfig &ctx_config) override {
        _configuration = GetConfiguration<configuration>();

        if ( GetMode() == Mode::DataGatherer ) {
            SetHeartbeatPeriod(1000ms);
        }

        _egl_context = eglGetCurrentContext();
        if ( _egl_context == EGL_NO_CONTEXT) {
            FatalError(TAG, "No EGL context available");
        }

        auto vertex_file = "Shaders/TeapotRenderer/simple.vsh";
        auto fragment_file = "Shaders/TeapotRenderer/simple.fsh";
        auto program = CreateProgram(vertex_file, fragment_file);

        if ( !program ) {
            FatalError(TAG, "Unable to load shaders");
        }

        if ( !_program.Init(program)) {
            FatalError(TAG, "Unable to init program state");
        }

        _renderer = std::make_unique<TeapotRenderer>();

        auto bounds = _renderer->GetObjectSpaceBounds();

        _approx_teapot_radius = glm::length(bounds.max - bounds.min);
        _approx_teapot_radius *= 0.35F;

        // determine size of x,y,z cube that would hold _configuration.count teapots
        _layout_cube_size = static_cast<int>(std::ceil(
                std::pow(static_cast<float>(_configuration.count), 1.0F / 3.0F)));

        // assign a random rotation axis for each teapot
        unsigned int range = 100;
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, range);

        for ( int i = 0 ; i < _configuration.count ; i++ ) {
            auto v = (vec3(dist(rng), dist(rng), dist(rng)) / vec3(range, range, range)) *
                    vec3(2, 2, 2) - vec3(1, 1, 1);
            _rotationAxes.push_back(glm::normalize(v));
        }
    }

    void OnGlContextResized(int width, int height) override {
        BaseGLES3Operation::OnGlContextResized(width, height);

        const float near = 1;
        const float far = 1000;
        const float FOV = glm::radians(70.0F);
        if ( width < height ) {
            auto aspect = static_cast<float>(width) / static_cast<float>(height);
            _projection = glm::perspective(FOV, aspect, near, far);
        } else {
            auto aspect = static_cast<float>(height) / static_cast<float>(width);
            _projection = glm::perspective(FOV, aspect, near, far);
        }
    }

    void Draw(double delta_seconds) override {
        BaseGLES3Operation::Draw(delta_seconds);

        _angle += static_cast<float>(M_PI * 0.1 * delta_seconds);
        _camera_angle += static_cast<float>(M_PI * 0.25 * delta_seconds);

        auto camera_distance = _layout_cube_size * _approx_teapot_radius * 2;
        auto camera_pos = vec3(cos(_camera_angle), 0, sin(_camera_angle)) * camera_distance;
        _view = glm::lookAt(camera_pos, vec3(0, 0, 0), vec3(0, 1, 0));

        glUseProgram(_program.program);
        glUniformMatrix4fv(
                _program.projection_mat_4, 1, GL_FALSE,
                glm::value_ptr(_projection));
        glUniformMatrix4fv(_program.view_mat4, 1, GL_FALSE, glm::value_ptr(_view));

        auto color = vec4(1, 1, 1, 1);
        float step_size = 2 * _approx_teapot_radius;
        int i = 0;
        mat4 R;
        auto offset =
                vec3(_layout_cube_size, _layout_cube_size, _layout_cube_size) * (-step_size / 2) +
                        vec3(step_size / 2, step_size / 2, step_size / 2);
        for ( int z = 0 ; z < _layout_cube_size ; z++ ) {
            for ( int y = 0 ; y < _layout_cube_size ; y++ ) {
                for ( int x = 0 ; x < _layout_cube_size ; x++, i++ ) {
                    if ( i < _configuration.count ) {
                        auto q = glm::angleAxis(_angle, _rotationAxes[i]);
                        R = glm::toMat4(q);
                        auto position = vec3(x * step_size, y * step_size, z * step_size) + offset;
                        auto model = glm::translate(glm::mat4(1), position) * R;
                        _renderer->Render(_program, model, color);
                    } else {
                        // we're done!
                        return;
                    }
                }
            }
        }
    }

    void OnHeartbeat(Duration elapsed) override {
        Report(datum{_configuration.count});
    }

protected:

    configuration _configuration;

    EGLContext _egl_context = nullptr;
    ProgramState _program;
    mat4 _projection;
    mat4 _view;
    float _angle = 0;
    float _camera_angle = 0;
    int _layout_cube_size = 0;
    float _approx_teapot_radius = 0;

    std::unique_ptr<TeapotRenderer> _renderer;
    std::vector<vec3> _rotationAxes;
};

EXPORT_ANCER_OPERATION(TeapotRendererGLES3Operation);