#pragma once
#include <regex>
#include <openGL/Texture/TextureArray2D.h>

#include "Material.h"
#include <util/enum_bit.h>
#include <util/Function.h>
#include <util/stl.h>
#include <memory/Array.h>

struct TexturePattern {
    Function<bool(std::string_view file)> pattern;
    ColorBufferInternalFormat format;
    TextureMinFilter minFilter;
    TextureMagFilter magFilter;
    TextureWrap wrap;
    bool generateMipmaps = false;
    bool anisotropic = false;

    TexturePattern(Function<bool(std::string_view file)> pattern, ColorBufferInternalFormat format, TextureMinFilter minFilter, TextureMagFilter magFilter, TextureWrap wrap, bool generateMipmaps, bool anisotropic)
        : pattern(std::move(pattern)), format(format), minFilter(minFilter), magFilter(magFilter), wrap(wrap), generateMipmaps(generateMipmaps), anisotropic(anisotropic)
    {}

    TexturePattern() = default;

    bool matches(const std::string_view file) const {
        return pattern(file);
    }
};

enum class TextureLoadParams {
    NONE = 0,
    FLIP_VERTICALLY = 1,
    CONVERT_SPECULAR_TO_PBR = 2,
    SORT_FILENAMES_ASCENDING = 4,
    FOLDER_RECURSIVE = 8,
    SKIP_HIGHER_DIMENSIONS = 16,
    SKIP_LOWER_DIMENSIONS = 32,
    SET_DIMENSIONS_TO_HIGHEST_APPEARANCE = 64,
    ADD_DEFAULT_TEXTURE = 128
};

constexpr bool hasParam(const TextureLoadParams params, const TextureLoadParams param) {
    return (params & param) == param;
}

struct DefaultTexture {
    unsigned char pixels[4]{};

    static DefaultTexture NormalMap() {
        return {128, 128,255, 255};
    }

    static DefaultTexture EmissiveMap() {
        return {0, 0, 0, 255};
    }

    static DefaultTexture AOMetallicRoughness() {
        return {255, 128, 0, 255};
    }

    std::vector<unsigned char> fillTexture(const int width, const int height, const int channels) const {
        std::vector<unsigned char> result(width * height * channels);

        const int pixelCount = width * height;
        for (int i = 0; i < pixelCount; ++i) {
            for (int c = 0; c < channels; ++c) {
                result[i * channels + c] = pixels[c];
            }
        }
        return result;
    }
};

struct FolderLoaderResult {
    std::vector<std::string> skippedFiles;
    std::vector<TextureData> textures;
};

class MaterialLoader {
    std::array<std::vector<TexturePattern>, static_cast<int>(TextureType::MAX_TEXTURE_TYPES)> patterns;

    static TextureData loadTextureData(const char *file, TextureLoadParams params, int channels, const TexturePattern &pattern);

    static Texture2D loadTexture(const char* file, TextureLoadParams params, const TexturePattern& pattern);

    Texture2D loadFile(const char* file, const TexturePattern* pat, const TextureLoadParams params) const;
public:
    void registerPattern(const TextureType texture, const TexturePattern& pattern) {
        patterns[static_cast<int>(texture)].emplace_back(pattern);
    }

    Texture2D load(const char* path, TextureLoadParams params = TextureLoadParams::NONE, const TexturePattern* pattern = nullptr) const;

    static TextureArray2D loadFolderArray(const char* path, TextureLoadParams params, const TexturePattern& pattern, int width, int height, FolderLoaderResult* result = nullptr, DefaultTexture defaultTex = {});
};

struct MaterialRef {
    TextureTarget target;
    int handle = -1;
};

class MaterialSystem {
    stl::unordered_stringmap<std::pair<TextureTarget, int>> materialToIndex;
    MemoryArray<TextureArray2D> textureArrays;
    MemoryArray<Texture2D> texture2Ds;
public:
    MaterialSystem();

    void addMaterialArray(TextureArray2D&& array, std::string as);

    MaterialRef getMaterialRef(std::string_view mat) const;

    unsigned getMaterialID(MaterialRef pointer) const;
    unsigned getMaterialID(const std::string_view mat) const;
};

struct MaterialConstants {
    static constexpr auto MINECRAFT_BLOCKS_DIFFUSE = "m$MBF";
    static constexpr auto MINECRAFT_BLOCKS_NORMAL = "m$MSN";
    static constexpr auto MINECRAFT_BLOCKS_SPECULAR = "m$MRS";
};