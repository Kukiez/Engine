#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;

layout(std140, binding = 0) uniform GlobalTransforms {
    mat4 projection3D;
    mat4 projection2D;
    mat4 view;
    vec3 camera_pos;
};

layout(binding = 1) buffer BoneBuffer {
    mat4 boneMatrices[];
};

struct BoneMetadata {
    int boneCount;
    int c;
    int _, _c;
};

layout(binding = 2) buffer BoneMetadataBuffer {
    BoneMetadata boneMetadata[];
};

out vec2 TexCoords;
out vec3 tint;

void main() {
    int boneCount = boneMetadata[gl_DrawID].boneCount;
    int offset = gl_BaseInstance + gl_InstanceID * (boneCount + 1);
// Draw 0:
    // bones[0].boneCount = 61;
    // bones[0].boneOffset = 0;
    // var instanceOffset = 0 * (62 + 1) = 0;
    // var offset = 0 + 0 = 0;
    // Correct: reading from 0 to 62.
// Draw 1:
    // bones[1].boneCount = 156;
    // bones[1].boneOffset = 62;
    // var instanceOffset = 0 * (156 + 1) = 0;
    // var offset = 62 + 0 = 62;
    // Correct: reading from 62 to (62 + 156).

    mat4 boneTransform  = boneMatrices[offset + 1 + boneIds[0]] * weights[0];
    boneTransform += boneMatrices[offset + 1 + boneIds[1]] * weights[1];
    boneTransform += boneMatrices[offset + 1 + boneIds[2]] * weights[2];
    boneTransform += boneMatrices[offset + 1 + boneIds[3]] * weights[3];

    vec4 posFinal = boneTransform * vec4(pos, 1.0f);
    mat4 viewModel = view * boneMatrices[offset];
    gl_Position = projection3D * viewModel * posFinal;

    TexCoords = tex;
    tint = vec3(1, 1, 1);
}