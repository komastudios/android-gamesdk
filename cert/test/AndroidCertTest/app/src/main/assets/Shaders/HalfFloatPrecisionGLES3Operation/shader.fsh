#version 300 es
out vec4 outColor;

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

uniform float offset;

void main()
{
    mediump vec3 startValue = vec3(1.0f, 0.0f, 0.0f);

    startValue.x += offset;
    startValue = normalize(startValue);

    outColor = vec4(startValue, 1.0);
}

