#pragma once
#include "Transform.h"
#include "ECS/Component/Component.h"
#include "glm/fwd.hpp"

struct NodeTransform {
    Transform local;
    float forward;
    float side;
    float up;

    explicit NodeTransform(const Transform &local, const float forward = 0.0f,
        const float side = 0.0f, const float up = 0.0f) : local(local), forward(forward), side(side), up(up) {}

    explicit NodeTransform(const glm::vec3& pos = glm::vec3(0), const glm::vec3& scale = glm::vec3(1),
        const glm::vec3& euler = glm::vec3(0), const float forward = 0.0f, const float side = 0.0f, const float up = 0.0f)
        : local(pos, scale, euler), forward(forward), side(side), up(up) {}
};

struct SceneNode : PrimaryComponent {
    Entity parent = NullEntity;
    std::vector<Entity> children;

    NodeTransform local;
    glm::mat4 finalTransform;
    unsigned version;

    SceneNode(const Entity parent, const NodeTransform& local)
        : parent(parent), local(local), finalTransform(glm::mat4(1.0f)), version(0) {}
};
