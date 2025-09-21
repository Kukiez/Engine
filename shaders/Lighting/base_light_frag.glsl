#version 460 core

#include "../common/projection.glsl"
#include "../common/texture.glsl"
#line 6

in vec2 v_TexCoords;
out vec4 FragColor;

uniform sampler2D lightBuffer;
uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gEmissive;
uniform sampler2D gMetallicRoughness;
uniform sampler2D gLinearDepth;
uniform sampler2D ssao;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform mat4 inverseProjection;
uniform mat4 inverseView;
uniform vec3 camPos;
uniform mat4 view;

const float PI = 3.14159265359;

vec3 reconstructViewPosition(vec2 uv, float depth) {
    vec4 clip = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 viewPos = inverseProjection * clip;
    viewPos /= viewPos.w;
    return viewPos.xyz;
}

uniform bool disable_ssao;

void main() {
    vec2 uv = v_TexCoords;

    vec3 albedo     = texture(gAlbedo, uv).rgb;
    vec3 normal     = getViewSpaceNormalNormalized(gNormal, uv, view);
    vec3 emissive   = texture(gEmissive, uv).rgb;

    Texture tex = getORM(gMetallicRoughness, uv);
    float ao        = texture(ssao, uv).r * tex.ambient;
    float metallic  = tex.metallic;
    float roughness = tex.roughness;
    float depth     = texture(gLinearDepth, uv).r;

    vec3 viewPos = reconstructViewPosition(uv, depth);
    vec3 V = normalize(-viewPos);
    vec3 N = normal;

    // Environment IBL
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // === 1. Diffuse IBL from irradianceMap ===
    vec3 diffuseIBL = texture(irradianceMap, N).rgb;
    vec3 diffuse = diffuseIBL * albedo * (1.0 - metallic);

    // === 2. Specular IBL ===
    vec3 R = reflect(-V, N);
    R = normalize(R);
    const float MAX_REFLECTION_LOD = 5.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;

    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F0 * brdf.x + brdf.y);

    // === 3. Ambient lighting with AO ===

    vec3 ambient = vec3(ao) * (specular + diffuse);

    // === 4. Add real-time lights and emissive ===
    vec3 light = texture(lightBuffer, uv).rgb;
    vec3 finalColor = light + ambient + emissive;
    FragColor = vec4(finalColor, 1.0);
}