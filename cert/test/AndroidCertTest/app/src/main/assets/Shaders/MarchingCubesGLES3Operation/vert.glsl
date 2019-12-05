#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

out vec3 vColor;
out vec3 vNormal;
out vec3 vWorldNormal;

uniform mat4 uMVP;
uniform mat4 uModel;

void main() {
    gl_Position = uMVP * vec4(inPosition, 1.0);
    vColor = inColor;
    vNormal = inNormal;
    vWorldNormal = mat3(uModel) * inNormal;
}
