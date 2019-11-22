#version 300 es

// Usage: Call glDrawArrays(GL_TRIANGLES, 0, 3) with no vertex arrays.
// Renders a full-screen triangle, clipped into a quad.

const vec2 points[3] = vec2[](
    vec2(-1.0, 3.0),
    vec2(-1.0, -1.0),
    vec2(3.0, -1.0)
);

void main() {
    gl_Position = vec4(points[gl_VertexID], 0.0, 1.0);
}
