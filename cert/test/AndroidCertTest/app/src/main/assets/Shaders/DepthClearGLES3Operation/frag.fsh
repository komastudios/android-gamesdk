#version 300 es
precision mediump float;

out vec4 outColor;

in vec4 vColor;

void main() {
    outColor = vec4(vColor.rgb,1);
}