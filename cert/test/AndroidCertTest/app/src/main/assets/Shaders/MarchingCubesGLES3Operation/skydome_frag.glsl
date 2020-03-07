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


uniform samplerCube uSkyboxSampler;
in vec3 vRayDir;

out vec4 fragColor;

/////////////////////////////////////////////

void main() {
    vec3 rayDir = normalize(vRayDir);
    vec3 color = texture(uSkyboxSampler, rayDir).rgb;
    fragColor = vec4(color,1);
}
