#pragma once
#include "ECS/Component/Component.h"
#include "glm/fwd.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>


enum class LightType {
    POINT_LIGHT,
    SPOT_LIGHT,
    DIRECTIONAL_LIGHT
};

struct PointLight : PrimaryComponent {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    float radius;

    PointLight(const glm::vec3 position, const glm::vec3 color, const float intensity, const float radius)
        : position(position), color(color), intensity(intensity), radius(radius) {}
};

struct SpotLight : PrimaryComponent {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float range = 50.f;
    float innerCutoff;
    float outerCutoff;

    SpotLight(const glm::vec3 position, const glm::vec3 direction, const glm::vec3 color, const float intensity, const float innerAngle, const float outerAngle)
        : position(position), direction(direction), color(color), intensity(intensity), innerCutoff(glm::cos(glm::radians(innerAngle))), outerCutoff(glm::cos(glm::radians(outerAngle))) {}
};


struct DirectionalLight : PrimaryComponent {
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;

    DirectionalLight(const glm::vec3 direction, const glm::vec3 color, const float intensity)
        : direction(direction), color(color), intensity(intensity) {}
};