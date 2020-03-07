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

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inTriangleNormal;
layout(location = 3) in float inShininess;
layout(location = 4) in float inTex0Contribution;
layout(location = 5) in float inTex1Contribution;


uniform mat4 uMVP;
uniform mat4 uModel;
uniform mediump vec3 uCameraPosition;

out vec4 vColor;
out vec3 vWorldNormal;
out vec3 vWorldPosition;
out float vShininess;
out float vTex0Contribution;
out float vTex1Contribution;
out float vWorldDistance;

void main()
{
    gl_Position = uMVP * vec4(inPosition, 1.0);

    vColor = inColor;
    vWorldNormal = mat3(uModel) * inTriangleNormal;
    vWorldPosition = vec3(uModel * vec4(inPosition, 1.0));
    vShininess = inShininess;
    vTex0Contribution = inTex0Contribution;
    vTex1Contribution = inTex1Contribution;
    vWorldDistance = distance(vWorldPosition, uCameraPosition);
}
