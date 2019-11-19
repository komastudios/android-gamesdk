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

#pragma once

#include <GLES3/gl32.h>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp>


using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ivec2;
using glm::ivec3;
using glm::ivec4;
using glm::mat4;

namespace ancer::glh {

    namespace color {

        // cribbed from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

        struct rgb {
            float r;       // a fraction between 0 and 1
            float g;       // a fraction between 0 and 1
            float b;       // a fraction between 0 and 1

            rgb() :
                    r(0), g(0), b(0) {}

            rgb(float R, float G, float B) :
                    r(R), g(G), b(B) {}
        };

        struct hsv {
            float h;       // angle in degrees (0,360)
            float s;       // a fraction between 0 and 1
            float v;       // a fraction between 0 and 1

            hsv() :
                    h(0), s(0), v(0) {}

            hsv(float H, float S, float V) :
                    h(H), s(S), v(V) {}
        };

        hsv rgb2hsv(rgb in);
        rgb hsv2rgb(hsv in);
    } // namespace color

    bool CheckGlError(const char* func_name);

    GLuint CreateProgramSrc(const char* vtx_src, const char* frag_src);

    GLuint CreateShader(GLenum shader_type, const char* src);

    /**
     * Creates an ortho projection with (0,0) at top-left,
     * (+width,+height) at bottom right
     */
    glm::mat4 Ortho2d(float left, float top, float right, float bottom);
}  // namespace ancer::glh
