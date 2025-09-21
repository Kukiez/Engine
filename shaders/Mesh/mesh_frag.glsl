#version 460 core
#include "../common/texture.glsl"

out vec4 gAlbedo;
//layout(location = 0) out vec4 gAlbedo;
//layout(location = 1) out vec4 gEmissive;
//layout(location = 2) out vec4 gNormal;
//layout(location = 3) out vec4 gLinearDepth;
//layout(location = 4) out vec4 gAOMetallicRoughness; // (AO, Roughness, Metallic)

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

uniform sampler2D diffuse;
uniform sampler2D emissive;
uniform sampler2D roughness;
uniform sampler2D normal;

layout(std140, binding = 0) uniform GlobalTransforms {
    mat4 projection3D;
    mat4 projection2D;
    mat4 view;
    vec3 camera_pos;
};

vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(normal, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main() {
    vec4 baseColor = texture(diffuse, TexCoords);
    gAlbedo = vec4(baseColor);

    //gEmissive = vec4(texture(emissive, TexCoords).rgb, 1);
//
    //gNormal = vec4(getNormalFromMap(), 1.0);
//
    //Texture tex = getORM(roughness, TexCoords);
    //gAOMetallicRoughness = vec4(tex.ambient, tex.roughness, tex.metallic, 1.0f);
//
    //gLinearDepth = vec4(gl_FragCoord.z, 0, 0, 1);
}