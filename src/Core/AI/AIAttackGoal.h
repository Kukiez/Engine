#pragma once
#include <Systems/Components/AIState.h>
#include <Systems/Components/OtherComponents.h>

class AIBasicAttack final : public AIGoal {
    Entity animationEntity;
    int maxSlashes = 3;
    int currentSlash = 0;

    AnimationStartInfo attackAnimation{};
    AnimationStartInfo walkAnimation{};
public:
    static float getDistanceFromTarget(Scene& scene, const Entity entity, AIContext& ctx) {
        return Physics::distance(scene, entity, ctx.get("targetID").get<float>());
    }

    void onEnter(Scene &scene, AIContext &ctx, const Entity &entity) override {
        animationEntity = scene.getAnimationPlayer().getAnimationByName("slashHorizontal");
        attackAnimation = {animationEntity, entity, BlendMode::MULTIPLY, FadeIn(), FadeOut(0.1), false, 2.1, true};

        scene.getAnimationPlayer().play({"walk_anim", entity, BlendMode::MULTIPLY, FadeIn(), FadeOut(0.2), true, 0.75, true});

        scene.getAnimationPlayer().play(attackAnimation);
        ++currentSlash;

        const glm::vec3 direction = scene.get<Transform&>(ctx.get("targetID").get<float>()).translation - scene.get<Transform&>(entity).translation;
        glm::vec3 normalized = normalize(direction);
        normalized.y = 0.0f;

        scene.get<Velocity&>(entity).aiVelocity = normalized * 0.025f;
        MeshPose rotPose;
        rotPose.transform.rotation = Physics::rotateTowardsVelocity(normalized);
        auto& pose = scene.get<ModelPoseStack&>(entity);
        pose.setPose(0, 914141415, rotPose, MeshAffected::ROTATION, BlendMode::MULTIPLY, 1, ModelPart::MESH);

        scene.get<DamageHitbox&>(scene.get<ChildrenComponent&>(entity).children["DamageHitbox"]).setEnabled(true);
    }

    void onTick(Scene &scene, AIContext &ctx, const Entity &entity) override {
        const double progress = scene.getAnimationPlayer().getAnimationProgress(animationEntity, entity);

        if (math::within(progress, 0.7, 0.9) && currentSlash < maxSlashes) {
            if (getDistanceFromTarget(scene, entity, ctx) < 4.0f) {
                scene.getAnimationPlayer().play(attackAnimation);
                ++currentSlash;
            }
        } else if (progress == 1)
            setCompleted(true);
    }

    void onExit(Scene &scene, AIContext &ctx, const Entity &entity) override {
        scene.get<ModelPoseStack&>(entity).removePose(0, 914141415, ModelPart::MESH);
        currentSlash = 0;
        scene.get<DamageHitbox&>(scene.get<ChildrenComponent&>(entity).children["DamageHitbox"]).setEnabled(false);
    }

    static AITransition getDefaultTransition() {
        return {"basicAttack", [](Scene& scene, AIContext& ctx, const Entity& entity) {
            if (Physics::distance(scene, entity, ctx.get("targetID").get<float>()) <= 1.75f) {
                return true;
            }
            return false;
        }};
    }
};