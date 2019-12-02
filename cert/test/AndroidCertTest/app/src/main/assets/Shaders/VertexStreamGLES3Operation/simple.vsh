#version 300 es

layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;

uniform mat4 uProjection;

out vec4 vColor;
out vec2 vTexCoord;

void main() {
    gl_Position = uProjection * vec4(pos, 0.0, 1.0);
    vColor = color;
    vTexCoord = texCoord;
}
