#version 300 es

in vec3 vColor;
in vec3 vNormal;
in vec3 vWorldNormal;

out vec4 fragColor;

/////////////////////////////////////////////

void main() {
    vec3 color = vColor * ((vNormal + vec3(1.0)) * 0.5);
    fragColor = vec4(color, 1);
}
