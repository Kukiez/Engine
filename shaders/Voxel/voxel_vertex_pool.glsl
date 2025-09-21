#version 460 core
#include "../common/global_transforms.glsl"

layout(location = 0) in vec3 aPos;

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

struct QuadDrawInstance {
    mat4 model; 
    int textureID;
    int aFace;
    vec2 uv;
};

layout(std430, binding = 3) buffer QuadDrawInstances {
    QuadDrawInstance instances[];
};

void main() {
    QuadDrawInstance quad = instances[gl_BaseInstance + gl_InstanceID];
    gl_Position = projection3D * view * quad.model * vec4(aPos, 1.0f);

    texIndex = quad.textureID;
    faceID = quad.aFace;
    texCoords = faces[quad.aFace].uv[gl_VertexID % 4] * quad.uv;
}