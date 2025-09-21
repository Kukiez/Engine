#version 450 core
out vec4 Frag_Color;

in v_out {
    flat int texLayer;
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    vec4 color;
} fs_in;

uniform sampler2DArray texture_unit_array;

void main() {
    vec4 texColor = texture(texture_unit_array, vec3(fs_in.texCoords, fs_in.texLayer));

    if (texColor.a <= 0.0001f) discard;
    Frag_Color = texColor * fs_in.color;
}