#version 450 core

mat4 translate(mat4 m, vec3 v) {
    mat4 result = m;
    result[3] = m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3];
    return result;
}

mat4 rotate(mat4 m, float angle, vec3 v) {
    float c = cos(angle);
    float s = sin(angle);

    vec3 axis = normalize(v);
    vec3 temp = (1.0 - c) * axis;

    mat4 Rotate = mat4(1.0);
    Rotate[0][0] = c + temp.x * axis.x;
    Rotate[0][1] = temp.x * axis.y + s * axis.z;
    Rotate[0][2] = temp.x * axis.z - s * axis.y;

    Rotate[1][0] = temp.y * axis.x - s * axis.z;
    Rotate[1][1] = c + temp.y * axis.y;
    Rotate[1][2] = temp.y * axis.z + s * axis.x;

    Rotate[2][0] = temp.z * axis.x + s * axis.y;
    Rotate[2][1] = temp.z * axis.y - s * axis.x;
    Rotate[2][2] = c + temp.z * axis.z;

    mat4 result;
    result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
    result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
    result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
    result[3] = m[3];

    return result;
}

mat4 eulerAngleXYZ(float t1, float t2, float t3) {
    float c1 = cos(-t1);
    float c2 = cos(-t2);
    float c3 = cos(-t3);
    float s1 = sin(-t1);
    float s2 = sin(-t2);
    float s3 = sin(-t3);

    return mat4(
        c2 * c3, -c1 * s3 + s1 * s2 * c3,  s1 * s3 + c1 * s2 * c3, 0.0,
        c2 * s3,  c1 * c3 + s1 * s2 * s3, -s1 * c3 + c1 * s2 * s3, 0.0,
        -s2,      s1 * c2,                c1 * c2,                0.0,
        0.0,      0.0,                     0.0,                    1.0
    );
}

mat4 scale(mat4 m, vec3 v) {
    mat4 result;
    result[0] = m[0] * v.x;
    result[1] = m[1] * v.y;
    result[2] = m[2] * v.z;
    result[3] = m[3];
    return result;
}

struct ParticleData {
    vec4 color;
    vec4 beginColor;
    vec4 endColor;

    vec4 position;
    vec4 velocity;
    vec4 growthRate;
    vec4 scale;

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

void main()
{
    int id = gl_InstanceID;

    mat4 model = mat4(1.0f);

    model = translate(model, particles[id].position.xyz);

    if (true) {
        vec3 forward = normalize(cameraVectors[3].xyz - particles[id].position.xyz);
        vec3 right = normalize(cross(vec3(0, 1, 0), forward));
        vec3 up = cross(forward, right);

        model = mat4(
        vec4(right, 0.0),
        vec4(up, 0.0),
        vec4(forward, 0.0),
        vec4(particles[id].position.xyz, 1.0)
        );
        mat4 rotationZ = rotate(mat4(1.0f), radians(90.0f), vec3(0, 0, 1));
        mat4 rotationX = rotate(mat4(1.0f), radians(90.0f), vec3(1,0, 0));
        mat4 rotationY = rotate(mat4(1), radians(-90), vec3(0, 1, 0));

        model = model * rotationZ;
        model = model * rotationX;
        model = model * rotationY;
    }

    model = translate(model, vec3(-0.5f, -0.5f, 0.0f));
    model = scale(model, particles[id].scale.xyz);

    gl_Position = projection3D * view * model * vec4(aPos, 1);
    mat3 normalMatrix = transpose(inverse(mat3(model)));

    vertex_out.fragPos = vec3(model * vec4(aPos, 1));
    vertex_out.normal = normalize(normalMatrix * aNormal);
    vertex_out.texCoords = aTexCoords;
    vertex_out.color = particles[id].color;

    vertex_out.texLayer = particles[id].layer + int(float(particles[id].age) / float(particles[id].lifetime) * float(particles[id].maxLayer));
}