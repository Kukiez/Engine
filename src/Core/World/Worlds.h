#pragma once
#include <glm/glm.hpp>

struct WorldRegionID {
    glm::ivec3 position;

    WorldRegionID(glm::ivec3 _position) : position(_position) {}
    WorldRegionID(const int x, const int y, const int z) : position(x, y, z) {}
    WorldRegionID(float x, float y, float z) : position(x, y, z) {}


    bool operator == (const WorldRegionID& other) const {
        return position == other.position;
    }

    operator const glm::ivec3& () const {
        return position;
    }
};

template <>
struct std::hash<WorldRegionID> {
    std::size_t operator()(const WorldRegionID& w) const noexcept {
        const glm::ivec3& v = w.position;
        std::size_t seed = 0;
        auto hash_combine = [&seed](std::size_t h) {
            seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        };
        hash_combine(std::hash<int>()(v.x));
        hash_combine(std::hash<int>()(v.y));
        hash_combine(std::hash<int>()(v.z));
        return seed;
    }
};

template <typename World>
struct WorldQuery;