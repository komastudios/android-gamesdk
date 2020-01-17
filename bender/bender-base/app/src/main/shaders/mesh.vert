#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "shader_bindings.h"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 3) in mat4 instanceMvp;
layout(location = 4) in mat4 instanceModel;
layout(location = 5) in mat4 instanceInvTranspose;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;

void main() {
    gl_Position = instanceMvp * vec4(inPosition, 1.0);
    fragNormal = mat3(instanceInvTranspose) * inNormal;
    fragPos = vec3(instanceModel * vec4(inPosition, 1.0));
    fragTexCoord = inTexCoord;
}