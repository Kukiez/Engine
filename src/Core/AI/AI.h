#pragma once

#pragma once
#include <Systems/PhysicsSystem.h>
#include <Systems/Mesh/ModelAnimationPlayer.h>
#include <Systems/Mesh/ModelPose.h>
#include "UIComponents.h"

#include <util/Random.h>
#include <util/stl.h>
#include <Systems/Components/CameraComponent.h>

inline glm::vec3 getRandomPositionAround(
    const glm::vec3& position,
    const glm::vec3& minRadius,
    const glm::vec3& maxRadius
) {
    auto randomOffsetForAxis = [](const float minR, const float maxR) -> float {
        const float sign = (Random::Float(0.0f, 1.0f) < 0.5f) ? -1.0f : 1.0f;
        const float val = Random::Float(minR, maxR);
        return sign * val;
    };

    const float xOffset = randomOffsetForAxis(minRadius.x, maxRadius.x);
    const float yOffset = randomOffsetForAxis(minRadius.y, maxRadius.y);
    const float zOffset = randomOffsetForAxis(minRadius.z, maxRadius.z);

    return position + glm::vec3{xOffset, yOffset, zOffset};
}

class AIContext {
    stl::unordered_stringmap<Uniform> memory;

public:
    AIContext() = default;

    void set(const std::string_view key, const Uniform &value) {
        memory[key.data()] = value;
    }

    Uniform& get(const std::string_view item) {
        if (const auto it = memory.find(item); it != memory.end()) {
            return it->second;
        }
        return memory.emplace(item, Uniform{}).first->second;
    }

    bool has(const std::string_view item) const {
        return memory.contains(item);
    }

    Uniform& getOrCreate(const std::string_view item, Uniform&& value) {
        if (const auto it = memory.find(item); it != memory.end()) {
            return it->second;
        }
        return memory.emplace(item, value).first->second;
    }
};

class AIGoal : public IComponentData<"AIGoal"> {
protected:
    bool isFinished = false;
public:
    virtual ~AIGoal() = default;

    virtual void onExit(Scene& scene, AIContext&, const Entity& entity) = 0;
    virtual void onEnter(Scene& scene, AIContext &ctx, const Entity& entity) = 0;
    virtual void onTick(Scene& scene, AIContext &ctx, const Entity& entity) = 0;

    void setCompleted(const bool completed) { isFinished = completed; }
    bool isCompleted() const { return isFinished; }
};

using AITransitionCondition = bool(*)(Scene&, AIContext&, const Entity&);

struct AITransition {
    std::string targetGoal;
    AITransitionCondition condition;

    AITransition(std::string targetGoal, const AITransitionCondition condition) : targetGoal(std::move(targetGoal)), condition(condition) {}
};

class AIVoidGoal final : public AIGoal {
public:
    AIVoidGoal() {
        isFinished = true;
    }

    void onEnter(Scene& scene, AIContext &ctx, const Entity& entity) override {}
    void onTick(Scene& scene, AIContext &ctx, const Entity& entity) override {}
    void onExit(Scene& scene, AIContext&, const Entity& entity) override {}
};

struct AINullState_t {};

constexpr static auto AI_NULL_STATE = AINullState_t{};
constexpr static auto AI_NULL_GOAL_NAME = "__NULLGOAL__";
static inline auto AI_NULL_GOAL = AIVoidGoal();

struct AIState {
    std::string name;
    std::vector<AITransition> transitions;
    AIGoal* goal;

    explicit AIState(std::string name, AIGoal* goal) : name(std::move(name)), goal(goal) {}

    AIState() : name(AI_NULL_GOAL_NAME), goal(&AI_NULL_GOAL) {}

    AIState& addTransition(std::string transition, AITransitionCondition condition) {
        transitions.emplace_back(std::move(transition), condition);
        return *this;
    }

    AIState& addTransition(AITransition&& transition) {
        transitions.emplace_back(std::move(transition));
        return *this;
    }

    AIState& setGoal(AIGoal* goal) {
        this->goal = goal;
        return *this;
    }

    bool operator != (AINullState_t) const {
        return name != AI_NULL_GOAL_NAME;
    }

    bool operator == (AINullState_t) const {
        return name == AI_NULL_GOAL_NAME;
    }
};

class AIController : public IComponentData<"AIController"> {
    static inline AIState INVALID_STATE{};

    static size_t hash(const std::string_view str) {
        constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ull;

        uint64_t hash = FNV_OFFSET_BASIS;
        for (const char c : str) {
            constexpr uint64_t FNV_PRIME = 1099511628211ull;
            hash ^= static_cast<uint64_t>(c);
            hash *= FNV_PRIME;
        }
        return hash;
    }
    std::unordered_map<size_t, AIState> states;
    AIContext context;

    size_t currentState;
    size_t fallbackState;
    bool active = true;
public:
    AIController() = default;

    AIState& addState(const std::string_view name, AIGoal* goal) {
        assert(!states.contains(hash(name))); // Hash Collision, Try renaming a State

        return states.emplace(std::piecewise_construct,
            std::forward_as_tuple(hash(name)),
            std::forward_as_tuple(name.data(), goal)
        ).first->second;
    }

    bool update(Scene& scene, const Entity& entity) {
        const auto it = states.find(currentState);
        it->second.goal->onTick(scene, context, entity);
        return it->second.goal->isCompleted();
    }

    void setFallbackState(const std::string_view fallback) {
        fallbackState = hash(fallback);
    }

    void setActive(const bool active) {
        this->active = active;
    }

    bool isActive() const {
        return active;
    }

    const AIState& getState(const std::string_view state) {
        if (const auto it = states.find(hash(state)); it != states.end()) {
            return it->second;
        }
        if (const auto it = states.find(fallbackState); it != states.end()) {
            return it->second;
        }
        assert(false); // should never reach
        std::unreachable();
    }

    const AIState& getNextGoal(Scene& scene, const Entity entity) {
        for (const auto& [targetGoal, condition] : states[currentState].transitions) {
            if (condition(scene, context, entity)) {
                if (const auto it = states.find(hash(targetGoal)); it != states.end()) {
                    return it->second;
                }
            }
        }
        if (const auto it = states.find(fallbackState); it != states.end()) {
            return it->second;
        }
        assert(false);
        return INVALID_STATE;
    }

    void exitCurrentState(Scene& scene, const Entity entity) {
        std::cout << "Exiting: " << states[currentState].name << std::endl;
        auto& current = states.at(currentState);
        current.goal->onExit(scene, context, entity);
        current.goal->setCompleted(false);
    }

    void enterState(Scene& scene, const Entity entity, const AIState& state) {
        std::cout << "Entering: " << state.name << std::endl;
        state.goal->onEnter(scene, context, entity);
        currentState = hash(state.name);
    }
};