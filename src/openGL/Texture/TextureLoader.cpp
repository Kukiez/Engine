#include "TextureLoader.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <algorithm>
#include <stb/stb_image.h>
#include <filesystem>
#include <unordered_map>
#include <fstream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

TextureData::TextureData(std::string path, const unsigned char *pixels, const int width, const int height, const int channels)
    : width(width), height(height), channels(channels), path(std::move(path))
{
    this->pixels = new unsigned char[width * height * channels];
    memcpy(this->pixels, pixels, width * height * channels * sizeof(unsigned char));

    if (channels == 4)
        format = TextureFormat::RGBA;
    else if (channels == 3)
        format = TextureFormat::RGB;
    else if (channels == 2)
        format = TextureFormat::RG;
    else
        format = TextureFormat::R;
}

TextureData::TextureData(const float *pixels, const int width, const int height, const int channels)
    : width(width), height(height), channels(channels) {
    this->floatPixels = new float[width * height * channels];
    memcpy(this->floatPixels, pixels, width * height * channels * sizeof(float));
    if (channels == 4)
        format = TextureFormat::RGBA;
    else if (channels == 3)
        format = TextureFormat::RGB;
    else if (channels == 2)
        format = TextureFormat::RG;
    else
        format = TextureFormat::R;
}


TextureData::TextureData(const int width, const int height, const int channels, const TextureFormat format)
    : width(width), height(height), channels(channels), format(format)
{
    assert(width * height * channels); // no char[0]
    pixels = new unsigned char[width * height * channels];
}

TextureData::TextureData(const TextureData &other)
    : width(other.width), height(other.height), channels(other.channels), format(other.format), path(other.path)
{
    if (other.pixels) {
        pixels = new unsigned char[width * height * channels];
        std::memcpy(pixels, other.pixels, width * height * channels);
    } else {
        pixels = nullptr;
    }
}

TextureData& TextureData::operator=(const TextureData& other) {
    if (this != &other) {
        delete[] pixels;

        width    = other.width;
        height   = other.height;
        channels = other.channels;
        format   = other.format;
        path     = other.path;

        if (other.pixels) {
            pixels = new unsigned char[width * height * channels];
            std::memcpy(pixels, other.pixels, width * height * channels);
        } else {
            pixels = nullptr;
        }
    }
    return *this;
}

TextureData::TextureData(TextureData&& other) noexcept
    : pixels(other.pixels), width(other.width), height(other.height),
      channels(other.channels), format(other.format), path(std::move(other.path))
{
    other.pixels = nullptr;
}

TextureData& TextureData::operator=(TextureData&& other) noexcept {
    if (this != &other) {
        pixels   = other.pixels;
        width    = other.width;
        height   = other.height;
        channels = other.channels;
        format   = other.format;
        path     = std::move(other.path);

        other.pixels   = nullptr;
    }
    return *this;
}

TextureData::~TextureData() {
    delete[] pixels;
}





TextureData TextureLoader::load(const char* file, bool flip, const int forcedChannels)
{
    int width, height, channels;

    if (flip)
        stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(file, &width, &height, &channels, forcedChannels);
    if (!image) {
        std::cerr << "[ERROR] Failed to load texture: " << file << std::endl;
        return {};
    }

    if (forcedChannels) {
        channels = forcedChannels;
    }
    TextureData data(file, image, width, height, channels);

    stbi_image_free(image);
    stbi_set_flip_vertically_on_load(0);
    return data;
}

TextureData TextureLoader::loadf(const char *file, bool flip, const int forcedChannels) {
    int width, height, channels;

    if (flip)
        stbi_set_flip_vertically_on_load(1);
    float* image = stbi_loadf(file, &width, &height, &channels, forcedChannels);
    if (!image) {
        std::cerr << "[ERROR] Failed to load texture: " << file << std::endl;
        return {};
    }

    std::cout << "[INFO] Loaded texture: " << file << std::endl;
    std::cout << "[INFO] Width: " << width << std::endl;
    std::cout << "[INFO] Height: " << height << std::endl;
    std::cout << "[INFO] Channels: " << channels << std::endl;
    if (forcedChannels) {
        channels = forcedChannels;
    }
    TextureData data(image, width, height, channels);
    data.path = file;

    stbi_image_free(image);
    stbi_set_flip_vertically_on_load(0);
    return data;
}
#include <ImfRgbaFile.h>   // for Imf::Rgba, Imf::RgbaInputFile
#include <ImfArray.h>      // for Imf::Array2D
#include <half.h>
using namespace OPENEXR_IMF_NAMESPACE;
using namespace IMATH_NAMESPACE;


HDRTextureData<half> TextureLoader::loadEXR16F(const char *path, bool flip, int forcedChannels) {
    try {
        RgbaInputFile file(path);
        Box2i dw = file.dataWindow();

        int width  = dw.max.x - dw.min.x + 1;
        int height = dw.max.y - dw.min.y + 1;

        Array2D<Rgba> pixels;
        pixels.resizeErase(height, width);

        file.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
        file.readPixels(dw.min.y, dw.max.y);

        half* rawData = reinterpret_cast<half*>(&pixels[0][0]);

        std::cout << "Loaded: " << width << " x " << height << std::endl;

        return {rawData, width, height, 4};
    } catch (std::exception& e) {
        std::cerr << "Failed to load EXR file: " << path << std::endl;
        std::cerr << e.what() << std::endl;
        return {};
    }
}

