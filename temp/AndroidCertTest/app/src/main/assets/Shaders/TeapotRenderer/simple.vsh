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

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uView;

out vec3 vNormalWorld;

/**
This is really simple - no lighting, no effects. We do, however,
encode the world-space normal for each vertex to colorize output
*/
void main() {
    mat4 mv = uView * uModel;
    vec4 vertex = mv * vec4(pos, 1);
    gl_Position = uProjection * vertex;
    vNormalWorld = normalize(mat3(uModel) * normal);
}
