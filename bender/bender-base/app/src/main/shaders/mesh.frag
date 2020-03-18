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
layout(set = BINDING_SET_MATERIAL, binding = SAMPLER_SPECULAR_EXPONENT)
uniform sampler2D textureSpecularExponent;
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

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
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
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 calcPointLight(PointLight light){
    vec3 N = normalize(texture(textureNormal, fragTexCoord).xyz * 2.0 - 1.0);
    vec3 V = normalize(fragLightBlock.cameraPos - fragPos);
    float metallic = texture(textureSpecularExponent, fragTexCoord).x;
    float roughness = texture(textureSpecularExponent, fragTexCoord).y;
    vec3 albedo = pow(texture(textureDiffuse, fragTexCoord).xyz, vec3(2.2));
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // calculate per-light radiance
    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(V + L);
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance     = light.color * attenuation * light.intensity;

    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular     = numerator / max(denominator, 0.001);

    // return outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 calcAmbientLight(AmbientLight light){
    vec3 color =  pow(texture(textureDiffuse, fragTexCoord).xyz, vec3(2.2));
    return light.intensity * light.color * materialAttr.ambient * color;
}

void main(){
    PointLight point_light;
    point_light.position = fragLightBlock.pointLightPosition;
    point_light.color = fragLightBlock.pointLightColor;
    point_light.intensity = fragLightBlock.pointLightIntensity;

    AmbientLight ambient_light;
    ambient_light.color = fragLightBlock.ambientLightColor;
    ambient_light.intensity = fragLightBlock.ambientLightIntensity;

    vec3 res = calcPointLight(point_light) + calcAmbientLight(ambient_light);

    res = res / (res + vec3(1.0));
    res = pow(res, vec3(1.0/2.2));

    outColor = vec4(res, 1.0);
}