std::vector<TextureData> TextureLoader::loadSpritesheet(const char *file)
{
    std::vector<TextureData> spritesheet;

    int width, height, channels;

    unsigned char* image = stbi_load(file, &width, &height, &channels, 0);

    if (!image) {
        std::cerr << "[ERROR] Failed to load texture: " << file << std::endl;
        return {};
    }

    std::cout << "[INFO] Loaded texture: " << file << std::endl;
    std::cout << "[INFO] Width: " << width << std::endl;
    std::cout << "[INFO] Height: " << height << std::endl;
    std::cout << "[INFO] Channels: " << channels << std::endl;


    std::string filename(file);
    std::ifstream meta(filename + ".meta");

    if (!meta) { 
        std::cerr << "[ERROR] Could not open metadata file: " << filename << ".meta" << std::endl;
        stbi_image_free(image);
        return {};
    }

    int cols, rows;
    meta >> cols >> rows;
    meta.close();

    if (rows <= 0 || cols <= 0) {
        std::cerr << "[ERROR] Invalid rows or columns in metadata file: " << filename << ".meta" << std::endl;
        stbi_image_free(image);
        return {};
    }

    int subWidth = width / cols;
    int subHeight = height / rows;

    std::vector<unsigned char> subPixels(subWidth * subHeight * channels);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int x = col * subWidth;
            int y = row * subHeight;

            for (int j = 0; j < subHeight; ++j) {
                memcpy(&subPixels[j * subWidth * channels],
                       &image[((y + j) * width + x) * channels],
                       subWidth * channels);
            }
            spritesheet.push_back(TextureData(file, subPixels.data(), subWidth, subHeight, channels));
            spritesheet.back().path = file;
        }
    }
    std::cout << "Spritesheet Created: " << file << ": " <<  spritesheet.size() << std::endl;
    stbi_image_free(image);
    return spritesheet;
}

std::vector<TextureData> TextureLoader::loadFolder(const char* directory, bool flip, bool sorted, const int channels)
{
    namespace fs = std::filesystem;

    std::vector<std::string> filenames;

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            if (entry.path().extension() == ".png") {
                filenames.push_back(entry.path().string());
            }
        }
    }

    if (sorted)
        std::sort(filenames.begin(), filenames.end(), [](const std::string& a, const std::string& b) {
            return a.size() < b.size();
        });
    std::vector<TextureData> textures;

    for (const auto& filename : filenames) {
        TextureData data = load(filename.c_str(), flip, channels);
        data.path = filename;
        std::cout << "data.path: " << data.path << std::endl;
        textures.push_back(data);
    }
    return textures;
}

std::array<TextureData, 6> TextureLoader::loadCubemap(const char *directory) {
    namespace fs = std::filesystem;

    std::vector<std::pair<std::string, TextureData>> tempFiles;
    std::array<TextureData, 6> cubemap;

    std::unordered_map<std::string, int> cubemapOrder = {
        {"right",  0},  // +X
        {"left",   1},  // -X
        {"top",    2},  // +Y
        {"bottom", 3},  // -Y
        {"front",  4},  // +Z
        {"back",   5}   // -Z
    };

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().stem().string();
            auto it = cubemapOrder.find(filename);
            if (it != cubemapOrder.end()) {
                tempFiles.emplace_back(filename, load(entry.path().string().c_str()));
                tempFiles.back().second.path = std::string(directory) + "/" + entry.path().string();
            }
        }
    }

    std::sort(tempFiles.begin(), tempFiles.end(), [&](const auto& a, const auto& b) {
        return cubemapOrder[a.first] < cubemapOrder[b.first];
    });

    for (size_t i = 0; i < tempFiles.size(); i++) {
        cubemap[i] = tempFiles[i].second;
    }

    std::cout << "Cubemap Loaded: " << directory << std::endl;
    return cubemap;
}


TextureData TextureLoader::fromSpecularToPBR(const char *file, bool flip) {
    int width, height, channels;

    stbi_set_flip_vertically_on_load(flip);
    unsigned char* specularData = stbi_load(file, &width, &height, &channels, 1);

    if (!specularData) {
        std::cerr << "Failed to load image: " << file << std::endl;
        return {};
    }

    std::vector<unsigned char> pbrData(width * height * 3);

    for (int i = 0; i < width * height; ++i) {
        const unsigned char spec = specularData[i];
        const unsigned char roughness = 255 - spec;
        float specNorm = spec / 255.0f;
        const float metallicF = std::clamp((specNorm - 0.08f) / (1.0f - 0.08f), 0.0f, 1.0f);
        const unsigned char metallic = static_cast<unsigned char>(metallicF * 255.0f);
        constexpr static unsigned char r = 255;

        pbrData[i * 3] = r;
        pbrData[i * 3 + 1] = roughness;
        pbrData[i * 3 + 2] = metallic;
    }

    stbi_write_png("metallicRoughness.png", width, height, 3, pbrData.data(), width * 3);
    stbi_image_free(specularData);
    stbi_set_flip_vertically_on_load(0);

    return TextureData(file, pbrData.data(), width, height, 3);
}

void TextureLoader::writePNG(const int width, const int height, const int channels, const void *pixels, const std::string &as) {
    stbi_write_png(std::string(as + ".png").c_str(), width, height, channels, pixels, width * channels);
}
