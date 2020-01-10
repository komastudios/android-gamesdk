#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "shader_bindings.h"

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

struct AmbientLight {
    vec3 color;
    float intensity;
};

layout(set = BINDING_SET_MESH, binding = VERTEX_BINDING_MODEL_VIEW_PROJECTION)
uniform UniformBufferObject {
    mat4 mvp;
    mat4 model;
    mat4 invTranspose;
} ubo;

layout(set = BINDING_SET_LIGHTS, binding = FRAGMENT_BINDING_LIGHTS)
uniform LightBlock {
    PointLight pointLight;
    AmbientLight ambientLight;
    vec3 cameraPos;
} lightBlock;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inBitangent;
layout(location = 4) in vec2 inTexCoord;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out LightBlock {
    vec3 pointLightPosition;
    vec3 pointLightColor;
    float pointLightIntensity;
    vec3 ambientLightColor;
    float ambientLightIntensity;
    vec3 cameraPos;
} fragLightBlock;

void main() {
    vec3 T = normalize(vec3(ubo.invTranspose * vec4(inTangent, 0.0)));
    vec3 B = normalize(vec3(ubo.invTranspose * vec4(inBitangent, 0.0)));
    vec3 N = normalize(vec3(ubo.invTranspose * vec4(inNormal, 0.0)));
    mat3 worldToTangent = transpose(mat3(T, B, N));

    gl_Position = ubo.mvp * vec4(inPosition, 1.0);
    fragPos = worldToTangent * vec3(ubo.model * vec4(inPosition, 1.0));
    fragTexCoord = inTexCoord;

    fragLightBlock.pointLightPosition = worldToTangent * lightBlock.pointLight.position;
    fragLightBlock.pointLightColor = lightBlock.pointLight.color;
    fragLightBlock.pointLightIntensity = lightBlock.pointLight.intensity;

    fragLightBlock.ambientLightColor = lightBlock.ambientLight.color;
    fragLightBlock.ambientLightIntensity = lightBlock.ambientLight.intensity;

    fragLightBlock.cameraPos = worldToTangent * lightBlock.cameraPos;
}