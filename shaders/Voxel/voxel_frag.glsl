#version 460 core
#include "../common/global_transforms.glsl"
#include "../common/texture.glsl"

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec4 gEmissive;
layout(location = 2) out vec4 gNormal;
layout(location = 3) out vec4 gLinearDepth;
layout(location = 4) out vec4 gAOMetallicRoughness;

in flat int texIndex;
in flat int faceID;
in vec2 texCoords;

uniform sampler2DArray materials;
uniform sampler2DArray normalMap;
uniform sampler2DArray specMap;

struct FaceTBNUV {
    vec3 tangent;
    float _;
    vec3 bitangent;
    float _2;
    vec3 normal;
    float _3;
    vec2 uv[4];
};

layout(std430, binding = 1) buffer VBNUVData {
    FaceTBNUV faces[6];
};

struct Material {
    int diffuse;
    int normal;
    int metallicRoughness;
    int _;
};
layout(std430, binding = 2) buffer Linker {
    Material mats[];
};

void main() {
    vec2 uv = texCoords;
    Material material = mats[texIndex];

    gAlbedo = texture(materials, vec3(uv, material.diffuse)).rgba;
    gEmissive = vec4(0, 0, 0, 1);

    vec3 tangentNormal = texture(normalMap, vec3(uv, material.normal)).rgb;

    tangentNormal = tangentNormal * 2.0 - 1.0;
    tangentNormal = normalize(tangentNormal);
    mat3 TBN = mat3(faces[faceID].tangent, faces[faceID].bitangent, faces[faceID].normal);
    vec3 worldNormal = normalize(TBN * tangentNormal);

    gNormal = vec4(worldNormal, 1.0);
    gLinearDepth = vec4(gl_FragCoord.z, 0, 0, 1);

    Texture tex = getORM(specMap, vec3(uv, material.metallicRoughness));
    gAOMetallicRoughness = vec4(tex.ambient, tex.roughness, tex.metallic, 1);
}