#version 300 es
precision mediump float;

uniform vec4 uColor;
in vec3 vNormalWorld;

out vec4 outColor;

/**
Output color is uColor modulated by world-space normal (which is translated from (-1,+1) to (0,1))
*/
void main() {
    outColor = uColor * vec4(vNormalWorld * 0.5 + vec3(1,1,1), 1);
}
