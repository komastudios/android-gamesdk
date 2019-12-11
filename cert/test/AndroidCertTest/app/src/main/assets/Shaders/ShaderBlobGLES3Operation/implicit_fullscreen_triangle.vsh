#version 300 es

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

// Usage: Call glDrawArrays(GL_TRIANGLES, 0, 3) with no vertex arrays.
// Renders a full-screen triangle, clipped into a quad.

const vec2 points[3] = vec2[](
    vec2(-1.0, 3.0),
    vec2(-1.0, -1.0),
    vec2(3.0, -1.0)
);

void main() {
    gl_Position = vec4(points[gl_VertexID], 0.0, 1.0);
}
