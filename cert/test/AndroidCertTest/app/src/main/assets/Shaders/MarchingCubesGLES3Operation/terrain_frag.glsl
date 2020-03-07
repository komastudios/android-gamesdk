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

in vec4 vColor;
in vec3 vWorldNormal;
in vec3 vWorldPosition;
in float vShininess;
in float vTex0Contribution;
in float vTex1Contribution;
in float vWorldDistance;

uniform vec3 uAmbientLight;
uniform vec3 uCameraPosition;
uniform float uRenderDistance;

out vec4 fragColor;

/////////////////////////////////////////////

void main()
{
    vec3 I = normalize(vWorldPosition - uCameraPosition);
    vec3 R = reflect(I, vWorldNormal);

    vec3 color = vWorldNormal * 0.5 + 0.5;
    vec3 reflectionColor = R * 0.5 + 0.5;

    color *= uAmbientLight;
    color = mix(color, reflectionColor, vShininess);

    float distFactor = vWorldDistance / uRenderDistance;
    float recolorFactor = distFactor * distFactor;
    float alpha = 1.0 - (recolorFactor * recolorFactor * recolorFactor);

    color =mix(color, vec3(1,0,1), recolorFactor);

    fragColor = vec4(color, alpha);
}
