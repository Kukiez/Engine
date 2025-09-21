#include "TerrainWorld.h"

#include <Collision/CollisionComponents.h>

#include "Math/Shapes/AABB.h"
#include <glm/glm.hpp>

#include "ECS/ECS.h"

TerrainWorld::Region::Region(const glm::ivec3 local) {
    matrices.worldTransform = glm::translate(glm::mat4(1.0f), glm::vec3(local) * static_cast<float>(REGION_SIZE));
    matrices.inverseWorldTransform = glm::inverse(matrices.worldTransform);
    matrices.normalMatrix = glm::transpose(glm::inverse(glm::mat3(matrices.worldTransform)));
}

void TerrainWorld::Region::addEntity(const Entity& entity, const AABB& aabb) {
    stash.emplace_back(aabb, entity);
    ++changes;
}

void TerrainWorld::Region::updateEntity(const Entity& entity, const AABB& aabb) {
    if (const auto result = bvh.find(entity)) {
        result.primitive->bounds = aabb;
        bvh.updateBounds(result.node);
        bvh.refitUpwards(result.node);
    } else {
        stash.emplace_back(aabb, entity);
    }
    ++changes;
}

void TerrainWorld::Region::build() {
    if (changes > 10) {
        if (bvh.empty()) {
            bvh.build(stash, 6);
        } else {
            bvh = bvh.rebuildWith(stash, 6);
        }
    } else if (bvh.empty()) {
        bvh.build(stash, 6);
    } else {
        for (const auto& entry : stash) {
            bvh.insert(entry);
        }
    }
    stash.clear();
    changes = 0;
}

void TerrainWorld::Region::refit()
{
    if (stash.empty()) {
        bvh = bvh.rebuild(6);
    } else {
        bvh = bvh.rebuildWith(stash, 6);
        stash.clear();
    }
    changes = 0;
}

void TerrainWorld::onLevelLoad(LevelLoadView<TerrainWorld> &level) {
    level.enableEventEmission<Collider>();
}

void TerrainWorld::onUpdate(LevelUpdateView<TerrainWorld>& level) {
    level.query<StaticCollider, Transform, AABBCollision>().forEachChangedOrNew<StaticCollider>(
        [&](const Entity e, const StaticCollider& collider, const Transform& transform, const AABBCollision& aabb)
    {
        Region & region = getRegion(transform.translation);
        const AABB bounds = geom::transform(aabb, transform.translation, transform.scale);

        region.updateEntity(e, bounds);

        dirtyRegions.emplace_back(worldToLocalCoords(transform.translation));
    });

    updateRegions();
    for (auto& region: regions | std::views::values) {
        if (region.changes > 0) {
            region.refit();
            return;
        }
    }
}

void TerrainWorld::updateRegions() {
    for (const auto& dirty : dirtyRegions) {
        auto& region = regions.at(dirty);
        region.build();
    }
    dirtyRegions.clear();
}

TerrainWorld::Region& TerrainWorld::getRegion(const glm::vec3& worldPos) {
    auto local = worldToLocalCoords(worldPos);

    const auto it = regions.find(local);
    if (it == regions.end())
        return regions.emplace(local, local).first->second;
    else
        return it->second;
}