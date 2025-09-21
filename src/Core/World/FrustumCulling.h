#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <util/glm_double.h>

#include "TerrainWorld.h"

enum class FrustumPlane {
    LEFT, RIGHT,
    BOTTOM, TOP,
    NEAR_PLANE, FAR_PLANE
};

struct Frustum {
    glm::vec4 planes[6];

    void extract(const glm::mat4& m) {
        // Left
        planes[0] = glm::vec4(
            m[0][3] + m[0][0],
            m[1][3] + m[1][0],
            m[2][3] + m[2][0],
            m[3][3] + m[3][0]);

        // Right plane = row 3 - row 0
        planes[1] = glm::vec4(
            m[0][3] - m[0][0],
            m[1][3] - m[1][0],
            m[2][3] - m[2][0],
            m[3][3] - m[3][0]);

        // Bottom plane = row 3 + row 1
        planes[2] = glm::vec4(
            m[0][3] + m[0][1],
            m[1][3] + m[1][1],
            m[2][3] + m[2][1],
            m[3][3] + m[3][1]);

        // Top plane = row 3 - row 1
        planes[3] = glm::vec4(
            m[0][3] - m[0][1],
            m[1][3] - m[1][1],
            m[2][3] - m[2][1],
            m[3][3] - m[3][1]);

        // Near plane = row 3 + row 2
        planes[4] = glm::vec4(
            m[0][3] + m[0][2],
            m[1][3] + m[1][2],
            m[2][3] + m[2][2],
            m[3][3] + m[3][2]);

        // Far plane = row 3 - row 2
        planes[5] = glm::vec4(
            m[0][3] - m[0][2],
            m[1][3] - m[1][2],
            m[2][3] - m[2][2],
            m[3][3] - m[3][2]);

        // Normalize
        for (int i = 0; i < 6; ++i) {
            float len = glm::length(glm::vec3(planes[i]));
            planes[i] /= len;
        }
    }
};

class FrustumCullingSystem : public DefaultStage::Reads<CameraComponent>, public DefaultStage::ReadsResources<TerrainWorld>, public ResourceSystem<>, public Stages<DefaultStage> {
    Frustum frustum{};

    std::unordered_set<glm::ivec3, IVec3Hash> visibleChunks;
    std::vector<std::pair<glm::ivec3, bool>> chunkChanges;
public:
    void onUpdate(LevelUpdateView<FrustumCullingSystem> view) {
        chunkChanges.clear();

        view.query<CameraComponent>().forEach([&](const Entity e, const CameraComponent& camera) {
            glm::mat4 viewProjection = camera.projection * camera.view;
            frustum.extract(viewProjection);

            for (auto& [coords, chunk] : view.get<TerrainWorld>().getRegions()) {
                if (isAABBInsideFrustum(chunk.bounds()) && isInsideViewDistance(camera.position, chunk.bounds(), camera.viewDistance)) {
                    if (visibleChunks.insert(coords).second) {
                        chunkChanges.emplace_back(coords, true);
                    }
                } else if (visibleChunks.erase(coords)) {
                    chunkChanges.emplace_back(coords, false);
                }
            }
        });
        std::sort(chunkChanges.begin(), chunkChanges.end(), [](const std::pair<glm::ivec3, bool>& a, const auto& b){
            return a.second < b.second;
        });
    }

    bool isInsideViewDistance(const glm::vec3& origin, const AABB& box, const float viewDistance) {
        glm::vec3 clampedPoint = glm::clamp(origin, box.min(), box.max());
        const float d = glm::length(origin - clampedPoint);

        return d <= viewDistance;
    }

    bool isFullyInsideViewDistance(const glm::vec3& cameraPos, const AABB& box, float viewDistance) {
        glm::vec3 corners[8] = {
            { box.min().x, box.min().y, box.min().z },
            { box.min().x, box.min().y, box.max().z },
            { box.min().x, box.max().y, box.min().z },
            { box.min().x, box.max().y, box.max().z },
            { box.max().x, box.min().y, box.min().z },
            { box.max().x, box.min().y, box.max().z },
            { box.max().x, box.max().y, box.min().z },
            { box.max().x, box.max().y, box.max().z },
        };
        for (const auto& corner : corners) {
            float dist = glm::distance(cameraPos, corner);
            if (dist > viewDistance) {
                return false;
            }
        }
        return true;
    }

    bool intersectsPlane(const AABB& box, const FrustumPlane plane) const {
        const glm::vec4 normal = frustum.planes[static_cast<int>(plane)];

        const float r =
            box.halfSize.x * std::abs(normal.x) +
            box.halfSize.y * std::abs(normal.y) +
            box.halfSize.z * std::abs(normal.z);
        const float s = glm::dot(glm::vec3(normal), box.center) + normal.w;

        return std::abs(s) <= r;
    }

    bool intersectsAnyPlane(const AABB& box) const {
        for (int i = 0; i < 6; ++i) {
            if (intersectsPlane(box, FrustumPlane{i})) {
                return true;
            }                   
        }
        return false;
    }
    
    bool isAABBInsideFrustum(const AABB& box) const {
        for (int i = 0; i < 6; ++i) {
            auto& plane = frustum.planes[i];

            glm::vec3 axisVert;

            axisVert.x = plane.x < 0.0f ? box.min().x : box.max().x;
            axisVert.y = plane.y < 0.0f ? box.min().y : box.max().y;
            axisVert.z = plane.z < 0.0f ? box.min().z : box.max().z;

            if (dot(glm::vec3(plane), axisVert) + plane.w < 0.0f) {
                return false;
            }
        }
        return true;
    }

    const std::unordered_set<glm::ivec3, IVec3Hash>& getVisibleChunks() const {
        return visibleChunks;
    }

    const std::vector<std::pair<glm::ivec3, bool>>& getChanges() const {
        return chunkChanges;
    }
};