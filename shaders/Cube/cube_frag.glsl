#version 460 core

layout(location = 0) out vec4 f_FragColor;
layout(location = 1) out vec4 f_Emissive;
layout(location = 2) out vec4 gNormal;
layout(location = 3) out vec4 gLinearDepth;
in vec2 v_TexCoords;  
in vec3 v_Normal;     
in vec3 v_FragPos;   
in vec3 v_ViewPos;   

in flat uint materialIndex;
in flat uint materialTint;

uniform sampler2DArray sampler;

void main() {
    vec4 color = texture(sampler, vec3(v_TexCoords, materialIndex));
    vec4 unpacked = vec4(
        float((materialTint >> 24) & 0xFFu),
        float((materialTint >> 16) & 0xFFu),
        float((materialTint >> 8)  & 0xFFu),
        float((materialTint)       & 0xFFu)
    ) / 255.0;
    f_FragColor = unpacked * color;

    f_Emissive = vec4(0);
    if (f_FragColor.a <= 0.01f) discard;
    gNormal = vec4(v_Normal, 1);
    gLinearDepth = vec4(gl_FragCoord.z, 0, 0, 1);

}
