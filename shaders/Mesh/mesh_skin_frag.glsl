#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragEmissive;

in vec2 TexCoords;
in vec3 tint;

uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D emissive;
uniform sampler2D roughness;
uniform sampler2D normal;

uniform bool has_emissive;
uniform bool has_specular;
uniform bool has_roughness;
uniform bool has_normal;
uniform bool has_diffuse;

void main()
{
    vec4 baseColor = texture(diffuse, TexCoords) * vec4(tint, 1.0);
    FragColor = baseColor;

    if (has_emissive) {
        vec4 emissiveColor = texture(emissive, TexCoords);
        FragColor += emissiveColor;
        FragEmissive = emissiveColor;
    }
    FragColor = clamp(FragColor, 0.0, 1.0);

    if (FragColor.a < 0.1f) discard;
}