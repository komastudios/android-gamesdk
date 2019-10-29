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

#include "GLHelpers.hpp"
#include "Error.hpp"
#include "Log.hpp"


namespace ancer::glh {
    namespace {
        constexpr Log::Tag TAG{"GLHelpers"};
    }


    namespace color {

        hsv rgb2hsv(rgb in) {
            hsv out;
            float min, max, delta;

            min = in.r < in.g ? in.r : in.g;
            min = min < in.b ? min : in.b;

            max = in.r > in.g ? in.r : in.g;
            max = max > in.b ? max : in.b;

            out.v = max;                                // v
            delta = max - min;
            if ( delta < 0.00001F ) {
                out.s = 0;
                out.h = 0; // undefined, maybe nan?
                return out;
            }
            if ( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
                out.s = (delta / max);                  // s
            } else {
                // if max is 0, then r = g = b = 0
                // s = 0, h is undefined
                out.s = 0.0;
                out.h = NAN;                            // its now undefined
                return out;
            }
            if ( in.r >= max )                           // > is bogus, just keeps compilor happy
                out.h = (in.g - in.b) / delta;        // between yellow & magenta
            else if ( in.g >= max )
                out.h = 2.0F + (in.b - in.r) / delta;  // between cyan & yellow
            else
                out.h = 4.0F + (in.r - in.g) / delta;  // between magenta & cyan

            out.h *= 60.0F;                              // degrees

            if ( out.h < 0.0 )
                out.h += 360.0F;

            return out;
        }


        rgb hsv2rgb(hsv in) {
            float hh, p, q, t, ff;
            long i;
            rgb out;

            if ( in.s <= 0.0 ) {       // < is bogus, just shuts up warnings
                out.r = in.v;
                out.g = in.v;
                out.b = in.v;
                return out;
            }
            hh = in.h;
            if ( hh >= 360.0F ) hh = 0.0;
            hh /= 60.0F;
            i = (long)hh;
            ff = hh - i;
            p = in.v * (1.0F - in.s);
            q = in.v * (1.0F - (in.s * ff));
            t = in.v * (1.0F - (in.s * (1.0F - ff)));

            switch ( i ) {
            case 0:out.r = in.v;
                out.g = t;
                out.b = p;
                break;
            case 1:out.r = q;
                out.g = in.v;
                out.b = p;
                break;
            case 2:out.r = p;
                out.g = in.v;
                out.b = t;
                break;

            case 3:out.r = p;
                out.g = q;
                out.b = in.v;
                break;
            case 4:out.r = t;
                out.g = p;
                out.b = in.v;
                break;
            case 5:
            default:out.r = in.v;
                out.g = p;
                out.b = q;
                break;
            }
            return out;
        }
    }

    bool CheckGlError(const char* func_name) {
#ifndef NDEBUG
        GLint err = glGetError();
        if ( err != GL_NO_ERROR ) {
            // TODO: Need to review if this should be fatal or not.
            Log::E(TAG, "GL error after %s(): 0x%08x\n", func_name, err);
            return true;
        }
        return false;
#endif
        return false;
    }

    GLuint CreateProgramSrc(const char* vtx_src, const char* frag_src) {
        GLuint vtx_shader = 0;
        GLuint frag_shader = 0;
        GLuint program = 0;
        GLint linked = GL_FALSE;

        vtx_shader = CreateShader(GL_VERTEX_SHADER, vtx_src);
        if ( !vtx_shader )
            goto exit;

        frag_shader = CreateShader(GL_FRAGMENT_SHADER, frag_src);
        if ( !frag_shader )
            goto exit;

        program = glCreateProgram();
        if ( !program ) {
            CheckGlError("glCreateProgram");
            goto exit;
        }
        glAttachShader(program, vtx_shader);
        glAttachShader(program, frag_shader);

        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if ( !linked ) {
            GLint info_log_len = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len);
            if ( info_log_len ) {
                auto* info_log = (GLchar*)malloc(static_cast<size_t>(info_log_len));
                if ( info_log ) {
                    glGetProgramInfoLog(program, info_log_len, nullptr, info_log);
                    FatalError(TAG, "Could not link program:\n%s\n", info_log);
                    free(info_log);
                }
            }
            glDeleteProgram(program);
            program = 0;
            FatalError(TAG, "Could not link program");
        }

        exit:
        glDeleteShader(vtx_shader);
        glDeleteShader(frag_shader);
        return program;
    }

    GLuint CreateShader(GLenum shader_type, const char* src) {
        GLuint shader = glCreateShader(shader_type);
        if ( !shader ) {
            CheckGlError("glCreateShader");
            return 0;
        }
        glShaderSource(shader, 1, &src, nullptr);

        GLint compiled = GL_FALSE;
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if ( !compiled ) {
            GLint info_log_len = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_len);
            if ( info_log_len > 0 ) {
                auto* info_log = (GLchar*)malloc(static_cast<size_t>(info_log_len));
                if ( info_log ) {
                    glGetShaderInfoLog(shader, info_log_len, nullptr, info_log);
                    // TODO: Need to review if this should be fatal or not.
                    Log::E(
                            TAG, "Could not compile %s shader:\n%s\n",
                            shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment",
                            info_log);
                    free(info_log);
                }
            }
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    glm::mat4 Ortho2d(float left, float top, float right, float bottom) {
        // this was cribbed from elsewhere because glm's ortho function proved hard to use
        const float z_near = -1.0F;
        const float z_far = 1.0F;
        const float inv_z = 1.0F / (z_far - z_near);
        const float inv_y = 1.0F / (-top + bottom);
        const float inv_x = 1.0F / (right - left);

        glm::mat4 result;
        result[0] = glm::vec4(2 * inv_x, 0, 0, 0);
        result[1] = glm::vec4(0, 2 * inv_y, 0, 0);
        result[2] = glm::vec4(0, 0, -2 * inv_z, 0);
        result[3] = glm::vec4(
                -(right + left) * inv_x,
                (top + bottom) * inv_y,
                -(z_far + z_near) * inv_z,
                1
        );

        return result * glm::scale(glm::mat4(1), glm::vec3(1, -1, 1));
    }
}