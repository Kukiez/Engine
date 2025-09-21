#pragma once
#include "Resource.h"

class TextureSystem {
    std::vector<Texture2D> textures2Ds;
public:
    TextureSystem() = default;

    TextureSystem(const TextureSystem&) = delete;
    TextureSystem(TextureSystem&&) = delete;
    TextureSystem& operator=(const TextureSystem&) = delete;
    TextureSystem& operator=(TextureSystem&&) = delete;

    Texture2DKey loadTexture(Texture2D&& texture) {
        textures2Ds.emplace_back(std::move(texture));
        return Texture2DKey{textures2Ds.size() - 1};
    }


    Texture2D& getTexture(const Texture2DKey key) {
        return textures2Ds[static_cast<size_t>(key)];
    }
};