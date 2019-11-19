#version 300 es

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;

out vec4 vColor;

void main() {
    gl_Position = vec4(pos, 1.0);
    vColor = vec4(color, 1);
}
