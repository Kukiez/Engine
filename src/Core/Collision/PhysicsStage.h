#pragma once
#include <ECS/System/ISystem.h>

template <typename> struct PhysicsView;

struct PhysicsStage : Stage<PhysicsStage> {
    using stage = PhysicsStage;

    template <typename T>
    static constexpr auto Function = &T::onPhysicsUpdate;

    template <typename T>
    constexpr static bool HasFunction = requires
    {
        &T::onPhysicsUpdate;
    };

    static constexpr auto ExecutionModel = StageExecutionModel::DETERMINISTIC;
    static constexpr auto ScheduleModel = StageScheduleModel::FIXED_HZ;
    static constexpr auto Hz = 1.f / 60.f;

    template <typename System>
    using StageView = PhysicsView<System>;
    /* Deterministic: LevelUpdateView,
     * Serial: LevelSerialView,
     * Parallel: LevelParallelView,
     */

    struct OnInherit {
        template <typename S>
        using In = ::In<PhysicsStage, S>;

        static float getDeltaTime() {
            return Hz;
        }
    };
    // OnStageEnd, onStageBegin, ShouldRun, GetHz
};

template <typename S>
struct PhysicsView : public LevelDeterministicView<PhysicsStage, S> {
    using LevelDeterministicView<PhysicsStage, S>::LevelDeterministicView;
};

struct PhysicsSystem : Stages<PhysicsStage> {};