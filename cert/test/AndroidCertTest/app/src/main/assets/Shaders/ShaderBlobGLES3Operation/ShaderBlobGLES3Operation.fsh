#version 300 es

precision mediump float;

uniform vec2 uResolution;
uniform float uTime;

out vec4 frag_color;

// getCoord() returns clipspace coordinate (defined from 0.0-1.0, origin bottom-left)
vec2 getCoord() {
    return vec2(gl_FragCoord.x / uResolution.x, gl_FragCoord.y / uResolution.y);
}

vec4 defaultShader() {
    vec2 uv = getCoord();
    vec3 color = 0.5 + 0.5*cos(uTime + uv.xyx + vec3(0, 2, 4));
    return vec4(color, 1.0);
}

float sdSphere(vec3 p, float r) {
    return length(p) - r;
}

float sdLine(vec3 p, float r) {
    return 0.0;
}

float line2d(vec2 p) {
    return p.y;
}

float line3d(vec3 p, float r) {
    return length(p.yz) - r;
}

void main() {
    vec2 uv = getCoord();

    vec3 p = vec3(2.0 * uv - 1.0, 0.0); //, 0.1 * sin(uTime));

    vec3 color = vec3(0.0, 0.0, 0.0);

//    if (uv.y > 0.49 && uv.y < 0.51) {
//        color.r = 1.0;
//    }

//    vec3 p = vec3(0.5 * uv + 0.5, 0.0);
//    float r = 0.15;
//    color.r = sdSphere(p, r);

//
//    color.r = abs(p.x);
//    color.g = abs(p.y);

//    color.r = sdLine(p, 0.2);

//    if (abs(line2d(p.xy)) < 0.01) {
//        color.r = 1.0;
//    }

    if (line3d(p, 0.1 * abs(sin(uTime))) <= 0.0) {
        color.r = 1.0;
    }
//    color.r = abs(line(p.xy));

    frag_color = vec4(color, 1.0);
}
