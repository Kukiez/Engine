#pragma once
#include <ECS/ECS.h>
#include <glm/vec3.hpp>
#include <Input/InputSystem.h>
#include <Renderer/Common.h>

struct CameraComponent;
struct Velocity;
struct Transform;
struct PlayerComponent;
struct RigidBody;
struct KeyPressEvent;
struct MouseMotionEvent;
struct MouseButtonEvent;

using PlayerView = View<Transform, Velocity, PlayerComponent, const RigidBody>;

class PlayerController : LevelInStage::Reads<RigidBody>, LevelInStage::Writes<Transform, Velocity, PlayerComponent, CameraComponent>, Stages<LevelInStage> {
    FRIEND_DESCRIPTOR

    Entity playerEntity;
public:
    void onLevelLoad(LevelLoadView<PlayerController> level);

    void movePlayer(PlayerView view, const glm::vec3 dir) const;

    void onLevelIn(LevelInView<PlayerController> level, InputIn inputs) const;
};