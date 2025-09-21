#pragma once

#include <FastNoiseLite.h>
#include <generator>
#include <Systems/Mesh/FCurve.h>

class Noise {
    FastNoiseLite noise;
    float frequency;
    int octaves;
    float lacunarity;
    float gain;
    float amplitude;
public:
    Noise(int seed = 1337, int frequency = 0.01f, int octaves = 1, float lacunarity = 2.0f, float gain = 0.5f, float amplitude = 1.0f, FastNoiseLite::NoiseType type = FastNoiseLite::NoiseType_OpenSimplex2) {
        noise.SetNoiseType(type);
        noise.SetSeed(seed);
        noise.SetFrequency(frequency);
        noise.SetFractalOctaves(octaves);
        noise.SetFractalLacunarity(lacunarity);
        noise.SetFractalGain(gain);
        this->frequency = frequency;
        this->octaves = octaves;
        this->lacunarity = lacunarity;
        this->gain = gain;
        this->amplitude = amplitude;
    }

    float at(const float x, const float z) const {
        return noise.GetNoise(x, z);
    }

    float ridge(glm::vec2 p) const {
        return 1.0f - glm::abs(at(p.x, p.y));
    }

    float getFrequency() const {
        return frequency;
    }
};


namespace noise {
    static constexpr int chunkWidth = 16;
    static constexpr int chunkHeight = 16;
    static constexpr int chunkDepth = 16;
    static constexpr int maxHeight = 256;

    inline std::vector<int> getHeightMap(const int worldX, const int worldZ) {
        Noise noise;
        std::vector<int> heightMap(chunkWidth * chunkDepth);

        for (int x = 0; x < chunkWidth; ++x) {
            for (int z = 0; z < chunkDepth; ++z) {
                float sampleX = (x + worldX) * noise.getFrequency();
                float sampleZ = (z + worldZ) * noise.getFrequency();

                float noiseVal = noise.at(sampleX, sampleZ);
                float normalized = (noiseVal + 1.0f) * 0.5f;
                int height = static_cast<int>(normalized * maxHeight);

                heightMap[x + z * chunkWidth] = height;
            }
        }
        return heightMap;
    }

}