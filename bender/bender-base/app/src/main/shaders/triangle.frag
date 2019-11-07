#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "shader_bindings.h"

struct PointLight {
    float intensity;
    vec3 position;
    vec3 color;
};

struct AmbientLight {
    float intensity;
    vec3 color;
};

layout(set = BINDING_SET_MATERIAL, binding = FRAGMENT_BINDING_SAMPLER)
uniform sampler2D texSampler;

layout(set = BINDING_SET_LIGHTS, binding = FRAGMENT_BINDING_LIGHTS)
uniform LightBlock {
    PointLight pointLight;
    AmbientLight ambientLight;
    vec3 cameraPos;
} lightBlock;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 calcPointLight(PointLight light){
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 viewDir = normalize(lightBlock.cameraPos - fragPos);
    vec3 halfwayDir = normalize(lightDir +  viewDir);

    vec3 reflectDir = reflect(-lightDir, fragNormal);
    float dist = length(light.position - fragPos);
    float attenuation = light.intensity / dist;

    float diffuseTerm = max(dot(lightDir, fragNormal), 0.0);
    vec3 diffuse = diffuseTerm * light.color * attenuation;
    float specularTerm = pow(max(dot(fragNormal, halfwayDir), 0.0), 128);
    vec3 specular = specularTerm * light.color * attenuation;
    return diffuse + specular;
}

vec3 calcAmbientLight(AmbientLight light){
    return light.intensity * light.color;
}

void main(){
    vec3 res = calcPointLight(lightBlock.pointLight) +
               calcAmbientLight(lightBlock.ambientLight);

    outColor = vec4((res * fragColor), 1.0) * texture(texSampler, fragTexCoord);
}