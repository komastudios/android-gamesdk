#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "shader_bindings.h"

layout(set = 0, binding = 1)
uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;


layout(location = 0) out vec4 outColor;

void main() {
    float distance = texture(texSampler, fragTexCoord).a;
    float smoothWidth = fwidth(distance);
    float alpha = smoothstep(0.5 - smoothWidth, 0.5 + smoothWidth, distance);
    vec3 rgb = vec3(alpha);

    outColor = vec4(rgb, alpha);
}
