#version 300 es

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uView;

out vec3 vNormalWorld;

/**
This is really simple - no lighting, no effects. We do, however,
encode the world-space normal for each vertex to colorize output
*/
void main() {
    mat4 mv = uView * uModel;
    vec4 vertex = mv * vec4(pos, 1);
    gl_Position = uProjection * vertex;
    vNormalWorld = normalize(mat3(uModel) * normal);
}
