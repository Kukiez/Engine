#version 460 core

layout(location = 0) in vec2 aPos;

struct UIUniform {
    mat4 model;
    vec4 vertex_color[4];
};

out vec4 v_Frag_Position;
out noperspective vec4 v_Color;

layout(std140, binding = 0) uniform GlobalTransforms {
    mat4 projection3D;
    mat4 projection2D;
    mat4 view;
};

layout (std140, binding = 1) buffer WidgetUniforms {
    UIUniform uniforms[];
};

void main()
{
    gl_Position = projection2D * uniforms[gl_InstanceID].model * vec4(aPos.xy, 0, 1.0);
    gl_Position.z = 0.0f;

    v_Frag_Position = gl_Position;
    v_Color = uniforms[gl_InstanceID].vertex_color[gl_VertexID];
}
