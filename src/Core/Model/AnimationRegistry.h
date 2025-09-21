#pragma once
#include "ModelAnimation.h"
#include "ECS/System/ISystem.h"

enum class AnimationKey : unsigned {};

struct AnimationMetadata {
    ModelAnimation animation;
    AnimationKey key;

    auto& getChannels() const {
        return animation.getChannels();
    }

    auto duration() const {
        return animation.getDuration();
    }
};

class AnimationRegistry : public ResourceSystem<> {
    auto newKey() {
        return AnimationKey{nextKey++};
    }

    mem::vector<AnimationMetadata> animations;
    std::unordered_map<std::string, size_t> animationsByString;
    std::unordered_map<AnimationKey, size_t> animationsByKey;
    unsigned nextKey = 0;
public:
    void addAnimation(ModelAnimation&& animation) {
        if (const auto it = animationsByString.find(animation.getName()); it != animationsByString.end()) {
            animations[it->second].animation = std::move(animation);
        } else {
            AnimationKey key = newKey();
            animations.emplace_back(std::move(animation));
            animationsByString.emplace(animation.getName(), animations.size() - 1);
            animationsByKey.emplace(key, animations.size() - 1);
        }
    }

    const AnimationMetadata* findAnimation(const std::string& name) const {
        const auto it = animationsByString.find(name);
        if (it != animationsByString.end()) return &animations[it->second];
        return nullptr;
    }

    const AnimationMetadata& findAnimation(AnimationKey key) const {
        return animations[animationsByKey.at(key)];
    }
};