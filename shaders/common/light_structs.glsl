

struct PointLight {
    vec3 position;
    vec3 color;
    float radius;
    float intensity;
};

struct Spotlight {
    vec3 position;
    vec3 direction;
    float innerCutoff;
    float outerCutoff;
    vec3 color;
    float intensity;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

const int LIGHT_TYPE_POINT_LIGHT = 0;
const int LIGHT_TYPE_SPOT_LIGHT = 1;
const int LIGHT_TYPE_DIRECTIONAL_LIGHT = 2;

struct LightParams {
    vec3 normal;
    vec3 albedo;
    float metallic;
    float roughness;
    vec3 viewPos;
};

struct LightResult {
    vec3 V;
    vec3 L;
    vec3 H;
    vec3 N;
    float attenuation;
    vec3 color;
    float intensity;
};