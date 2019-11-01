#version 450
#extension GL_ARB_separate_shader_objects : enable

struct pointLight{
    float intensity;
    vec3 position;
    vec3 color;
};

struct ambientLight{
    float intensity;
    vec3 color;
};

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform lightingBlock {
    pointLight pl;
    ambientLight al;
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
    vec3 halfwayDir = normalize(lightDir +  viewDir);

    vec3 reflectDir = reflect(-lightDir, fragNormal);
    float dist = length(light.position - fragPos);
    float attenuation = light.intensity / dist;

    float diff = max(dot(lightDir, fragNormal), 0.0);
    vec3 diffuse = diff * light.color * attenuation;
    float spec = pow(max(dot(fragNormal, halfwayDir), 0.0), 128);
    vec3 specular = spec * light.color * attenuation;
    return diffuse + specular;
}

vec3 calcAmbientLight(ambientLight al){
    return al.intensity * al.color;
}


void main(){
    vec3 res = calcPointLight(lb.pl) + calcAmbientLight(lb.al);

    outColor = vec4((res * fragColor), 1.0) * texture(texSampler, fragTexCoord);
}