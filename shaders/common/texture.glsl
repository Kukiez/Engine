
struct Texture {
    float metallic;
    float ambient;
    float roughness;
};

Texture getORM(sampler2D ORM, vec2 uv) {
    vec3 orm = texture(ORM, uv).rgb;

    Texture tex;
    tex.ambient = orm.r;
    tex.roughness = orm.g;
    tex.metallic = orm.b;
    return tex;
}

Texture getORM(sampler2DArray ORM, vec3 uv) {
    vec3 orm = texture(ORM, uv).rgb;

    Texture tex;
    tex.ambient = orm.r;
    tex.roughness = orm.g;
    tex.metallic = orm.b;
    return tex;
}