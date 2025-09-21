#version 460 core

uniform sampler2D lightBuffer;
uniform sampler2D forwardBuffer;

in vec2 TexCoords;
out vec4 FragColor;

void main() {
    vec4 litColor = texture(lightBuffer, TexCoords);
    vec4 forwardColor = texture(forwardBuffer, TexCoords);

    float alpha = forwardColor.a;
    vec3 blendedRGB = forwardColor.rgb * alpha + litColor.rgb * (1.0 - alpha);
    FragColor = vec4(blendedRGB, 1.0);
}