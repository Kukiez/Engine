//
// Created by dkuki on 5/18/2025.
//

#include "Model.h"

Texture2D MeshMaterial::createDefaultEmissive() {
    Texture2D emissive;

    unsigned char pixels[3] = {0,0,0};
    emissive.allocate(ColorBufferInternalFormat::RGB_8,
        ColorBufferFormat::RGB, GLType::UNSIGNED_BYTE, 1, 1, pixels,
        TextureMinFilter::LINEAR, TextureMagFilter::LINEAR, TextureWrap::REPEAT, false
    );
    return std::move(emissive);
}

Texture2D MeshMaterial::createDefaultNormal() {
    Texture2D normal;

    unsigned char pixels[3] = {128, 128, 255};

    normal.allocate(ColorBufferInternalFormat::RGB_8, ColorBufferFormat::RGB,
        GLType::UNSIGNED_BYTE, 1, 1, pixels,
        TextureMinFilter::LINEAR, TextureMagFilter::LINEAR, TextureWrap::REPEAT,
        false
    );
    return std::move(normal);
}

Texture2D MeshMaterial::createDefaultRoughnessMetallic() {
    Texture2D roughness;

    unsigned char pixels[3] = {255, 128, 0};
    roughness.allocate(ColorBufferInternalFormat::RGB_8, ColorBufferFormat::RGB,
        GLType::UNSIGNED_BYTE, 1, 1, pixels,
        TextureMinFilter::LINEAR, TextureMagFilter::LINEAR, TextureWrap::REPEAT,
        false
    );
    return std::move(roughness);
}

Texture2D MeshMaterial::DEFAULT_EMISSIVE;
Texture2D MeshMaterial::DEFAULT_NORMAL;
Texture2D MeshMaterial::DEFAULT_ROUGHNESS_METALLIC;