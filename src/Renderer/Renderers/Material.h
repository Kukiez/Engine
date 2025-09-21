#pragma once
#include <openGL/Texture/Texture2D.h>

struct MeshMaterial {
    static Texture2D createDefaultEmissive();
    static Texture2D createDefaultNormal();
    static Texture2D createDefaultRoughnessMetallic();

    static Texture2D DEFAULT_EMISSIVE;
    static Texture2D DEFAULT_NORMAL;
    static Texture2D DEFAULT_ROUGHNESS_METALLIC;

    int diffuse = -1;
    int metallicRoughness = -1;
    int normal = -1;
    int emissive = -1;
    bool isTransparent = false;
};