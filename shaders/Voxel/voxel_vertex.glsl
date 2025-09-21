#version 460 core
#include "../common/global_transforms.glsl"

layout(location = 0) in vec3 aPos;
layout(location = 1) in int aTBNUV;
layout(location = 2) in int aTexIndex;
layout(location = 3) in int aFace;
layout(location = 4) in ivec2 aSize;

out flat int texIndex;
out flat int faceID;
out vec2 texCoords;

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

void main() {
    gl_Position = projection3D * view * vec4(aPos, 1.0f);

    texIndex = aTexIndex;
    faceID = aTBNUV;
    texCoords = aSize;
}