#version 450 core

out vec4 fs_FragColor;

in v_out {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
} fs_in;

uniform sampler2D texture_unit;

uniform vec3 view_pos;

uniform vec3 light_pos;

void calc__light() {
    vec3 color = vec3(1, 1, 1);

    vec3 ambient = 0.05 * color;

    vec3 lightDir = normalize(light_pos - fs_in.fragPos);
    vec3 normal = normalize(fs_in.normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    vec3 viewDir = normalize(view_pos - fs_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;

    spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    
    vec3 specular = vec3(1.0, 0.5529, 0.5529) * spec; 
    fs_FragColor = vec4(ambient + diffuse + specular, 0.25f);
}

void main() {
    fs_FragColor = vec4(1, 0, 1, 1);
}