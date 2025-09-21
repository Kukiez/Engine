#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aTexCoords;

out v_out {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
} vout;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0f);

    vout.fragPos = aPos;
    vout.normal = aNormals;
    vout.texCoords = aTexCoords;
}