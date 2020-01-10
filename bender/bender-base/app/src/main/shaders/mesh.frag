#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "shader_bindings.h"

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

struct AmbientLight {
    vec3 color;
    float intensity;
};

layout(set = BINDING_SET_MATERIAL, binding = SAMPLER_DIFFUSE)
uniform sampler2D textureDiffuse;
layout(set = BINDING_SET_MATERIAL, binding = SAMPLER_SPECULAR)
uniform sampler2D textureSpecular;
layout(set = BINDING_SET_MATERIAL, binding = SAMPLER_EMISSIVE)
uniform sampler2D textureEmissive;
layout(set = BINDING_SET_MATERIAL, binding = SAMPLER_NORMAL)
uniform sampler2D textureNormal;

layout(set = BINDING_SET_MATERIAL, binding = FRAGMENT_BINDING_MATERIAL_ATTRIBUTES)
uniform MaterialAttributes {
    vec3 ambient;
    vec3 diffuse;
    vec4 specular;
    float bumpMultiplier;
} materialAttr;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in LightBlock {
    vec3 pointLightPosition;
    vec3 pointLightColor;
    float pointLightIntensity;
    vec3 ambientLightColor;
    float ambientLightIntensity;
    vec3 cameraPos;
} fragLightBlock;

layout(location = 0) out vec4 outColor;

vec3 calcPointLight(PointLight light){
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 viewDir = normalize(fragLightBlock.cameraPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 lightDiff = light.position - fragPos;
    float attenuation = light.intensity / length(lightDiff);

    vec3 normal = texture(textureNormal, fragTexCoord).xyz;
    normal = normalize(normal * 2.0 - 1.0);

    float diffuseTerm = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diffuseTerm * light.color * attenuation * texture(textureDiffuse, fragTexCoord).xyz * materialAttr.diffuse;

    float specularTerm = pow(max(dot(normal, halfwayDir), 0.0), materialAttr.specular.w);
    vec3 specular = specularTerm * light.color * attenuation * texture(textureSpecular, fragTexCoord).xyz * materialAttr.specular.xyz;

    return specular + diffuse;
}

vec3 calcAmbientLight(AmbientLight light){
    return light.intensity * light.color * materialAttr.ambient * texture(textureDiffuse, fragTexCoord).xyz;
}

void main(){
    PointLight pLight;
    pLight.position = fragLightBlock.pointLightPosition;
    pLight.color = fragLightBlock.pointLightColor;
    pLight.intensity = fragLightBlock.pointLightIntensity;

    AmbientLight aLight;
    aLight.color = fragLightBlock.ambientLightColor;
    aLight.intensity = fragLightBlock.ambientLightIntensity;

    vec3 res = calcPointLight(pLight) + calcAmbientLight(aLight);

    outColor = vec4(res, 1.0);
}