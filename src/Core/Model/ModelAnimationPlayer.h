#pragma once

#include "AnimationRegistry.h"
#include "ModelAnimation.h"
#include "ModelPose.h"

#include "FCurve.h"
#include "Animation/FadeEffect.h"
#include "ECS/Level/LevelView.h"
#include <ECS/ECS.h>

class Model;

enum class AnimationCancel {
    KEEP_FADEOUT = 1,
    SKIP_TO_END = 2,
    IMMEDIATE = 4,
    FADE_TO_IDENTITY = 8
};

enum class AnimationStartError {
    SUCCESS,
    MODEL_ANIMATION_NOT_FOUND,
    MODEL_NOT_FOUND,
    ANIMATION_ALREADY_PLAYING,
    SKELETON_NOT_FOUND
};


constexpr const char* animationStartErrorToString(const AnimationStartError error) {
    switch (error) {
        case AnimationStartError::SUCCESS:
            return "Animation started successfully";
        case AnimationStartError::MODEL_ANIMATION_NOT_FOUND:
            return "Model animation not found";
        case AnimationStartError::MODEL_NOT_FOUND:
            return "Model not found";
        case AnimationStartError::ANIMATION_ALREADY_PLAYING:
            return "Animation already playing";
        case AnimationStartError::SKELETON_NOT_FOUND:
            return "Model does not have a Skeleton Component";
        default:
            return "Unknown error";
    }
}

constexpr bool operator&(AnimationCancel lhs, AnimationCancel rhs) {
    return static_cast<int>(lhs) & static_cast<int>(rhs);
}

constexpr AnimationCancel operator | (const AnimationCancel& first, const AnimationCancel& second) {
    return static_cast<AnimationCancel>(static_cast<int>(first) | static_cast<int>(second));
}

enum class AnimationTarget {
    MODEL, SKELETON, SCENE_NODE
};

struct AnimationStartInfo {
    std::string animationByString;
    Entity model;
    FadeIn fadein;
    FadeOut fadeout;
    double speed;
    double weight;
    BlendMode blendMode;
    bool repeating;
    bool overrideExisting;

    AnimationStartInfo() = default;

    AnimationStartInfo(const std::string& anim, const Entity& model,
        const BlendMode blend = BlendMode::OVERRIDE, const FadeIn fadein = FadeIn(),
        const FadeOut fadeout = FadeOut(), const bool repeating = false,
        const double speed = 1.0, const bool overrideExisting = false, const double weight = 1.0)
        : animationByString(anim), model(model), fadein(fadein), fadeout(fadeout), speed(speed),
    weight(weight), blendMode(blend), repeating(repeating), overrideExisting(overrideExisting) {}
};

class Skeleton;

class AnimationPlayer : Writes<ModelPoseStack>, Reads<Model, Skeleton, Transform>, ReadsResources<AnimationRegistry>, Stages<DefaultStage> {
    FRIEND_DESCRIPTOR

    struct Keyframe {
        FCurve3 position, scale, tint;
        QCurve rotation;
        size_t pHint = 0, sHint = 0, tHint = 0, rHint = 0;
        short affected;

        Keyframe() : position(), tint(), rotation(), affected(4) {}

        Keyframe(const bool rotation, const bool moving, const bool scaling, const bool tinting) : position(), scale(),
            rotation(),
            affected(0)
        {
            affected |= (moving << 0);
            affected |= (scaling << 1);
            affected |= (rotation << 2);
            affected |= (tinting << 3);
        }
    };

    struct AnimationTrack {
        std::vector<std::pair<unsigned, Keyframe>> keyframes;
        AnimationKey animation;
        double currentTime = 0;
        double duration = 0;
        double speed;
        double weight;
        BlendMode blend;
        FadeIn fadein;
        FadeOut fadeout;
        bool repeating = false;
        AnimationTarget target;
    };

    void setKeyframesToAnimation(Viewable auto& view,
                                 AnimationTrack &track, const std::vector<AnimationChannel> &animation, Entity modelEntity
    ) const;

    void clearAnimationPoseForEntity(Viewable auto& view, const Entity &entity, std::vector<AnimationTrack> &track, unsigned index) const;

    std::unordered_map<Entity, std::vector<AnimationTrack>> tracks;
public:

    AnimationStartError play(Viewable auto view, const AnimationStartInfo& info, AnimationTarget target = AnimationTarget::MODEL);

    double cancel(const AnimationKey key, const Entity& forModel, AnimationCancel options = AnimationCancel::IMMEDIATE);
    double cancel(Viewable auto view, const std::string &animation, const Entity &forModel, AnimationCancel options = AnimationCancel::IMMEDIATE);

    void onUpdate(LevelUpdateView<AnimationPlayer>& view);

    bool isAnimationPlayingFor(const AnimationKey animation, const Entity &entity);
    bool isAnimationPlayingFor(Viewable auto view, const std::string &animation, const Entity &forModel);

    double getAnimationProgress(AnimationKey animation, const Entity &forModel) const;
};