#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 EmissiveColor;

in vec2 TexCoords;

in vec4 vertex_color;

uniform sampler2DArray text;

in flat int layer;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, vec3(TexCoords, layer)).r);
    FragColor = vertex_color * sampled;
    EmissiveColor = vec4(0);
    if (FragColor.a <= 0.0001f) discard;
}