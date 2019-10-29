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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-non-private-member-variables-in-classes"
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"
#pragma ide diagnostic ignored "misc-non-private-member-variables-in-classes"

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
    constexpr Log::Tag TAG{"StreamDrawGLES3Operation"};
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

    struct datum {
        float vertices_per_second = 0.0f;
        int components_per_vertex = 0;
    };

    JSON_WRITER(datum) {
        JSON_REQVAR(vertices_per_second);
    }
}

//==================================================================================================

namespace {

    struct Vertex {
        vec2 pos;
        vec2 tex_coord;
        vec4 rgba;
    };

    constexpr auto VertexComponents = 8;

    enum class Attributes : GLuint {
        Pos = 0,
        Color = 1,
        Texcoord = 2,
    };

    constexpr auto MAX_ROWS = 255;
    constexpr auto MAX_COLS = 255;


    class Renderer {
    public:

        Renderer(int rows, int cols, int left, int top, int width, int height) :
                _width(width), _height(height), _rows(std::min(rows, MAX_ROWS)), _cols(
                std::min(cols, MAX_COLS)) {
            //
            //  Build mesh
            //

            int n_verts_across = cols + 1;
            int n_verts_tall = rows + 1;

            for ( int row = 0 ; row < n_verts_tall ; row++ ) {
                const auto across_row = static_cast<float>(row) / rows;
                for ( int col = 0 ; col < n_verts_across ; col++ ) {
                    const auto across_col = static_cast<float>(col) / cols;

                    float x = left + width * across_col;
                    float y = top + height * across_row;

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

                    if ( row < rows && col < cols ) {
                        auto i = col + row * n_verts_across;

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
                ANCER_SCOPED_TRACE("StreamDrawGLES3Operation::Renderer::ctor");

                glGenBuffers(1, &_vertex_vbo_id);
                glBindBuffer(GL_ARRAY_BUFFER, _vertex_vbo_id);
                glBufferData(
                        GL_ARRAY_BUFFER, sizeof(Vertex) * _vertices.size(), nullptr,
                        GL_STREAM_DRAW);

                glGenBuffers(1, &_index_vbo_id);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_vbo_id);
                glBufferData(
                        GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * _indices.size(),
                        &_indices[0],
                        GL_STATIC_DRAW);

                glh::CheckGlError("building buffers");

                glGenVertexArrays(1, &_vbo_state);
                glBindVertexArray(_vbo_state);

                glh::CheckGlError("binding vertex array");

                glVertexAttribPointer(
                        static_cast<GLuint>(Attributes::Pos), 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex),
                        (const GLvoid*)offsetof(Vertex, pos));

                glVertexAttribPointer(
                        static_cast<GLuint>(Attributes::Texcoord), 2, GL_FLOAT,
                        GL_TRUE,
                        sizeof(Vertex),
                        (const GLvoid*)offsetof(Vertex, tex_coord));

                glVertexAttribPointer(
                        static_cast<GLuint>(Attributes::Color), 4, GL_FLOAT, GL_TRUE,
                        sizeof(Vertex),
                        (const GLvoid*)offsetof(Vertex, rgba));

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

        ~Renderer() {
            if ( _vbo_state > 0 ) glDeleteVertexArrays(1, &_vbo_state);
            if ( _vertex_vbo_id > 0 ) glDeleteBuffers(1, &_vertex_vbo_id);
            if ( _index_vbo_id > 0 ) glDeleteBuffers(1, &_index_vbo_id);
        }

        void Step(double delta_t) {
            ANCER_SCOPED_TRACE("StreamDrawGLES3Operation::Renderer::step");

            _time += delta_t;

            auto vertices = MapVerticesBuffer();
            auto vertex_iterator = vertices;
            size_t i = 0;
            const double min_dim = std::min(_width, _height);
            const double r = min_dim / 64;
            const double b = M_PI * 0.0125;
            const double c = M_PI * 0.025;
            const int verts_per_row = _cols + 1;

            // TODO: Be smarter about how I modulate vertices; try to modify the _vertices store to reduce copies
            for ( const auto& v : _vertices ) {
                int row = i / verts_per_row;
                int col = static_cast<int>(floor(i - (row * verts_per_row)));

                auto a = _time * M_PI * 4 + row * b - col * c;
                auto dx = static_cast<float>(std::cos(a) * r);
                auto dy = static_cast<float>(std::sin(a) * r);

                auto vv = v;
                vv.pos += vec2(dx, dy);

                *vertex_iterator = vv;
                vertex_iterator++;
                i++;
            }

            // modulate vertex positions
            UnmapVerticesBuffer();
        }

        void Draw() {
            glBindVertexArray(_vbo_state);
            glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_SHORT, nullptr);
        }

        size_t VertexCount() const { return _vertices.size(); }

    private:


        Vertex* MapVerticesBuffer() {
            glBindBuffer(GL_ARRAY_BUFFER, _vertex_vbo_id);
            return static_cast<Vertex*>(glMapBufferRange(
                    GL_ARRAY_BUFFER,
                    0, _vertices.size() * sizeof(Vertex),
                    GL_MAP_WRITE_BIT |
                            GL_MAP_INVALIDATE_BUFFER_BIT));
        }

        void UnmapVerticesBuffer() {
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }


    private:

        GLuint _vertex_vbo_id = 0;
        GLuint _index_vbo_id = 0;
        GLuint _vbo_state = 0;

        // internal state
        int _width, _height, _rows, _cols;
        double _time = 0;
        std::vector<Vertex> _vertices;
        std::vector<GLushort> _indices;
    };
}

class StreamDrawGLES3Operation : public BaseGLES3Operation {
public:

