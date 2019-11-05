#version 300 es
out vec4 outColor;

uniform float offset;

void main()
{
    mediump vec3 startValue = vec3(1.0f, 0.0f, 0.0f);

    startValue.x += offset;
    startValue = normalize(startValue);

    outColor = vec4(startValue, 1.0);
}

