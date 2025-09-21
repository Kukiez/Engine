#version 460

mat4 translate(mat4 m, vec3 v) {
    mat4 Result = m;
    Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
    return Result;
}

mat4 scale(mat4 m, vec3 v) {
    mat4 Result = mat4(1);
    Result[0] = m[0] * v[0];
    Result[1] = m[1] * v[1];
    Result[2] = m[2] * v[2];
    Result[3] = m[3];
    return Result;
}

mat4 rotate(vec3 euler) {
    float cx = cos(euler.x);
    float sx = sin(euler.x);
    float cy = cos(euler.y);
    float sy = sin(euler.y);
    float cz = cos(euler.z);
    float sz = sin(euler.z);

    mat4 rotX = mat4(
        1, 0, 0, 0,
        0, cx, -sx, 0,
        0, sx, cx, 0,
        0, 0, 0, 1
    );

    mat4 rotY = mat4(
        cy, 0, sy, 0,
        0, 1, 0, 0,
        -sy, 0, cy, 0,
        0, 0, 0, 1
    );

    mat4 rotZ = mat4(
        cz, -sz, 0, 0,
        sz, cz, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );

    return rotZ * rotY * rotX; // ZYX order
}

struct Char3D {
    vec3 position;
    float _;
    vec3 color;
    float alpha;
    int chID;
    int textID;
    int _1;
    int _2;
};

struct TextInstance {
    vec3 center;
    float scale;
};

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

layout(std140, binding = 0) uniform GlobalTransforms {
    mat4 projection3D;
    mat4 projection2D;
    mat4 view;
    mat4 vectors;
};

layout(std430, binding = 1) buffer Characters {
    Char3D characters[];
};

layout(std430, binding = 2) buffer TextInstances {
    TextInstance instance[];
};

out vec4 vertex_color;
out vec2 TexCoords;
flat out int layer;

void main() {
    Char3D c = characters[gl_BaseInstance + gl_InstanceID];
    vec3 _CENTER = instance[c.textID].center;
    float _SCALE = instance[c.textID].scale;

    mat3 billboardRotation = mat3(view);
    billboardRotation = transpose(billboardRotation);

    vec3 charWorldOffset = _CENTER - (c.position.xyz + _CENTER);

    vec3 localVertexOffset = aPos;
    vec3 totalOffset = charWorldOffset + localVertexOffset * _SCALE;
    vec3 rotatedOffset = billboardRotation * totalOffset;

    vec3 worldPos = _CENTER + rotatedOffset;

    gl_Position = projection3D * view * vec4(worldPos, 1.0);

    vertex_color = vec4(c.color, 1.0);
    TexCoords = vec2(aTexCoords.x, 1 - aTexCoords.y);
    layer = c.chID;
}