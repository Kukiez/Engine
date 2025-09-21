#include "AABB.h"
#include "OOBB.h"
#include "Capsule.h"


std::array<glm::vec3, 8> OOBB::getCorners() const {
    std::array<glm::vec3, 8> corners{};
    const glm::vec3 axes[] = {
        orientation[0] * halfSize.x,
        orientation[1] * halfSize.y,
        orientation[2] * halfSize.z
    };

    size_t i = 0;
    for (int x = -1; x <= 1; x += 2)
        for (int y = -1; y <= 1; y += 2)
            for (int z = -1; z <= 1; z += 2) {
                corners[i++] = center + static_cast<float>(x) * axes[0] + static_cast<float>(y) * axes[1] + static_cast<float>(z) * axes[2];
            }

    return corners;
}


double AABB::timeSpentIntersecting = 0;