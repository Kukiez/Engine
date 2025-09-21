#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 Normal;
out vec3 WorldPos;

struct Instance {
    mat4 model;
};

layout(std140, binding = 0) uniform GlobalTransforms {
    mat4 projection3D;
    mat4 projection2D;
    mat4 view;
    vec3 camera_pos;
};

layout(std430, binding = 1) buffer InstanceData {
    Instance instances[];
};

uniform int index;

uniform mat4 matrices[10];

void main() {
    TexCoords = aTexCoords;
    mat4 model = matrices[index];
    WorldPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection3D * view * vec4(WorldPos, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * aNormal;
}