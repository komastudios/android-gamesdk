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
in vec3 vModelPosition;
in vec3 vWorldNormal;
in vec3 vWorldPosition;
in float vShininess;
in float vTex0Contribution;
in float vTex1Contribution;
in float vWorldDistance;

uniform vec3 uAmbientLight;
uniform vec3 uCameraPosition;
uniform float uRenderDistance;
uniform sampler2D uTexture0Sampler;
uniform float uTexture0Scale;
uniform sampler2D uTexture1Sampler;
uniform float uTexture1Scale;
uniform samplerCube uLightProbeSampler;
uniform samplerCube uReflectionMapSampler;
uniform float uReflectionMapMipLevels;


out vec4 fragColor;

/////////////////////////////////////////////

vec4 PlanarTextureXZ(sampler2D sampler, float scale)
{
    vec2 UV = vModelPosition.xz / scale;
    return texture(sampler, UV);
}

vec4 PlanarTextureXY(sampler2D sampler, float scale)
{
    vec2 UV = vModelPosition.xy / scale;
    return texture(sampler, UV);
}

void main()
{
    vec3 I = normalize(vWorldPosition - uCameraPosition);
    vec3 R = reflect(I, vWorldNormal);

    float shininess = vShininess;
    float mipLevel = mix(uReflectionMapMipLevels, 0.0, shininess);
    vec3 reflectionColor = textureLod(uReflectionMapSampler, R, mipLevel).rgb;
    vec3 lightProbeLight = texture(uLightProbeSampler, vWorldNormal).rgb;

    vec3 diffuse0 = mix(vColor, vColor * PlanarTextureXZ(uTexture0Sampler, uTexture0Scale), vTex0Contribution).rgb;
    vec3 diffuse1 = mix(vColor, vColor * PlanarTextureXY(uTexture1Sampler, uTexture1Scale), vTex1Contribution).rgb;
    vec3 color = uAmbientLight + (lightProbeLight * (0.5 * (diffuse0 + diffuse1)));

    color = mix(color, reflectionColor, shininess);
    fragColor = vec4(color, 1.0);

    float distFactor = vWorldDistance / uRenderDistance;
    float recolorFactor = distFactor * distFactor;
    float alpha = 1.0 - (recolorFactor * recolorFactor * recolorFactor);

    color = mix(color, lightProbeLight, recolorFactor);

    fragColor = vec4(color, alpha);
}
