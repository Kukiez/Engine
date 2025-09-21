#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aID;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aNormal;

out vec3 v_Normal;
out vec3 v_Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    v_Normal = mat3(transpose(inverse(model))) * aNormal;
    v_Position = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(v_Position, 1.0);
}