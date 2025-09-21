#version 460 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in float aIndex;


uniform mat4 model; // required
uniform mat4 projection; // non required

out vec2 v_Position;

void main()
{
    vec2 pos = aPos.xy;
    gl_Position = projection * model * vec4(pos, 0, 1.0);
    v_Position = pos;
}
