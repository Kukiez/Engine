#version 460 core

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoords;

uniform sampler2D scene;
uniform sampler2D bloom;

uniform float bloomIntensity;
uniform float exposure;

void main()
{
    vec3 color = texture(scene, v_TexCoords).rgb;
    vec3 bloom = texture(bloom, v_TexCoords).rgb;

    vec3 final = color + bloom * bloomIntensity;

    FragColor = vec4(final, 1.0f);
}