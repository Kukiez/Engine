#version 460 core

layout (location = 0) in vec3 aPos;

out vec3 v_WorldPos;

layout(std140, binding = 0) uniform GlobalTransforms {
    mat4 projection3D;
    mat4 projection2D;
    mat4 view;
};

void main()
{
    v_WorldPos = aPos;
    vec4 pos = projection3D * mat4(mat3(view)) * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}