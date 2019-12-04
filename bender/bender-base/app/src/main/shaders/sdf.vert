#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "shader_bindings.h"

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

layout(set = 0, binding = 0)
uniform Orientation {
    mat4 transform;
} orientation;

void main() {
    gl_Position = orientation.transform * vec4(inPosition, 0.0, 1.0);
    fragTexCoord = inTexCoord;
}