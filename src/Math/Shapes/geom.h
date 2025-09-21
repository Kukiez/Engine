#pragma once
#include "Capsule.h"
#include "OOBB.h"
#include "AABB.h"
#include "Ray.h"

namespace geom {
    struct MTVResult {
        glm::vec3 direction;
        float depth;
    };

    OOBB toOOBB(const Capsule& cap);
    OOBB toOOBB(const AABB& aabb);

    AABB toAABB(const OOBB& oobb);
    AABB toAABB(const Capsule& cap);

    bool intersects(const AABB& aabb1, const AABB& aabb2);
    bool intersects(const OOBB& oobb1, const OOBB& oobb2);
    bool intersects(const Capsule& cap1, const Capsule& cap2);

    /* tests if oobb1 contains oobb2 */
    bool contains(const OOBB& oobb1, const OOBB& oobb2);
    bool contains(const AABB& aabb1, const AABB& aabb2);

    float projectExtent(const OOBB& oobb, glm::vec3 extent);

    MTVResult mtv(const OOBB& oobb1, const OOBB& oobb2);
    MTVResult mtv(const AABB& aabb1, const AABB& aabb2);

    glm::vec3 min(const AABB& aabb);
    glm::vec3 max(const AABB& aabb);

    AABB aabb_cast(const OOBB& oobb);
    OOBB oobb_cast(const AABB& aabb);

    AABB merge(const AABB& aabb1, const AABB& aabb2);

    glm::vec3 centroid(const AABB& aabb1);

    glm::vec3 extent(const AABB& aabb);
    float surface_area(const AABB& aabb);

    AABB inverse_transform(const AABB& worldAABB, const glm::mat4& worldTransform);

    struct transformer {
        AABB operator () (const AABB& aabb, const glm::mat4& worldTransform) const;
        AABB operator () (const AABB& aabb, const glm::vec3& position, const glm::vec3& scale = glm::vec3(1.f)) const;
        Ray operator () (const Ray& ray, const glm::mat4& worldTransform) const;

        OOBB operator () (const OOBB& oobb, const glm::mat4& worldTransform) const;
    };

    constexpr static transformer transform{};

    struct hash {
        static size_t hash_float(const float f) {
            uint32_t bits;
            std::memcpy(&bits, &f, sizeof(float));
            return std::hash<uint32_t>{}(bits);
        }

        static size_t vec3_hash(const glm::vec3 v) {
            size_t h = 0;
            auto hash_combine = [&h](size_t val) {
                h ^= val + 0x9e3779b9 + (h << 6) + (h >> 2);
            };
            hash_combine(hash_float(v.x));
            hash_combine(hash_float(v.y));
            hash_combine(hash_float(v.z));
            return h;
        }

        size_t operator() (const AABB& aabb) const noexcept {
            size_t h = 0;
            auto hash_combine = [&h](size_t val) {
                h ^= val + 0x9e3779b9 + (h << 6) + (h >> 2);
            };
            hash_combine(vec3_hash(aabb.center));
            hash_combine(vec3_hash(aabb.halfSize));
            return h;
        }
    };
}
