#pragma once
#include <Systems/Components/AIState.h>

class AIRandomStrollGoal final : public AIGoal {
    ComponentHandle<Transform> transform;
    ComponentHandle<Velocity> velocity;

    struct Goal {
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 radius = glm::vec3(0, 0, 0);
        double speed = 0.16;
        double time = 0;

        explicit Goal(const glm::vec3 radius) : position(1), radius(radius) {}
    } goal;

    inline static PoseID AI_MOVE_TO_POSITION_GOAL = MeshPose::hash("aiMoveToPositionGoal");
public:
    explicit AIRandomStrollGoal(const glm::vec3& radius) : goal(radius) {}

    void onEnter(Scene &scene, AIContext &ctx, const Entity& entity) override {
        transform = scene.getHandle<Transform>(entity);
        velocity = scene.getHandle<Velocity>(entity);
        goal.position = getRandomPositionAround(transform->translation, glm::vec3(5, 0, 5), glm::vec3(15, 0, 15));
        auto anyError = scene.getAnimationPlayer().play({"walk_anim", entity, BlendMode::MULTIPLY, FadeIn(), FadeOut(), true});

        if (anyError != AnimationStartError::NO_ERROR) {
            std::cout << animationStartErrorToString(anyError) << std::endl;
        }
    }

    void onExit(Scene &scene, AIContext&, const Entity& entity) override {
        velocity->aiVelocity = glm::vec3(0);
        auto& pose = scene.get<ModelPoseStack&>(entity);

        pose.removePose(0, AI_MOVE_TO_POSITION_GOAL, ModelPart::MESH);
        scene.getAnimationPlayer().cancel("walk_anim", entity, AnimationCancel::FADE_TO_IDENTITY);
        goal.time = 0;
    }

    void onTick(Scene& scene, AIContext &ctx, const Entity& entity) override {
        const glm::vec3 direction = goal.position - transform->translation;
        goal.time += scene.getDeltaTime();

        if (goal.time >= 5.0f) {
            setCompleted(true);
            return;
        }
        if (const float distance = length(direction); distance > 3) {
            glm::vec3 normalized = normalize(direction);
            normalized.y = 0.0f;
            velocity->aiVelocity = normalized * goal.speed;
            auto& pose = scene.get<ModelPoseStack&>(entity);
            MeshPose rotPose;
            rotPose.transform.rotation = Physics::rotateTowardsVelocity(velocity->aiVelocity);
            pose.setPose(0, AI_MOVE_TO_POSITION_GOAL, rotPose, MeshAffected::ROTATION, BlendMode::MULTIPLY, 1, ModelPart::MESH);
        } else {
            setCompleted(true);
        }
    }
};