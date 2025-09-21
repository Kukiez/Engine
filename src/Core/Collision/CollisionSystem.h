#pragma once
#include <ECS/ECS.h>

#include "PhysicsStage.h"

struct CollisionScanners : SystemPack {
    struct AABBScanner :
        PhysicsSystem,
        Writes<Transform>,
        Pipeline<CollisionScanners>,
        ReadsResources<TerrainWorld>,
        Dependencies<MovementSystem>,
        Reads<Transform, DynamicCollider, AABBCollision, ColliderMetadata>
    {
        struct CollisionCandidate {
            Entity entity;
            AABB worldAABB;
        };

        struct CollisionPair {
            CollisionCandidate first;
            Entity collided;
        };

        static auto make_candidate(Entity e, const AABB& worldAABB) {
            return CollisionCandidate{ e, worldAABB };
        }

        static ecs::frame_vector<CollisionPair> onPhysicsUpdate(PhysicsView<AABBScanner> view) {
            auto& world = view.get<TerrainWorld>();

            auto vec = view.allocate<std::vector, CollisionPair>(10);
            auto pairs = view.allocate<std::unordered_set, std::pair<Entity, Entity>>(10);

            view.query<Transform, DynamicCollider, AABBCollision, ColliderMetadata>().forEach(
                [&](const Entity entity, const Transform& transform, const DynamicCollider&,
                    const AABBCollision& aabb, const ColliderMetadata& metadata)
                {
                    const AABB worldAABB = aabb.world(transform);

                    auto query = world.query(worldAABB);

                    query.forEachIntersects([&](const TerrainWorld::WorldEntry& entry) {
                        if (entry.entity != entity) {
                            auto min = std::min(entity, entry.entity);
                            auto max = std::max(entity, entry.entity);
                            auto pair = std::make_pair(min, max);

                            if (pairs.contains(pair)) return;

                            vec.emplace_back(make_candidate(entity, worldAABB), entry.entity);
                            pairs.emplace(pair);
                        } else {
                        }
                    });
                }
            );
            return vec;
        }
    };

    using Pack = std::tuple<AABBScanner>;
};


struct CollisionNarrowPhase : SystemPack {
    struct Collision {
        Entity first, second;
        glm::vec3 mtv;
        glm::vec3 contactNormal = glm::vec3(0.f);
        float distance = 0.f;
    };

    struct AABBResolver :
        PhysicsSystem,
        ReadsResources<TerrainWorld>,
        Pipeline<CollisionNarrowPhase>,
        Reads<Transform, Collider, AABBCollision>
    {
        static auto make_collision(Entity first, Entity second, glm::vec3 mtv, glm::vec3 contactNormal, float distance) {
            return Collision{ first, second, mtv, contactNormal, distance };
        }

        static ecs::frame_vector<Collision> onPhysicsUpdate(PhysicsView<AABBResolver> view, In<CollisionScanners::AABBScanner> candidates) {
            auto& world = view.get<TerrainWorld>();

            auto collisions = view.allocate<std::vector, Collision>(candidates->size());

            for (const auto& [candidate, target] : *candidates) {
                if (const auto AABB = view.get<AABBCollision>(target)) {
                    auto targetWorldAABB = AABB->world(*view.get<Transform>(target));
                    auto [mtv, depth] = geom::mtv(candidate.worldAABB, targetWorldAABB);

                    glm::vec3 contactNormal = glm::length2(mtv) > 1e-6f ? glm::normalize(mtv) : glm::vec3(0.0f);
                    collisions.emplace_back(candidate.entity, target, mtv, contactNormal, depth);

                    if (view.has<PlayerComponent>(target) || view.has<PlayerComponent>(candidate.entity)) {
                        std::cout << "PLAYER COLLISION " << std::endl;
                    }
                    std::cout << "COLLISION " << target << " " << candidate.entity << std::endl;
                }
            }
            return collisions;
        }
    };

    using Pack = std::tuple<AABBResolver>;
};