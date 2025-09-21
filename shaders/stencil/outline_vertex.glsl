#version 460 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 projection;

out vec2 v_TexCoords;

void main() {
    gl_Position = projection * model * vec4(aPos, 0, 1);

    v_TexCoords = aTexCoords;
}