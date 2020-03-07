#version 330

in VS_OUT
{
    vec4 color;
    vec3 worldNormal;
    vec3 worldPosition;
    float shininess;
    float tex0Contribution;
    float tex1Contribution;
    float worldDistance;
}
fs_in;

uniform vec3 uAmbientLight;
uniform vec3 uCameraPosition;
uniform float uRenderDistance;

out vec4 fragColor;

/////////////////////////////////////////////

void main()
{
    vec3 I = normalize(fs_in.worldPosition - uCameraPosition);
    vec3 R = reflect(I, fs_in.worldNormal);

    vec3 color = fs_in.worldNormal * 0.5 + 0.5;
    vec3 reflectionColor = R * 0.5 + 0.5;

    color *= uAmbientLight;
    color = mix(color, reflectionColor, fs_in.shininess);

    float distFactor = fs_in.worldDistance / uRenderDistance;
    float recolorFactor = distFactor * distFactor;
    float alpha = 1 - (recolorFactor * recolorFactor * recolorFactor);

    color =mix(color, vec3(1,0,1), recolorFactor);

    fragColor = vec4(color, alpha);
}
