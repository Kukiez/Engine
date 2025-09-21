#include "MaterialSystem.h"

#include <filesystem>

TextureData MaterialLoader::loadTextureData(const char* file, const TextureLoadParams params, const int channels, const TexturePattern& pattern) {
    TextureData data;
    const bool flip = hasParam(params, TextureLoadParams::FLIP_VERTICALLY);
    if (hasParam(params, TextureLoadParams::CONVERT_SPECULAR_TO_PBR)) {
        data = TextureLoader::fromSpecularToPBR(file, flip);
    } else {
        data = TextureLoader::load(file,
            flip,
            channels
        );
    }
    return data;
}
Texture2D MaterialLoader::loadTexture(const char *file, TextureLoadParams params, const TexturePattern &pattern) {
    const auto format = internal_format_to_format(pattern.format);

    auto data = loadTextureData(file, params, channel_count(format), pattern);
    if (!data.pixels) return {};

    Texture2D texture;
    texture.allocate(pattern.format, format,
        internal_format_to_type(pattern.format), data.width,
        data.height, data.pixels, pattern.minFilter,
        pattern.magFilter, pattern.wrap, pattern.generateMipmaps, pattern.anisotropic
    );
    return std::move(texture);
}

Texture2D MaterialLoader::loadFile(const char *file, const TexturePattern *pat, const TextureLoadParams params) const {
    if (pat) {
        return loadTexture(file, params, *pat);
    }
    for (const auto& patternType : patterns) {
        for (const auto& pattern : patternType) {
            if (!pattern.matches(file)) continue;

            return loadTexture(file, params, pattern);
        }
    }
    return {};
}

Texture2D MaterialLoader::load(const char *path, const TextureLoadParams params, const TexturePattern *pattern) const {
    return loadFile(path, pattern, params);
}

TextureArray2D MaterialLoader::loadFolderArray(const char *path, const TextureLoadParams params, const TexturePattern& pattern, const int width, const int height, FolderLoaderResult* result, DefaultTexture defaultTex) {
    std::vector<std::string> filenames;

    static int defOutC = 0;
    for (const auto& file : std::filesystem::directory_iterator(path)) {
        if (!file.is_regular_file()) continue;
        if (!pattern.matches(file.path().string())) continue;
        filenames.push_back(file.path().string());
    }
    if (filenames.empty()) return {};

    if (hasParam(params, TextureLoadParams::SORT_FILENAMES_ASCENDING)) {
        std::ranges::sort(filenames);
    }

    std::vector<TextureData> textures;
    if (hasParam(params, TextureLoadParams::ADD_DEFAULT_TEXTURE)) {
        const bool isSpecular = hasParam(params, TextureLoadParams::CONVERT_SPECULAR_TO_PBR);
        const int channels = isSpecular ? 3 : channel_count(pattern.format);
        const auto pixels = defaultTex.fillTexture(width, height, channels);
    //    TextureLoader::writePNG(width, height, channels, pixels.data(), (std::string("defOutC") + std::to_string(defOutC++)).c_str());
        textures.emplace_back("__NULL__", pixels.data(), width, height, channels);

    }
    textures.reserve(filenames.size() + 1);
    const int channels = channel_count(pattern.format);

    for (const auto& filename : filenames) {
        auto& tex = textures.emplace_back(loadTextureData(filename.c_str(), params, channels, pattern));

        if (hasParam(params, TextureLoadParams::SKIP_LOWER_DIMENSIONS) && tex.width < width && tex.height < height) {
            textures.pop_back();
        }
        if (hasParam(params, TextureLoadParams::SKIP_HIGHER_DIMENSIONS) && tex.width > width && tex.height > height) {
            textures.pop_back();
        }
    }

    TextureArray2D array;

    array.allocate(pattern.format, internal_format_to_format(pattern.format),
        internal_format_to_type(pattern.format), width, height,
        textures.size(), MemorySpan(textures), pattern.minFilter, pattern.magFilter, pattern.wrap,
        pattern.generateMipmaps, pattern.anisotropic
    );
    if (result) {
        result->textures = std::move(textures);
    }
    return std::move(array);
}

MaterialSystem::MaterialSystem() : textureArrays(10), texture2Ds(10) {
}

void MaterialSystem::addMaterialArray(TextureArray2D &&array, std::string as) {
    auto index = textureArrays.emplace(std::move(array));
    materialToIndex.emplace(std::move(as), std::make_pair(TextureTarget::TEXTURE_ARRAY_2D, index));
}

MaterialRef MaterialSystem::getMaterialRef(const std::string_view mat) const {
    const auto it = materialToIndex.find(mat);

    if (it == materialToIndex.end()) return {};
    return {it->second.first, it->second.second};
}

unsigned MaterialSystem::getMaterialID(const MaterialRef pointer) const {
    if (pointer.handle == -1) return 0;

    switch (pointer.target) {
        case TextureTarget::TEXTURE_2D:
            return texture2Ds[pointer.handle].id();
        case TextureTarget::TEXTURE_ARRAY_2D:
            return textureArrays[pointer.handle].id();
        default: return 0;
    }
}

unsigned MaterialSystem::getMaterialID(const std::string_view mat) const {
    const auto it = materialToIndex.find(mat);

    if (it == materialToIndex.end()) return 0;

    switch (it->second.first) {
        case TextureTarget::TEXTURE_2D:
            return texture2Ds[it->second.second].id();
        case TextureTarget::TEXTURE_ARRAY_2D:
            return textureArrays[it->second.second].id();
        default: assert(false); std::unreachable();
    }
}

