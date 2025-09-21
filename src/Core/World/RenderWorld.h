#pragma once

class RenderWorld : Reads<Model, Transform> {
public:
    FRIEND_DESCRIPTOR

    struct Region {
        struct RegionBVHMatrices {
            glm::mat4 worldTransform{};
            glm::mat4 inverseWorldTransform{};
            glm::mat4 normalMatrix{};
        };

        struct Node {
            AABB bounds;
            Entity entity;

            operator const AABB&() const {
                return bounds;
            }
        };

        RegionBVHMatrices matrices;
        BVH<Node> bvh;

        Region(const glm::ivec3 local) {
            matrices.worldTransform = glm::translate(glm::mat4(1.0f), glm::vec3(local) * static_cast<float>(REGION_SIZE));
            matrices.inverseWorldTransform = glm::inverse(matrices.worldTransform);
            matrices.normalMatrix = glm::transpose(glm::inverse(glm::mat3(matrices.worldTransform)));
        }
    };

    std::unordered_map<WorldRegionID, Region> regions;
    std::vector<WorldRegionID> dirtyRegions;

    static constexpr int REGION_SIZE = 64;

    static WorldRegionID worldToLocalCoords(const glm::vec3& worldPos) {
        return {
            std::floor(worldPos.x / REGION_SIZE),
            std::floor(worldPos.y / REGION_SIZE),
            std::floor(worldPos.z / REGION_SIZE)
        };
    }

    Region& getRegion(const glm::vec3& worldPos) {
        auto local = worldToLocalCoords(worldPos);

        const auto it = regions.find(local);
        if (it == regions.end())
            return regions.emplace(local, local).first->second;
        else
            return it->second;
    }

    void onUpdate(LevelUpdateView<RenderWorld>& view) {
        view.query<Model, Transform>().forEachChanged<Transform>(
            [&](const Entity e, const Model& model, const Transform& transform) {

        });
    }
};