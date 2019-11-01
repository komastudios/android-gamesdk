#version 450
#extension GL_ARB_separate_shader_objects : enable

struct pointLight{
    float intensity;
    vec3 position;
    vec3 color;
};

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform lightingBlock {
    pointLight pl;
    vec3 cameraPos;
} lb;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 calcPointLight(pointLight light){
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 viewDir = normalize(lb.cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, fragNormal);
    float dist = length(light.position - fragPos);
    float attenuation = light.intensity / dist;

    float diff = max(dot(lightDir, fragNormal), 0.0);
    vec3 diffuse = diff * light.color * attenuation;
    float spec = max(dot(viewDir, reflectDir), 0.0);
    vec3 specular = spec * light.color * attenuation;
    return diffuse + specular;
}


void main(){
    vec3 res = calcPointLight(lb.pl);

    outColor = vec4((res * fragColor), 1.0);
}