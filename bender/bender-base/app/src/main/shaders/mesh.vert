#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "shader_bindings.h"

layout(set = BINDING_SET_MESH, binding = VERTEX_BINDING_MODEL_VIEW_PROJECTION)
uniform UniformBufferObject {
    mat4 mvp;
    mat4 model;
    mat4 invTranspose;
} ubo;

layout(set = BINDING_SET_LIGHTS, binding = FRAGMENT_BINDING_LIGHTS)
uniform LightBlock {
    vec3 pointLightPosition;
    vec3 pointLightColor;
    vec3 dirLightDirection;
    vec3 dirLightColor;
    vec3 cameraPos;
} lightBlock;

// This mirrors the vertex format found in ../jni/vulkan_main.cc:443
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inQTangent;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 cameraPos;
layout(location = 3) out vec3 pointLightPosition;
layout(location = 4) out vec3 pointLightColor;
layout(location = 5) out vec3 dirLightDirection;
layout(location = 6) out vec3 dirLightColor;

// Computes the first row of the rotation matrix represented by a given quaternion
vec3 xAxis(vec4 qQuat)
{
    float fTy  = 2.0 * qQuat.y;
    float fTz  = 2.0 * qQuat.z;
    float fTwy = fTy * qQuat.w;
    float fTwz = fTz * qQuat.w;
    float fTxy = fTy * qQuat.x;
    float fTxz = fTz * qQuat.x;
    float fTyy = fTy * qQuat.y;
    float fTzz = fTz * qQuat.z;

    return vec3(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
}

// Computes the second row of the rotation matrix represented by the given quaternion
vec3 yAxis(vec4 qQuat)
{
    float fTx  = 2.0 * qQuat.x;
    float fTy  = 2.0 * qQuat.y;
    float fTz  = 2.0 * qQuat.z;
    float fTwx = fTx * qQuat.w;
    float fTwz = fTz * qQuat.w;
    float fTxx = fTx * qQuat.x;
    float fTxy = fTy * qQuat.x;
    float fTyz = fTz * qQuat.y;
    float fTzz = fTz * qQuat.z;

    return vec3(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
}

void main() {
    // Decoding the tangent space basis vectors (normal, tangent, bitangent)
    // from the quaternion qtangent (stored in a vec4)
    vec4 qtangent = normalize(inQTangent);
    vec3 inNormal = xAxis(qtangent);
    vec3 inTangent = yAxis(qtangent);
    float bitangentReflection = sign(inQTangent.w);
    vec3 inBitangent = cross(inNormal, inTangent) * bitangentReflection;

    vec3 T = normalize(vec3(ubo.invTranspose * vec4(inTangent, 0.0)));
    vec3 B = normalize(vec3(ubo.invTranspose * vec4(inBitangent, 0.0)));
    vec3 N = normalize(vec3(ubo.invTranspose * vec4(inNormal, 0.0)));
    mat3 worldToTangent = transpose(mat3(T, B, N));

    gl_Position = ubo.mvp * inPosition;
    fragPos = worldToTangent * vec3(ubo.model * inPosition);
    fragTexCoord = inTexCoord;

    pointLightPosition = worldToTangent * lightBlock.pointLightPosition;
    pointLightColor = lightBlock.pointLightColor;
    dirLightDirection = worldToTangent * lightBlock.dirLightDirection;
    dirLightColor = lightBlock.dirLightColor;
    cameraPos = worldToTangent * lightBlock.cameraPos;
}