/*
 * Copyright (c) 2015-2016 The Khronos Group Inc.
 * Copyright (c) 2015-2016 Valve Corporation
 * Copyright (c) 2015-2016 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * Vertex shader used by Cube demo.
 */
#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout(std140, binding = 0) uniform buf {
        mat4 MVP;
        vec4 position[12*3];
        vec4 attr[12*3];
        highp int numPerRow;
        highp int numRows;
} ubuf;

layout (location = 0) out vec4 texcoord;
layout (location = 1) out vec3 frag_pos;

const int verticesPerModel = 12 * 3;

void main()
{
   const int vertexIndex = gl_VertexIndex % verticesPerModel;
   texcoord = ubuf.attr[vertexIndex];
   gl_Position = ubuf.MVP * ubuf.position[vertexIndex];

    int count = 0;
    for(int i = 0; i < ubuf.numPerRow*ubuf.numRows; ++i) {
      texcoord += vec4(0.0001);
    }

   const int modelIndex = gl_VertexIndex / verticesPerModel;
   const float gridIndexX = (modelIndex % ubuf.numPerRow) - (ubuf.numPerRow >> 1);
   const float gridIndexY = (modelIndex / ubuf.numPerRow) - (ubuf.numRows >> 1);
   const float distance = 0.0;//10.0 / ubuf.numPerRow;
   gl_Position.xy += vec2(gridIndexX, gridIndexY) * distance;
   frag_pos = gl_Position.xyz;
}
