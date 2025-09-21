#pragma once
#include "ECS/Component/Component.h"
#include "Math/BoundingVolumeHierarchy.h"
#include "Math/Shapes/AABB.h"
#include "Math/Shapes/geom.h"
#include "Math/Shapes/OOBB.h"
#include <Core/Transform.h>

struct Collider : PrimaryComponent, TrackedComponent {
    bool isStatic = false;
};

struct StaticCollider : PrimaryComponent, TrackedComponent {

};

struct DynamicCollider : PrimaryComponent {

};

struct CollisionLayer : std::bitset<32> {
};

struct ColliderMetadata : PrimaryComponent {
    CollisionLayer mask;
    CollisionLayer collidesWithMask;

    bool collidesWith(const ColliderMetadata& other) const {
        return (mask & other.collidesWithMask| other.mask & collidesWithMask) != 0;
    }
};

struct TerrainWorldMetadata : SecondaryComponent {
};

struct AABBCollision : PrimaryComponent, AABB {

    AABBCollision() = default;
    AABBCollision(const glm::vec3& centerOffset, glm::vec3 extents) : AABB(centerOffset, extents / 2.f) {}

    AABB world(const Transform& worldTransform) const {
        return geom::transform(*this, worldTransform.translation, glm::vec3(1));
    }

    const AABB& boundingBox() const {
        return *this;
    }
};

struct OOBBCollision : PrimaryComponent, OOBB {
    OOBB world(const Transform& worldTransform) const {
        return geom::transform(*this, worldTransform.createModel3D());
    }

    const AABB& boundingBox() const {

    }
};

enum class CollisionMeshType {
    NO_MESH,
    BVH_TRIANGLE_MESH,
    BVH_AABB_MESH
};

template <typename Geom>
struct BVHCollisionNode {
    Geom geom;
    int info = 0;

    BVHCollisionNode() = default;
    BVHCollisionNode(const Geom& geom, int info = 0) : geom(geom), info(info) {}

    operator const Geom& () const {
        return geom;
    }
};

using CollisionMeshVariant = std::variant<BVH<BVHCollisionNode<AABB>>>;

struct BoundsVisitor {
    const AABB& operator () (const auto& bvh) const {
        return bvh.bounds();
    }
};

template<class... Ts> struct RayOverloads : Ts... { using Ts::operator()...; };
template<class... Ts> RayOverloads(Ts...) -> RayOverloads<Ts...>;

template <typename Overloads>
struct RayVisitor {
    Overloads& overloads;
    const Ray& ray;
    const glm::mat4& worldTransform;

    RayVisitor(const Ray& ray, const glm::mat4& worldTransform, Overloads& overloads) : overloads(overloads), ray(ray), worldTransform(worldTransform) {}

    void operator () (const BVH<BVHCollisionNode<AABB>>& bvh) const {
        bvh.forEachRayIntersects(ray, worldTransform, [this](const BVHRayResult<BVHCollisionNode<AABB>>& node) {
            if constexpr (std::is_invocable_r_v<bool, Overloads, const BVHRayResult<BVHCollisionNode<AABB>>&>)
                return overloads(node);
            else if constexpr (std::is_invocable_r_v<bool, Overloads, const RayResult&>) {
                return overloads(static_cast<const RayResult&>(node));
            } else {
                static_assert(false, "No overload found for RayVisitor");
            }
        });
    }
};

template <typename T>
concept CollisionMeshCompatible = std::is_same_v<std::decay_t<T>, BVH<BVHCollisionNode<AABB>>>;

template <typename... Args>
concept CollisionMeshConstructible = std::constructible_from<BVH<BVHCollisionNode<AABB>>, Args...>;

struct CollisionMesh : PrimaryComponent {
    using BVH_AABB = BVH<BVHCollisionNode<AABB>>;

    CollisionMeshVariant mesh;
    CollisionMeshType type;

    template <typename... Args>
    requires CollisionMeshConstructible<Args...>
    CollisionMesh(Args&&... args) : mesh(std::forward<Args>(args)...) {
        if constexpr (std::constructible_from<BVH<BVHCollisionNode<AABB>>, Args...>) {
            type = CollisionMeshType::BVH_AABB_MESH;
        }
    }

    template <CollisionMeshCompatible Variant>
    CollisionMesh& operator = (Variant&& var) {
        using Type = std::decay_t<Variant>;
        mesh = std::forward<Variant>(var);

        if constexpr (std::is_same_v<Type, BVH_AABB>) {
            type = CollisionMeshType::BVH_AABB_MESH;
        }
        return *this;
    }

    bool holdsMesh() const {
        return type != CollisionMeshType::NO_MESH;
    }

    AABB bounds() const {
        return std::visit(BoundsVisitor{}, mesh);
    }

    template <typename RayOverloads>
    void raycast(const Ray& ray, const glm::mat4& meshWorldTransform, RayOverloads&& overloads) const {
        std::visit(RayVisitor{ray, meshWorldTransform, overloads}, mesh);
    }
};


struct RigidBody : PrimaryComponent {
    float mass = 1.0f;
    float gravity = 0.35f;
    float airDrag = 170.f;
    float damping = 5.f;
    bool onGround = false;
    float maxVelocity = 0.1f;
};

