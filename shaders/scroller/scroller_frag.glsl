#version 460 core

in vec2 v_Position;

out vec4 f_FragColor;
uniform vec4 vertex_color[3];

uniform vec2 breakpoint;
uniform float slider_begin_x;
uniform vec2 slider_end;

uniform bool enable; //if should render the circle

float reverseLerpFastEnd(float value, float begin, float end, float power) {
    float normalized = (value - begin) / (end - begin);
    return pow(normalized, 1.0 / power);
}

uniform bool relative_to_end;

void main() {
    if (!enable) {
        if (relative_to_end) {
            float lerp = reverseLerpFastEnd(gl_FragCoord.x, slider_begin_x, slider_end.x, 2);

            if (gl_FragCoord.x <= slider_begin_x) {
                f_FragColor = vertex_color[2];
            } else {
                f_FragColor = mix(vertex_color[2], vertex_color[0], lerp );
            }
        } else {
            f_FragColor = vertex_color[0];
        }
    } else if (gl_FragCoord.x <= breakpoint.x) {
        if (relative_to_end) {
            f_FragColor = mix(vertex_color[2], vertex_color[0], reverseLerpFastEnd(gl_FragCoord.x, slider_begin_x, slider_end.x, 2));
        } else {
            f_FragColor = mix(vertex_color[2], vertex_color[0], reverseLerpFastEnd(gl_FragCoord.x, slider_begin_x, breakpoint.x, 2));
        }
    } else {
        f_FragColor = vertex_color[1];
    }
}