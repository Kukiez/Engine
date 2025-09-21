#version 460 core
#include "../common/light_structs.glsl"

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform int LIGHT_TYPE;
uniform bool noview;

void main() {
    if (LIGHT_TYPE == LIGHT_TYPE_DIRECTIONAL_LIGHT || noview) {
        gl_Position = vec4(aPos, 1);
    } else {
        gl_Position = projection * view * model * vec4(aPos, 1);
    }
}