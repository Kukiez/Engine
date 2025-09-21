#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aTexCoords;

layout(std140, binding = 0) uniform GlobalTransforms {
    mat4 projection3D;
    mat4 projection2D;
    mat4 view;
    vec3 camera_pos;
};

layout(std430, binding = 1) buffer Matrices {
    mat4 models[];
};

void main() {
    gl_Position = projection3D * view * models[gl_InstanceID] * vec4(aPos, 1);
}