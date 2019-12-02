#version 300 es
precision mediump float;

uniform sampler2D uTex;

in vec4 vColor;
in vec2 vTexCoord;

out vec4 outColor;

void main() {
    outColor = vColor * texture(uTex, vTexCoord);
}