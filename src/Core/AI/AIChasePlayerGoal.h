#pragma once
#include <Systems/Components/AIState.h>

class AIChasePlayerGoal final : public AIGoal {
    inline static PoseID AI_CHASE_PLAYER_GOAL = MeshPose::hash("aiChasePlayerGoal");
    ComponentHandle<Transform> playerPosition;

public:
    AIChasePlayerGoal() = default;

    void onEnter(Scene &scene, AIContext &ctx, const Entity &entity) override {
        playerPosition = scene.getHandle<Transform>(ctx.get("targetID").get<float>());
        scene.getAnimationPlayer().play({"walk_anim", entity, BlendMode::MULTIPLY, FadeIn(), FadeOut(0.2), true, 1, true});
    }

    void onTick(Scene &scene, AIContext &ctx, const Entity &entity) override {
        const glm::vec3 direction = playerPosition->translation - scene.get<Transform&>(entity).translation;
        glm::vec3 normalized = normalize(direction) * ctx.getOrCreate("chaseSpeed", 0.08f).get<float>();
        normalized.y = 0.0f;

        scene.get<Velocity&>(entity).aiVelocity = normalized;
        auto& pose = scene.get<ModelPoseStack&>(entity);
        MeshPose rotPose;
        rotPose.transform.rotation = Physics::rotateTowardsVelocity(normalized);
        pose.setPose(0, AI_CHASE_PLAYER_GOAL, rotPose, MeshAffected::ROTATION, BlendMode::MULTIPLY, 1, ModelPart::MESH);

        if (Physics::distance(scene, entity, ctx.get("targetID").get<float>()) <= 1.75f) {
            setCompleted(true);
        }
    }

    void onExit(Scene &scene, AIContext &, const Entity &entity) override {
        auto& pose = scene.get<ModelPoseStack&>(entity);
        pose.removePose(0, AI_CHASE_PLAYER_GOAL, ModelPart::MESH);
        scene.getAnimationPlayer().cancel("walk_anim", entity, AnimationCancel::FADE_TO_IDENTITY);
    }

    static AITransition getDefaultChasePlayerTransition() {
        return {"chasePlayer", [](Scene& scene, AIContext& ctx, const Entity& entity) {
            const float viewRange = ctx.getOrCreate("view_range", 33.0f).get<float>();
            ctx.getOrCreate("chaseSpeed", 0.08f);

            const auto& myTransform = scene.get<Transform&>(entity);

            for (const auto& [entity, player, playerTransform] : scene.getEntityManager().view<PlayerComponent, Transform>()) {
                if (distance(myTransform.translation, playerTransform.translation) <= viewRange) {
                    ctx.set("targetID", static_cast<float>(entity.id()));
                    return true;
                }
            }
            return false;
        }};
    }
};