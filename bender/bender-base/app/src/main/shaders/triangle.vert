#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "shader_bindings.h"

layout(binding = VERTEX_BINDING_MODEL_VIEW_PROJECTION) uniform UniformBufferObject {
    mat4 mvp;
    mat4 model;
    mat4 invTranspose;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragPos;
layout(location = 3) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.mvp * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragNormal = mat3(ubo.invTranspose) * inNormal;
    fragPos = vec3(ubo.model * vec4(inPosition, 1.0));
    fragTexCoord = inTexCoord;
}