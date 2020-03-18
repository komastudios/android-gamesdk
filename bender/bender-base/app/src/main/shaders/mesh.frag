#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "shader_bindings.h"

layout(set = BINDING_SET_MATERIAL, binding = SAMPLER_DIFFUSE)
uniform sampler2D textureDiffuse;
layout(set = BINDING_SET_MATERIAL, binding = SAMPLER_SPECULAR)
uniform sampler2D textureSpecular;
layout(set = BINDING_SET_MATERIAL, binding = SAMPLER_EMISSIVE)
uniform sampler2D textureEmissive;
layout(set = BINDING_SET_MATERIAL, binding = SAMPLER_SPECULAR_EXPONENT)
uniform sampler2D textureMetalRough;
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
layout(location = 2) in vec3 cameraPos;
layout(location = 3) in vec3 pointLightPosition;
layout(location = 4) in vec3 pointLightColor;
layout(location = 5) in vec3 dirLightDirection;
layout(location = 6) in vec3 dirLightColor;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;

vec3 N = normalize(texture(textureNormal, fragTexCoord).xyz * 2.0 - 1.0);
float metallic = texture(textureMetalRough, fragTexCoord).x;
float roughness = texture(textureMetalRough, fragTexCoord).y;
vec3 albedo = texture(textureDiffuse, fragTexCoord).xyz;

float DistributionGGX(float NdotH, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 calcLight(vec3 lightDirection, vec3 radiance) {
    vec3 V = normalize(cameraPos - fragPos);
    vec3 L = normalize(lightDirection);
    vec3 H = normalize(V + L);

    float NdotH = clamp(dot(N, H), 0.0, 1.0);
    float NdotV = clamp(dot(N, V), 0.0, 1.0);
    float NdotL = clamp(dot(N, L), 0.0, 1.0);
    float HdotV = clamp(dot(H, V), 0.0, 1.0);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // cook-torrance brdf
    float NDF = DistributionGGX(NdotH, roughness);
    float G   = GeometrySmith(NdotV, NdotL, roughness);
    vec3 F    = fresnelSchlick(HdotV, F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL;
    vec3 specular     = numerator / max(denominator, 0.001);

    // return outgoing radiance Lo
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 calcLightNoSpecular(vec3 lightDirection, vec3 radiance) {
    vec3 V = normalize(cameraPos - fragPos);
    vec3 L = normalize(lightDirection);
    vec3 H = normalize(V + L);

    float NdotL = clamp(dot(N, L), 0.0, 1.0);
    float HdotV = clamp(dot(H, V), 0.0, 1.0);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // cook-torrance brdf
    vec3 F    = fresnelSchlick(HdotV, F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    // return outgoing radiance Lo
    return (kD * albedo / PI) * radiance * NdotL;
}

vec3 calcPointLight(vec3 lightPosition, vec3 lightColor){
    vec3 posToLight = lightPosition - fragPos;
    float attenuation = 1.0 / dot(posToLight, posToLight);
    vec3 radiance     = lightColor * attenuation;
    return calcLight(posToLight, radiance);
}

vec3 calcDirectionLight(vec3 lightDirection, vec3 lightColor) {
    return calcLight(lightDirection, lightColor);
}

void main(){
    vec3 res = calcPointLight(pointLightPosition, pointLightColor);
    res += calcDirectionLight(dirLightDirection, dirLightColor);
    res += calcLightNoSpecular(-dirLightDirection, 2 * dirLightColor);
    res = res / (res + vec3(1.0));

    outColor = vec4(res, 1.0);
}