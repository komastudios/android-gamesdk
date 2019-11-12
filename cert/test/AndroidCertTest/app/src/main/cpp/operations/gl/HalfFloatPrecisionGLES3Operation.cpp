//
// Created by Ganesa Chinnathambi on 2019-10-29.
//
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
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"
#pragma ide diagnostic ignored "misc-non-private-member-variables-in-classes"

#include <cmath>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/util/Json.hpp>
#include <ancer/System.hpp>
#include <iostream>

#include <list>

using namespace ancer;


//==============================================================================

namespace {
    constexpr Log::Tag TAG{"HalfFloatPrecisionGLES3Operation"};
}

//==============================================================================

namespace {
struct configuration {
};

JSON_CONVERTER(configuration) {
}

//------------------------------------------------------------------------------

struct half_precision {
    bool is_correct;
    int offset;
    int expected_r;
    int expected_g;
    int expected_b;
    int expected_a;

    int actual_r;
    int actual_g;
    int actual_b;
    int actual_a;
};

struct datum {
    half_precision half_precision;
};

JSON_WRITER(half_precision) {
    JSON_REQVAR(is_correct);
    JSON_REQVAR(offset);
    JSON_OPTVAR(expected_r);
    JSON_OPTVAR(expected_g);
    JSON_OPTVAR(expected_b);
    JSON_OPTVAR(expected_a);
    JSON_OPTVAR(actual_r);
    JSON_OPTVAR(actual_g);
    JSON_OPTVAR(actual_b);
    JSON_OPTVAR(actual_a);
}

JSON_WRITER(datum) {
    JSON_REQVAR(half_precision);
}
}

//==============================================================================

class HalfFloatPrecisionGLES3Operation : public BaseGLES3Operation {
 public:

    HalfFloatPrecisionGLES3Operation() = default;

    void OnGlContextReady() override {
        Log::D(TAG, "GlContextReady");
        _configuration = GetConfiguration<configuration>();

        SetHeartbeatPeriod(500ms);

        // Create and compile our GLSL program from the shaders

        auto vertex_file =
                "Shaders/HalfFloatPrecisionGLES3Operation/shader.vsh";

        auto fragment_file =
              "Shaders/HalfFloatPrecisionGLES3Operation/shader.fsh";

        _program_id = CreateProgram(vertex_file, fragment_file);
        glh::CheckGlError("created program");

        _offset_uniform_id = glGetUniformLocation(_program_id, "offset");
        if (_offset_uniform_id < 0) {
            Log::E(TAG, "Unable to find \"offset\" uniform in program");
        }
        glh::CheckGlError("looked up offset");

        const float vertex_length = 0.9f;
        const GLfloat g_vertex_buffer_data[] = {
            -vertex_length, +vertex_length, 0,
            -vertex_length, -vertex_length, 0,
            +vertex_length, +vertex_length, 0,
            +vertex_length, -vertex_length, 0
        };

        //
        glGenBuffers(1, &_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
        glBufferData(
            GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data),
            g_vertex_buffer_data, GL_STATIC_DRAW);
        glh::CheckGlError("created vertex buffer");

        glGenVertexArrays(1, &_vertex_array_id);
        glBindVertexArray(_vertex_array_id);
        glh::CheckGlError("created vertex array");

        // 1rst attribute buffer : vertices
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0,
            // but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            nullptr            // array buffer offset
        );
        glEnableVertexAttribArray(0);

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    }

    void Draw(double delta_seconds) override {
        BaseGLES3Operation::Draw(delta_seconds);
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(_program_id);
        glUniform1f(_offset_uniform_id, static_cast<GLfloat>(_color_offset));

        // Draw  Square !
        glBindVertexArray(_vertex_array_id);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // 4 vertices Quad
        auto size = GetGlContextSize();
        int col_width = size.x;
        int row_height = size.y;

        if (_did_update_color_offset) {
          getPixels(col_width, row_height);
          _did_update_color_offset = false;
        }
    }

    void getPixels(int x, int y) {
        // TODO(shamyl@google.com) Handle R8G8B8A8 contexts if needed
        std::vector<unsigned char> data(static_cast<unsigned long>(3 * x * y));

        glFinish();
        glReadPixels
            (
                0, 0,
                x, y,
                GL_RGB, GL_UNSIGNED_BYTE, &data[0]
            );

        int position = (x * y / 2) + (x / 3);
        std::array<int, 3> actualColor = {data[position], data[position + 1],
                                          data[position + 2]};
        std::array<int, 3> expectedColor = {255, 0, 0};

        bool is_expected =
            actualColor[0] == expectedColor[0] &&
                actualColor[1] == expectedColor[1] &&
                actualColor[2] == expectedColor[2];

        half_precision results{
            is_expected,
            _color_offset,
            expectedColor[0], expectedColor[1], expectedColor[2],
            0, actualColor[0], actualColor[1],
            actualColor[2], 0
        };
        Report(datum{results});

        if (!is_expected) {
            Log::D(
                TAG, "Vector is not normalized expected: offset: %d "
                     "(%d,%d,%d,%d) received: (%d,%d,%d,%d)",
                _color_offset,
                expectedColor[0], expectedColor[1],
                expectedColor[2], 0,
                actualColor[0], actualColor[1],
                actualColor[2], 0
            );
        }

        glh::CheckGlError("check read pixels");
    }

    void OnHeartbeat(Duration elapsed) override {
        /*Increment offset value gradually and reset to 0 if _color_offset
           reaches MAX_COLOR*/
        _color_offset += 16;
        if (_color_offset > MAX_COLOR) { _color_offset = 0; }
        _did_update_color_offset = true;
    }

 private:
    configuration _configuration;
    GLuint _vertex_array_id = 0;
    GLuint _vertex_buffer = 0;
    GLuint _program_id = 0;
    GLint _offset_uniform_id = -1;
    int _color_offset = 0;
    bool _did_update_color_offset = false;
    int MAX_COLOR = 255;
};

EXPORT_ANCER_OPERATION(HalfFloatPrecisionGLES3Operation)

#pragma clang diagnostic pop

