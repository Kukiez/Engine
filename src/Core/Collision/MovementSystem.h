#pragma once
#include "PhysicsStage.h"

struct SlowdownSystem : Reads<RigidBody>, Writes<Velocity>, Stages<PhysicsStage> {
    static void onPhysicsUpdate(PhysicsView<SlowdownSystem> level) {
        level.query<Velocity, RigidBody>().forEach([](const Entity e, Velocity& velocity, const RigidBody& body) {
            if (!body.onGround) {
                velocity.y -= 0.35f * PhysicsStage::Hz;

                float horizontalSpeed = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
                float maxAirSpeed = .15f;

                if (horizontalSpeed > maxAirSpeed) {
                    float scale = maxAirSpeed / horizontalSpeed;
                    velocity.x *= 1.f - scale;
                    velocity.z *= 1.f - scale;
                }
            } else {
                const float decay = std::exp(-body.damping * PhysicsStage::Hz);
                velocity.x *= decay;
                velocity.y *= decay;
                velocity.z *= decay;
            }
            constexpr float epsilon = 0.0001f;
            if (std::abs(velocity.x) < epsilon) velocity.x = 0.0f;
            if (std::abs(velocity.y) < epsilon) velocity.y = 0.0f;
            if (std::abs(velocity.z) < epsilon) velocity.z = 0.0f;
        });
    }
};

struct MovementSystem : Reads<Velocity>, Writes<Transform>, Dependencies<SlowdownSystem>, Stages<PhysicsStage> {
    void onPhysicsUpdate(PhysicsView<MovementSystem> level) {
        level.query<Transform, Velocity>().forEach([](const Entity e, Transform& transform, const Velocity& velocity) {
            transform.translation += velocity * PhysicsStage::Hz;
        });
    }
};

struct CameraMovementSystem : Writes<CameraComponent>, Reads<Transform> {
    void onUpdate(LevelUpdateView<CameraMovementSystem> level) {
        level.query<Transform, CameraComponent>().forEach([](const Entity e, const Transform& transform, CameraComponent& camera) {
            camera.position = transform.translation;
            camera.updateViewMatrix();
        });
    }
};