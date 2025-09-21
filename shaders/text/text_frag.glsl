#version 450 core

in vec2 TexCoords;
out vec4 color;

in vec4 vertex_color;

uniform sampler2DArray text;

in flat int layer;
in flat int visible;

void main() {
    if (visible == 1) {
        vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, vec3(TexCoords, layer)).r);
        color = vertex_color * sampled;
    } else {
        discard;
    }
}