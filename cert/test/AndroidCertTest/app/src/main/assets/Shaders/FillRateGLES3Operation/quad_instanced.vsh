#version 300 es

layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec2 offset;
layout(location = 4) in vec4 scaleRot;

uniform mat4 uProjection;

out vec4 vColor;
out vec2 vTexCoord;

void main() {
    mat2 sr = mat2(scaleRot.xy, scaleRot.zw);
    gl_Position = uProjection * vec4(sr * pos + offset, 0.0, 1.0);
    vColor = color;
    vTexCoord = texCoord;
}
