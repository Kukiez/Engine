#version 450 core

layout(location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform GlobalTransforms {
    mat4 projection3D;
    mat4 projection2D;
    mat4 view;
};

struct Instance {
    mat4 model;
    vec4 color;
};

layout(std430, binding = 1) buffer Instances {
    Instance instances[];
};

out vec4 color;

void main() {
    gl_Position = projection3D * view * instances[gl_InstanceID].model * vec4(a_Position, 1.0);
    color = instances[gl_InstanceID].color;


}