#pragma once
#include <memory/hash.h>

#include "ECS/System/ISystem.h"
#include "Model/Model.h"
#include <ECS/ECS.h>

#include "Model/ModelLoaderSystem.h"

enum class GeometryKey : size_t {
    INVALID = std::numeric_limits<size_t>::max()
};

struct PendingLoadGeometry {
    std::string file;
    std::string as;
};

struct PendingLoadGeometryFromGeometry {
    Geometry geometry;
    std::string as;
};

class GeometrySystem {
    mem::vector<Geometry> geometries;
    mem::unordered_stringmap<size_t> geometryKeys;
public:
    GeometrySystem() {}

    GeometrySystem(const GeometrySystem&) = delete;
    GeometrySystem& operator=(const GeometrySystem&) = delete;

    GeometryKey loadGeometry(Geometry& geometry, std::string_view as) {
        geometries.emplace_back(std::move(geometry));
        geometryKeys.emplace(as, geometries.size() - 1);
        return GeometryKey{geometries.size() - 1};
    }

    GeometryKey loadGeometry(const std::string_view file, std::string as) {
        if (auto geometry = ModelLoaderSystem::loadGeometry(file); geometry.has_value()) {
            geometries.emplace_back(std::move(geometry.value()));
            geometryKeys.emplace(as, geometries.size() - 1);
            return GeometryKey{geometries.size() - 1};
        }
        std::cout << "Failed to load geometry: " << file << std::endl;
        return GeometryKey::INVALID;
    }

    GeometryKey getGeometryKey(const std::string_view str) const {
        const auto it = geometryKeys.find(str);
        return it == geometryKeys.end() ? GeometryKey::INVALID : GeometryKey{it->second};
    }

    const Geometry& getGeometry(const GeometryKey key) const {
        if (key == GeometryKey::INVALID) {
            cexpr::require(false);
        }
        return geometries[cexpr::enum_cast(key)];
    }

    const Geometry& getGeometry(const std::string_view str) const {
        return getGeometry(getGeometryKey(str));
    }
};
