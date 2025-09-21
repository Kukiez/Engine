//
// Created by dkuki on 4/16/2025.
//

#include "Ray.h"

#include <glm/gtx/component_wise.inl>

#include "AABB.h"
#include <util/glm_double.h>

RayResult Ray::intersects(const glm::vec3 &center, float radius) const {
    glm::vec3 oc = origin - center;

    float a = glm::dot(direction, direction);
    float b = 2.0f * glm::dot(oc, direction);
    const float c = glm::dot(oc, oc) - radius * radius;
    const float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f) {
        return {};
    }

    const float sqrtD = sqrt(discriminant);
    const float t0 = (-b - sqrtD) / (2.0f * a);
    const float t1 = (-b + sqrtD) / (2.0f * a);

    const float tHit = (t0 >= 0.0f) ? t0 : ((t1 >= 0.0f) ? t1 : -1.0f);
    if (tHit < 0.0f || tHit > length) {
        return {};
    }

    const glm::vec3 pos = origin + direction * tHit;

    std::cout << "Pos: " << pos << std::endl;
    std::cout << "Dir: " << direction << std::endl;
    std::cout << "Length: " << length << std::endl;
    std::cout << "Hit: " << tHit << std::endl;
    return {
        .hitPos = pos,
        .normal = glm::normalize(pos - center),
        .distance = tHit
    };
}

RayResult Ray::intersects(const AABB& aabb) const {
    const glm::vec3 min = aabb.min();
    const glm::vec3 max = aabb.max();

    glm::vec3 invDir = 1.0f / direction;
    glm::vec3 tMin = (min - origin) * invDir;
    glm::vec3 tMax = (max - origin) * invDir;

    glm::vec3 t1 = glm::min(tMin, tMax);
    glm::vec3 t2 = glm::max(tMin, tMax);

    float tNear = glm::compMax(t1);
    float tFar = glm::compMin(t2);

    if (tNear > tFar || tFar < 0.0f || tNear > length) {
        return {};
    }
    float hitDist = glm::max(tNear, 0.0f);

    glm::vec3 hitPoint = origin + direction * hitDist;

    glm::vec3 normal{0.0f};

    if (constexpr float epsilon = 0.01f; std::abs(hitPoint.x - min.x) < epsilon) normal = glm::vec3(-1, 0, 0);
    else if (std::abs(hitPoint.y - min.y) < epsilon) normal = glm::vec3(0, -1, 0);
    else if (std::abs(hitPoint.y - max.y) < epsilon) normal = glm::vec3(0, 1, 0);
    else if (std::abs(hitPoint.x - max.x) < epsilon) normal = glm::vec3(1, 0, 0);
    else if (std::abs(hitPoint.z - min.z) < epsilon) normal = glm::vec3(0, 0, -1);
    else if (std::abs(hitPoint.z - max.z) < epsilon) normal = glm::vec3(0, 0, 1);

    return { hitPoint, normal, hitDist};
}
