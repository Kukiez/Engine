#version 460 core

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoords;

uniform sampler2D bloom;

const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
uniform float texelSize;
uniform bool horizontal;
uniform float strength;

void main() {
    vec2 texOffset = horizontal ? vec2(texelSize, 0.0) : vec2(0.0, texelSize) * strength;
    vec3 result = texture(bloom, v_TexCoords).rgb * weight[0];

    for (int i = 1; i < 5; ++i) {
        result += texture(bloom, v_TexCoords + texOffset * float(i)).rgb * weight[i];
        result += texture(bloom, v_TexCoords - texOffset * float(i)).rgb * weight[i];
    }
    FragColor = vec4(result, 1.0);
}