#version 450 core

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

mat4 rotationMatrix(vec3 euler) {
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

struct ParticleData {
    vec4 beginColor;
    vec4 endColor;

    vec4 position;
    vec4 velocity;
    vec4 growthRate;
    vec4 scale;
    vec4 rotation;
    vec4 rotationSpeed;

    int lifetime;
    int age;
    int layer;
    int maxLayer;
};

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(std140, binding = 0) uniform GlobalTransforms {
    mat4 projection3D;
    mat4 projection2D;
    mat4 view;
    mat4 cameraVectors;
};

layout(std430, binding = 2) buffer ParticleBuffer {
    ParticleData particles[];
};

out v_out {
    flat int texLayer;
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    vec4 color;
} vertex_out;

void main() {
    int id = gl_InstanceID;
    mat4 model = mat4(1.0f);

    model = translate(model, particles[id].position.xyz);
    model *= rotationMatrix(particles[id].rotation.xyz);
    model = scale(model, vec3(particles[id].scale.xyz));

    gl_Position = projection3D * view * model * vec4(aPos, 1);
    mat3 normalMatrix = transpose(inverse(mat3(model)));

    vertex_out.fragPos = vec3(model * vec4(aPos, 1));
    vertex_out.normal = normalize(normalMatrix * aNormal);
    vertex_out.texCoords = aTexCoords;
    vertex_out.color = mix(particles[id].beginColor,
            particles[id].endColor,
            float(particles[id].age) / float(particles[id].lifetime));

    int frame = particles[id].layer;
    vertex_out.texLayer = int(mix(particles[id].layer, particles[id].maxLayer, float(particles[id].age) / float(particles[id].lifetime)));
}