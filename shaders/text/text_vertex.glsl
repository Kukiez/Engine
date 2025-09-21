#version 450 core

struct Character {
    vec3 position;
    float degrees;
    vec4 color;
    vec2 rotation;
    int layer;
    int visible;
};

layout(location = 0) in vec2 aPos;

layout(std140, binding = 0) uniform GlobalTransforms {
    mat4 projection3D;
    mat4 projection2D;
    mat4 view;
};

layout(std430, binding = 1) buffer Characters {
    Character characters[];
};

out vec2 TexCoords;
out vec4 vertex_color;
out flat int layer;
out flat int visible;

uniform float yOffset;
uniform float xOffset;

void main()
{
    Character c = characters[gl_InstanceID];

    vec2 scaledOffset = aPos * c.position.z;
    vec2 charWorldPos = c.position.xy + scaledOffset;

    vec2 textOrigin = vec2(c.rotation);
    float textRotation = radians(c.degrees);
    vec2 relToOrigin = charWorldPos - textOrigin;

    float cosA = cos(textRotation);
    float sinA = sin(textRotation);
    vec2 rotated = vec2(
    cosA * relToOrigin.x - sinA * relToOrigin.y,
    sinA * relToOrigin.x + cosA * relToOrigin.y
    );
    vec2 worldPos = textOrigin + rotated;

    gl_Position = projection2D * vec4(worldPos, 0.0, 1.0);
    gl_Position.xy += vec2(xOffset, yOffset);

    vertex_color = c.color;
    TexCoords = aPos;
    visible = c.visible;
    layer = c.layer;
}