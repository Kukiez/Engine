#version 460 core

in noperspective vec4 v_Frag_Position;
in vec4 v_Color;

out vec4 Frag_Color;

void main()
{
    Frag_Color = v_Color;
}