    StreamDrawGLES3Operation() = default;

    ~StreamDrawGLES3Operation() override {
        if ( eglGetCurrentContext() == _egl_context ) {
            glDeleteProgram(_program);
            glDeleteTextures(1, &_tex_id);
        }
    }

    void OnGlContextReady() override {
        SetHeartbeatPeriod(250ms);

        _configuration = GetConfiguration<configuration>();
        _layout_count = _configuration.count;

        Log::I(
                TAG, "glContextReady, configuration: %s - loading shaders, textures, etc",
                Json(_configuration).dump().c_str());

        _egl_context = eglGetCurrentContext();
        if ( _egl_context == EGL_NO_CONTEXT) {
            FatalError(TAG, "No EGL context available");
        }

        int tex_width = 0;
        int tex_height = 0;
        _tex_id = LoadTexture(
                "Textures/sphinx.png",
                &tex_width, &tex_height, nullptr);
        if ( tex_width == 0 || tex_height == 0 ) {
            FatalError(TAG, "Unable to load texture");
        }

        glDisable(GL_BLEND);

        //
        //  Build the shader program and get uniform locations
        //

        auto vertex_file = "Shaders/StreamDrawGLES3Operation/simple.vsh";
        auto fragment_file = "Shaders/StreamDrawGLES3Operation/simple.fsh";
        _program = CreateProgram(vertex_file, fragment_file);

        if ( !_program ) {
            FatalError(TAG, "Unable to load quad program");
        }

        _tex_id_uniform_loc = glGetUniformLocation(_program, "uTex");
        glh::CheckGlError("looking up uTex");

        _projection_uniform_loc = glGetUniformLocation(_program, "uProjection");
        glh::CheckGlError("looking up uProjection");

        _context_ready = true;
    }

    void Draw(double delta_seconds) override {
        BaseGLES3Operation::Draw(delta_seconds);

        glUseProgram(_program);

        glUniformMatrix4fv(_projection_uniform_loc, 1, GL_FALSE, glm::value_ptr(_projection));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _tex_id);
        glUniform1i(_tex_id_uniform_loc, 0);

        for ( const auto& r : _renderers ) {
            r->Step(delta_seconds);
            r->Draw();
        }
    }

    void OnGlContextResized(int width, int height) override {
        BaseGLES3Operation::OnGlContextResized(width, height);

        _projection = glh::Ortho2d(0, 0, width, height);

        if ( _context_ready ) {
            BuildRenderers();
        }
    }

    void OnHeartbeat(Duration elapsed) override {
        if ( GetMode() == Mode::DataGatherer ) {
            RecordPerfDatum(duration_cast<SecondsAs<float>>(elapsed));
        }

        _heartbeat_time += elapsed;
        if ((_configuration.increment.increment > 0) &&
                (_configuration.increment.period > Duration::zero()) &&
                (_heartbeat_time >= _configuration.increment.period)) {
            _heartbeat_time -= _configuration.increment.period;
            _layout_count += _configuration.increment.increment;

            GetFpsCalculator().Ignore([this]() { BuildRenderers(); });
        }
    }

private:

    void RecordPerfDatum(SecondsAs<float> elapsed_seconds) {
        size_t total_vertices = 0;
        for ( const auto& r : _renderers ) { total_vertices += r->VertexCount(); }
        auto vps = total_vertices / elapsed_seconds.count();

        Report(datum{vps, VertexComponents});
    }

    void BuildRenderers() {
        _renderers.clear();

        while ( _layout_count > _layout_rows * _layout_cols ) {
            _layout_rows++;
            _layout_cols++;
        }

        auto size = GetGlContextSize();
        int col_width = size.x / _layout_cols;
        int row_height = size.y / _layout_rows;
        int inset = 16;

        for ( int i = 0 ; i < _layout_count ; i++ ) {
            int row = i / _layout_cols;
            int col = i - (row * _layout_cols);

            int left = col * col_width + inset;
            int top = row * row_height + inset;
            int width = col_width - 2 * inset;
            int height = row_height - 2 * inset;

            _renderers.push_back(
                    std::make_shared<Renderer>(
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
    std::vector<std::shared_ptr<Renderer>> _renderers;
    int _layout_rows = 1;
    int _layout_cols = 1;
    int _layout_count = 1;
};

EXPORT_ANCER_OPERATION(StreamDrawGLES3Operation)

#pragma clang diagnostic pop
