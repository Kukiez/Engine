#include "PlayerController.h"

#include <Transform.h>
#include <Collision/CollisionComponents.h>
#include <Collision/PhysicsStage.h>

#include <Core/Input/InputEvents.h>
#include <util/glm_double.h>
#include <Renderer/Components/Components.h>

glm::vec3 getDirection(const PlayerComponent& player, const SDL_Keycode key) {
    const glm::vec3 forward = glm::normalize(glm::vec3(player.front.x, 0.0f, player.front.z));
    const glm::vec3 right   = glm::normalize(glm::vec3(player.right.x, 0.0f, player.right.z));

    switch (key) {
        case SDLK_w:
            return forward;
        case SDLK_s:
            return -forward;
        case SDLK_a:
            return -right;
        case SDLK_d:
            return right;
        default:
            return glm::vec3(0.0f);
    }
}

glm::vec3 pollKeyPressEvents(InputIn inputs, PlayerComponent& player) {
    glm::vec3 direction = glm::vec3(0.0f);

    for (auto event : inputs->keyPressEvents) {
        direction += getDirection(player, event.key);
    }

    direction.y = 0;
    return direction;
}

void PlayerController::onLevelLoad(LevelLoadView<PlayerController> level) {
    const float yaw = 0;
    const float pitch = 0;
    const glm::vec3 position = glm::vec3(0.0f, 60, 0);

    RigidBody body;
    body.onGround = false;

    std::cout << "t: " << Transform(position).translation << std::endl;
    auto [e, player, camera, vel, transform, dynC, aabb, metadata, rb]  = level.createEntityRet(
        PlayerComponent{yaw, pitch},
        CameraComponent(position, yaw, pitch),
        Velocity{},
        Transform{position},
        DynamicCollider(),
        AABBCollision(glm::vec3(0), glm::vec3(0.5, 1.33, 0.5)),
        ColliderMetadata(),
        body
    );
    playerEntity = e;
    std::cout << "TransformInit: "<< transform->translation << std::endl;
    level.createEntity(Skybox("assets/hdri/citrus_orchard_road_puresky_4k.exr"));
}

void PlayerController::movePlayer(PlayerView view, glm::vec3 direction) const {
    auto [transform, velocity, player, body] = view.get<Transform, Velocity, PlayerComponent, RigidBody>(playerEntity);

    const float len = glm::length(direction);
    if (len > 1e-6f) direction /= len;

    if (false) {
        const float a_ground = 20.0f;
        const float a_air    = 6.0f;
        const float v_max    = 6.0f;

        float a = body->onGround ? a_ground : a_air;

        auto accel = direction * a;
        accel.y = 0.0f;

        *velocity += glm::vec3(accel * PhysicsStage::Hz);

        const glm::vec3 horiz{velocity->x, 0.0f, velocity->z};
        const float speed = glm::length(horiz);
        if (speed > v_max) {
            const auto capped = (horiz / speed) * v_max;
            velocity->x = capped.x;
            velocity->z = capped.z;
        }
    }
    *velocity += glm::vec3(direction * 1.0f * PhysicsStage::Hz);
}

void PlayerController::onLevelIn(LevelInView<PlayerController> level, InputIn inputs) const {
    const auto dir = pollKeyPressEvents(inputs, *level.get<PlayerComponent>(playerEntity));
    bool moved = dir != glm::vec3(0);
    if (moved) {
        movePlayer(level, dir);
    }

    bool anyMotion = false;
    auto& player = *level.get<PlayerComponent>(playerEntity);

    for (auto& event : inputs->mouseMotionEvents) {
        if (anyMotion) return;
        float pitch = event.yrel * player.sensitivity.x;
        float yaw = event.xrel * player.sensitivity.y;
        player.pitch -= pitch;
        player.yaw += yaw;
        anyMotion = true;
    }

    if (anyMotion || moved) {
        auto& camera = *level.get<CameraComponent>(playerEntity);
        auto& transform = *level.get<Transform>(playerEntity);
        camera.move(transform.translation);
        player.rotate(player.yaw, player.pitch);
        camera.rotate(player.yaw, player.pitch);
        camera.updateViewMatrix();
    }
}
