#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aTexCoords;

layout(std140, binding = 0) uniform GlobalTransforms {
    mat4 projection3D;
    mat4 projection2D;
    mat4 view;
    vec3 camera_pos;
};

struct Instance {
    mat4 model;
    uint tint;
    uint materialIndex;
};

layout(std430, binding = 1) buffer Matrices {
    Instance instances[];
};

out vec3 v_Normal;
out vec2 v_TexCoords;
out vec3 v_FragPos;
out vec3 v_ViewPos;

out flat uint materialIndex;
out flat uint materialTint;

void main() {
    Instance instance = instances[gl_BaseInstance + gl_InstanceID];

    gl_Position = projection3D * view * instance.model * vec4(aPos, 1);

    v_TexCoords = aTexCoords;
    v_Normal = mat3(transpose(inverse(instance.model))) * aNormals;
    v_FragPos = vec3(instance.model * vec4(aPos, 1));
    v_ViewPos = camera_pos;

    materialIndex = instance.materialIndex;
    materialTint = instance.tint;
}