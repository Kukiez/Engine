#version 460 core

#include "../common/projection.glsl"
#include "../common/brdf.glsl"
#include "../common/light_structs.glsl"
#line 7

out vec4 FragColor;

uniform sampler2D gLinearDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetallicRoughness;

uniform mat4 inverseProjection;
uniform mat4 inverseView;
uniform mat4 view;
uniform vec3 camPos;

uniform vec2 screenSize;

uniform PointLight point_light;
uniform Spotlight spot_light;
uniform DirectionalLight dir_light;

uniform int LIGHT_TYPE;

LightResult computePointLight(PointLight light, LightParams params) {
    LightResult result;
    result.V = normalize(-params.viewPos);
    result.L = normalize(light.position - params.viewPos);
    result.H = normalize(result.V + result.L);
    result.N = normalize(params.normal);

    float distance = length(light.position - params.viewPos);
    result.attenuation = 1.0 / (distance * distance);
    result.attenuation *= 1.0 - smoothstep(0.0, light.radius, distance);

    result.intensity = light.intensity;
    result.color = light.color;
    return result;
}


LightResult computeSpotLight(Spotlight light, LightParams params) {
    LightResult result;
    result.V = normalize(-params.viewPos);
    result.L = normalize(light.position - params.viewPos);
    result.H = normalize(result.V + result.L);
    result.N = normalize(params.normal);

    float distance = length(light.position - params.viewPos);
    result.attenuation = 1.0 / (distance * distance);
    float theta = dot(normalize(-result.L), normalize(light.direction));
    float epsilon = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    result.attenuation *= intensity;

    result.intensity = light.intensity;
    result.color = light.color;
    return result;
}

LightResult computeDirectionalLight(DirectionalLight light, LightParams params) {
    LightResult result;
    result.V = normalize(-params.viewPos);
    result.L = normalize(-light.direction);
    result.H = normalize(result.V + result.L);
    result.N = normalize(params.normal);

    result.attenuation = 1.0;
    result.intensity = light.intensity;
    result.color = light.color;

    return result;
}

vec3 computeBRDF(LightResult light, LightParams params) {
    vec3 V = light.V;
    vec3 L = light.L;
    vec3 H = light.H;
    vec3 N = light.N;

    float NDF = DistributionGGX(N, H, params.roughness);
    float G = GeometrySmith(N, V, L, params.roughness);
    vec3 F0 = mix(vec3(0.04), params.albedo, params.metallic);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - params.metallic;

    float NdotL = max(dot(N, L), 0.0);
    vec3 irradiance = (kD * params.albedo / PI + specular) * light.color * light.intensity * light.attenuation * NdotL;
    return irradiance;
}

void main() {
    vec2 uv = gl_FragCoord.xy / screenSize;
    vec3 finalColor;

    LightParams params;
    params.normal = getViewSpaceNormal(gNormal, uv, view);
    params.albedo = texture(gAlbedo, uv).rgb;
    params.metallic = texture(gMetallicRoughness, uv).b;
    params.roughness = texture(gMetallicRoughness, uv).g;
    params.viewPos = reconstructViewPosition(uv, texture(gLinearDepth, uv).r, inverseProjection);

    LightResult result;
    if (LIGHT_TYPE == LIGHT_TYPE_POINT_LIGHT)
        result = computePointLight(point_light, params);
    else if (LIGHT_TYPE == LIGHT_TYPE_SPOT_LIGHT) {
        result = computeSpotLight(spot_light, params);
    } else {
        result = computeDirectionalLight(dir_light, params);
    }
    finalColor = computeBRDF(result, params);
    FragColor = vec4(finalColor, 1);
}