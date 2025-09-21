#version 460 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

layout(std140, binding = 0) uniform GlobalTransforms {
    mat4 projection3D;
    mat4 projection2D;
    mat4 view;
};

uniform mat4 model;

out vec2 TexCoords;

void main()
{
    gl_Position = projection2D * model * vec4(aPos, 0, 1.0);

    TexCoords = aTexCoords;
}
