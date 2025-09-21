#pragma once

template <> struct WorldQuery<TerrainWorld> {
    const TerrainWorld* world;
    union {
        AABB aabb;
        Ray ray;
    };

    WorldQuery(const TerrainWorld& world, const AABB& aabb)
    : world(&world), aabb(aabb) {}

    WorldQuery(const TerrainWorld& world, const Ray& ray)
    : world(&world), ray(ray) {}

    template <typename CB>
    requires std::is_invocable_r_v<void, CB, const TerrainWorld::WorldEntry&>
    void forEachIntersects(CB&& cb) {
        WorldRegionID min(TerrainWorld::worldToLocalCoords(aabb.min()));
        WorldRegionID max(TerrainWorld::worldToLocalCoords(aabb.max()));

        for (int x = min.position.x; x <= max.position.x; ++x)
        for (int y = min.position.y; y <= max.position.y; ++y)
        for (int z = min.position.z; z <= max.position.z; ++z)
        {
            const auto it = world->getRegions().find(WorldRegionID{x, y, z});
            if (it == world->getRegions().end())
                continue;            
            it->second.bvh.forEachIntersects(aabb, [&] (const auto& data) {
                cb(data);
                return false;
            });
        }
    }

    template <typename CB>
    void forEachRayIntersects(CB&& cb) {
        glm::vec3 dir = glm::normalize(ray.direction);
        glm::vec3 pos = ray.origin;
        glm::ivec3 chunk = TerrainWorld::worldToLocalCoords(pos).position;

        const auto step = glm::ivec3(glm::sign(dir));
        glm::vec3 tMax, tDelta;

        for (int i = 0; i < 3; ++i) {
            const float rayDir = dir[i];
            const float originInAxis = pos[i];
            const float chunkCoord = static_cast<float>(chunk[i]) * float(TerrainWorld::REGION_SIZE);

            if (rayDir > 0) {
                const  float nextBoundary = chunkCoord + TerrainWorld::REGION_SIZE;
                tMax[i] = (nextBoundary - originInAxis) / rayDir;
                tDelta[i] = static_cast<float>(TerrainWorld::REGION_SIZE) / rayDir;
            } else if (rayDir < 0) {
                const float prevBoundary = chunkCoord;
                tMax[i] = (prevBoundary - originInAxis) / rayDir;
                tDelta[i] = -static_cast<float>(TerrainWorld::REGION_SIZE) / rayDir;
            } else {
                tMax[i] = std::numeric_limits<float>::infinity();
                tDelta[i] = std::numeric_limits<float>::infinity();
            }
        }
        float distanceTraveled = 0.0f;

        while (distanceTraveled <= ray.length) {
            const auto it = world->getRegions().find(WorldRegionID{chunk});
            if (it != world->getRegions().end()) {
                const auto& [world, inverse, normal] = it->second.matrices;
                it->second.bvh.forEachRayIntersects(ray, [&] (const auto& data) {
                    return cb(data);
                });
            }

            if (tMax.x < tMax.y && tMax.x < tMax.z) {
                chunk.x += step.x;
                distanceTraveled = tMax.x;
                tMax.x += tDelta.x;
            } else if (tMax.y < tMax.z) {
                chunk.y += step.y;
                distanceTraveled = tMax.y;
                tMax.y += tDelta.y;
            } else {
                chunk.z += step.z;
                distanceTraveled = tMax.z;
                tMax.z += tDelta.z;
            }
        }
    }
};