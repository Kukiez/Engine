#pragma once
#include <Math/BoundingVolumeHierarchy.h>
#include "Worlds.h"
#include "ECS/Entity/Entity.h"
#include "ECS/Level/Views/LevelUpdateView.h"
#include "Math/Shapes/AABB.h"
#include <unordered_map>
#include <Collision/CollisionComponents.h>

/**
 * Component Requirements:
 * - Transform
 * - CollisionMesh or CollisionObject
 */
struct TerrainWorld : Reads<StaticCollider, DynamicCollider, Transform, AABBCollision>, ResourceSystem<>, Stages<DefaultStage> {
    FRIEND_DESCRIPTOR

    constexpr static auto REGION_SIZE = 64;

    struct WorldEntry {
        AABB bounds{};
        Entity entity{};

        WorldEntry() = default;

        WorldEntry(const AABB& aabb, const Entity entity) : bounds(aabb), entity(entity) {}
        operator const AABB&() const { 
            return bounds; 
        }

        bool operator == (const Entity e) const {
            return entity == e;
        }
    };
    
    struct RegionBVHMatrices {
        glm::mat4 worldTransform{};
        glm::mat4 inverseWorldTransform{};
        glm::mat4 normalMatrix{};
    };

    struct Region {
        RegionBVHMatrices matrices;
        BVH<WorldEntry> bvh;
        std::vector<Entity> visitors;
        mem::vector<WorldEntry> stash;
        int changes = 0;

        Region() = default;
        Region(glm::ivec3 local);

        void addEntity(const Entity& entity, const AABB& aabb);
        void updateEntity(const Entity& entity, const AABB& aabb);
        void build();
        void refit();

        AABB bounds() const {
            return bvh.bounds();
        }
    };

private:
    Region& getRegion(const glm::vec3& worldPos);
    void updateRegions();

    std::unordered_map<WorldRegionID, Region> regions;
    std::vector<WorldRegionID> dirtyRegions;
public:
    TerrainWorld() = default;
    TerrainWorld(const TerrainWorld&) = delete;
    TerrainWorld& operator = (const TerrainWorld&) = delete;
    TerrainWorld(TerrainWorld&& other) = delete;
    TerrainWorld& operator = (TerrainWorld&& other) = delete;

    static WorldRegionID worldToLocalCoords(const glm::vec3& worldPos) {
        return {
            std::floor(worldPos.x / REGION_SIZE),
            std::floor(worldPos.y / REGION_SIZE),
            std::floor(worldPos.z / REGION_SIZE)
        };
    }

    void onLevelLoad(LevelLoadView<TerrainWorld>& level);
    void onUpdate(LevelUpdateView<TerrainWorld>& level);

    auto& getRegions() const {
        return regions;
    }

    static auto getRegionSize() {
        return REGION_SIZE;
    }

    WorldQuery<TerrainWorld> query(const AABB& aabb) const;
    WorldQuery<TerrainWorld> query(const Ray& ray) const;
};

#include "TerrainWorldQuery.h"

inline WorldQuery<TerrainWorld> TerrainWorld::query(const AABB& aabb) const {
    return WorldQuery<TerrainWorld>(*this, aabb);
}

inline WorldQuery<TerrainWorld> TerrainWorld::query(const Ray& ray) const {
    return WorldQuery<TerrainWorld>(*this, ray);
}