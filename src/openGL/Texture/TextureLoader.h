#pragma once

#include <openGL/Texture/TextureEnum.h>
#include <array>
#include <half.h>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <algorithm>

class TextureData {
    friend class TextureLoader;

public:
    std::string path;
    unsigned char* pixels = nullptr;
    float* floatPixels = nullptr;
    int width, height, depth = 0, channels;
    TextureFormat format;

    TextureData(std::string path, const unsigned char* pixels, int width, int height, int channels);
    TextureData(const float* pixels, int width, int height, int channels);

    TextureData() : width(0), height(0), channels(0), format(TextureFormat::RGBA) {}
    TextureData(int width, int height, int channels, TextureFormat format);

    TextureData(const TextureData& other);
    TextureData& operator=(const TextureData& other);

    TextureData(TextureData&& other) noexcept;
    TextureData& operator=(TextureData&& other) noexcept ;

    ~TextureData();
};

template <typename F>
struct HDRTextureData;

template <>
struct HDRTextureData<half> {
    void flip() const {
        const int rowSize = width * channels;

        for (int y = 0; y < height / 2; ++y) {
            half* rowTop = pixels + y * rowSize;
            half* rowBottom = pixels + (height - 1 - y) * rowSize;

            for (int i = 0; i < rowSize; ++i) {
                std::swap(rowTop[i], rowBottom[i]);
            }
        }
    }

    half* pixels;
    int width = 0, height = 0, channels = 0;

    HDRTextureData() : pixels(nullptr), width(0), channels(0) {}

    HDRTextureData(const half* pixels, const int width, const int height, const int channels)
        :  width(width), height(height), channels(channels) {
        this->pixels = new half[width * height * channels];
        memcpy(this->pixels, pixels, width * height * channels * sizeof(half));
    }

    ~HDRTextureData() {
        delete[] pixels;
    }
};

template <>
struct HDRTextureData<float> {
    float* pixels;
    int width = 0, height = 0, channels = 0;

    HDRTextureData() : pixels(nullptr) {}

    HDRTextureData(const half* pixels, const int width, const int height, const int channels)
        :  width(width), height(height), channels(channels) {
        this->pixels = new float[width * height * channels];
        for (int i = 0; i < width * height * channels; i++) {
            this->pixels[i] = pixels[i];
        }
    }

    ~HDRTextureData() {
        delete[] pixels;
    }
};


class TextureLoader {
public:
    static TextureData load(const char* file, bool flip = false, int forcedChannels = 0);
    static TextureData loadf(const char* file, bool flip = false, int forcedChannels = 0);
    static HDRTextureData<half> loadEXR16F(const char* file, bool flip = false, int forcedChannels = 0);
    static std::vector<TextureData> loadSpritesheet(const char* file);
    static std::vector<TextureData> loadFolder(const char* directory, bool flip = false, bool sorted = true, const int channels = 0);
    static std::array<TextureData, 6> loadCubemap(const char* directory);
    static TextureData fromSpecularToPBR(const char* file, bool flip = false);

    template <typename K> // requires K convertible to float*
    static std::vector<uint8_t> floatToUint8(const std::vector<K>& data) {
        std::vector<uint8_t> bytes(data.size() * sizeof(K) / sizeof(float));
        float* fdata = (float*)data.data();

        for (size_t i = 0; i < bytes.size(); ++i) {
            float v = fdata[i];
            v = (v < 0.0f) ? 0.0f : (v > 1.0f ? 1.0f : v);
            bytes[i] = static_cast<uint8_t>(v * 255.0f);
        }
        return bytes;
    }
    static void writePNG(int width, int height, int channels, const void *pixels, const std::string &as);
};