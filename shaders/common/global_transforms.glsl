
layout(std140, binding = 0) uniform GlobalTransforms {
    mat4 projection3D;
    mat4 projection2D;
    mat4 view;
};