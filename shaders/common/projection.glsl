struct NearFarProjection {
    float near;
    float far;
};

NearFarProjection getNearFar(mat4 projection) {
    NearFarProjection nf;

    float C = projection[2][2];
    float D = projection[3][2];
    nf.near = D / (C - 1.0f);
    nf.far = D / (C + 1.0f);
    return nf;
}

vec3 reconstructViewPosition(vec2 uv, float linearZ, mat4 invProj) {
    vec2 ndc = uv * 2.0 - 1.0;
    vec4 clip = vec4(ndc, linearZ * 2.0 - 1.0, 1.0);
    vec4 view = invProj * clip;
    return view.xyz / view.w;
}

vec3 getViewSpaceNormal(sampler2D normal, vec2 uv, mat4 view) {
    vec3 worldNormal = texture(normal, uv).rgb;
    vec3 viewNormal = mat3(view) * worldNormal;
    return viewNormal;
}

vec3 getViewSpaceNormalNormalized(sampler2D normal, vec2 uv, mat4 view) {
    return normalize(getViewSpaceNormal(normal, uv, view));
}