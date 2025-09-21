#pragma once
#include <Systems/Components/AIState.h>

class AIIdleGoal final : public AIGoal {
    double time = 0;
    double duration = 0;
public:
    explicit AIIdleGoal(const double duration) : duration(duration) {}

    void onEnter(Scene& scene, AIContext &ctx, const Entity& entity) override {

    }

    void onTick(Scene& scene, AIContext &ctx, const Entity& entity) override {
        time += scene.getDeltaTime();

        if (time >= duration) {
            setCompleted(true);
        }


    }

    void onExit(Scene& scene, AIContext&, const Entity& entity) override {
        time = 0;
    }
};