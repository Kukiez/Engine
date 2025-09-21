#include "ModelAnimationPlayer.h"

#include <ranges>

#include "Model.h"

#include "ModelAnimation.h"
#include "ModelPose.h"
#include "World/SceneGraph.h"
#include <ECS/ECS.h>

void AnimationPlayer::setKeyframesToAnimation(Viewable auto& view, AnimationTrack& track, const std::vector<AnimationChannel>& animation, const Entity modelEntity) const {;
    for (const auto& [target, position, scale, tint, rotation] : animation) {
        MeshPose pose;
        unsigned kfIdx = 0;
        switch (track.target) {
            case AnimationTarget::MODEL: {
                auto& model = *view.get<Model>(modelEntity);
                if (model.getMeshPart(target) == MeshID::INVALID) continue;
                kfIdx = static_cast<unsigned>(model.getMeshPart(target));
                pose = view.get<ModelPoseStack>(modelEntity)->getFinalPose(kfIdx, ModelPart::MESH);
                break;
            }
            case AnimationTarget::SKELETON: {
                auto& skeleton = *view.get<Skeleton>(modelEntity);
                pose.transform = skeleton.getBoneTransform(target);
                pose.tint = glm::vec3(1);
                kfIdx = skeleton.getBoneID(target);
                break;
            }
            case AnimationTarget::SCENE_NODE: {
                pose.transform = *view.get<Transform>(modelEntity);
                pose.tint = glm::vec3(1);
                kfIdx = 0;
                break;
            }
            default:
                pose = MeshPose{};
        }
        auto& kf = track.keyframes.emplace_back(kfIdx, Keyframe(
            !rotation.empty(),
            !position.empty(),
            !scale.empty(),
            !tint.empty())
        ).second;

        kf.scale = scale;
        kf.rotation = rotation;
        kf.position = position;
        kf.tint = tint;

        kf.scale.add(pose.transform.scale, 0);
        kf.rotation.add(pose.transform.rotation, 0);
        kf.position.add(pose.transform.translation, 0);
        kf.tint.add(pose.tint, 0);
    }
}

void AnimationPlayer::clearAnimationPoseForEntity(Viewable auto& view, const Entity &entity, std::vector<AnimationTrack>& track, const unsigned index) const {
    auto& animation = *(track.begin() + index);

    switch (animation.target) {
        case AnimationTarget::SKELETON:
        case AnimationTarget::MODEL: {
            auto& modelPose = *view.get<ModelPoseStack>(entity);
            for (const auto& mesh: animation.keyframes | std::views::keys) {
                modelPose.removePose(mesh, PoseID{static_cast<unsigned long long>(animation.animation)}, static_cast<ModelPart>(animation.target));
            }
        }
        default: break;
    }
}

AnimationStartError AnimationPlayer::play(Viewable auto view, const AnimationStartInfo& info, AnimationTarget target) {
    auto readStartInfoToTrack = [target](AnimationTrack& track, const AnimationStartInfo& animInfo, const AnimationMetadata* animation) {
        track.fadein = animInfo.fadein;
        track.fadeout = animInfo.fadeout;
        track.repeating = animInfo.repeating;
        track.speed = animInfo.speed;
        track.blend = animInfo.blendMode;
        track.weight = animInfo.weight;
        track.duration = animation->duration();
        track.animation = animation->key;
        track.target = target;
    };

    AnimationMetadata* animation;

    if (animation = view.get<AnimationRegistry>().findAnimation(info.animationByString); !animation) {
        return AnimationStartError::MODEL_ANIMATION_NOT_FOUND;
    }

    switch (target) {
        case AnimationTarget::SKELETON: {
            if (!view.has<Skeleton>(info.model)) {
                return AnimationStartError::SKELETON_NOT_FOUND;
            }
            break;
        }
        case AnimationTarget::MODEL: {
            if (!view.has<Model>(info.model))
                return AnimationStartError::MODEL_NOT_FOUND;
            break;
        }
        case AnimationTarget::SCENE_NODE: {
            if (!view.has<SceneNode>(info.model)
                || !view.has<ModelPoseStack>(info.model))
            {
                return AnimationStartError::MODEL_NOT_FOUND; //todo change error enum
            }
        }
    }

    if (isAnimationPlayingFor(animation->key, info.model)) {
        if (info.overrideExisting) {
            for (auto& track : tracks[info.model.id()]) {
                if (track.animation == animation.key) {
                    readStartInfoToTrack(track, info, animation);
                    track.keyframes.clear();
                    track.currentTime = 0;
                    setKeyframesToAnimation(track, animation->getChannels(), info.model.id());
                    return AnimationStartError::SUCCESS;
                }
            }
        }
        return AnimationStartError::ANIMATION_ALREADY_PLAYING;
    }
    AnimationTrack track;
    readStartInfoToTrack(track, info, animation);
    setKeyframesToAnimation(track, animation->getChannels(), info.model);
    tracks[info.model.id()].emplace_back(std::move(track));
    return AnimationStartError::SUCCESS;
}

double AnimationPlayer::cancel(const AnimationKey animation, const Entity &forModel, const AnimationCancel options) {
    const auto it = tracks.find(forModel);

    if (it == tracks.end()) return 0;
    auto& track = it->second;

    for (int i = 0; i < track.size(); ++i) {
        if (track[i].animation == animation) {
            double exitTime = track[i].currentTime;

            if (options & AnimationCancel::IMMEDIATE) {
                track.erase(track.begin() + i);

                if (track.empty()) tracks.erase(it);
                return exitTime;
            }
            if (options & AnimationCancel::KEEP_FADEOUT) {
                track[i].duration = track[i].currentTime + track[i].fadeout.duration;
            }
            if (options & AnimationCancel::FADE_TO_IDENTITY) {
                track[i].duration = track[i].currentTime + track[i].fadeout.duration;

                for (auto & keyframe: track[i].keyframes | std::views::values) {
                    const double fadeOutTime = track[i].currentTime + track[i].fadeout.duration;
                    const double currentTime = track[i].currentTime;

                    auto rkf = keyframe.rotation.at(currentTime);
                    auto skf = keyframe.scale.at(currentTime);
                    auto pkf = keyframe.position.at(currentTime);
                    auto tkf = keyframe.tint.at(currentTime);

                    keyframe.rotation.clear();
                    keyframe.scale.clear();
                    keyframe.position.clear();
                    keyframe.tint.clear();

                    keyframe.rotation.add(rkf, currentTime);
                    keyframe.rotation.add(glm::quat(1, 0, 0, 0), fadeOutTime);

                    keyframe.scale.add(skf, currentTime);
                    keyframe.scale.add(glm::vec3(1.0f), fadeOutTime);

                    keyframe.position.add(pkf, currentTime);
                    keyframe.position.add(glm::vec3(0.0f), fadeOutTime);

                    keyframe.tint.add(tkf, currentTime);
                    keyframe.tint.add(glm::vec3(1.0f), fadeOutTime);
                }
            }
            track[i].repeating = false;
            return exitTime;
        }
    }
    return 0;
}

double AnimationPlayer::cancel(Viewable auto view, const std::string& animation, const Entity &forModel, const AnimationCancel options) {
    if (auto* anim = view.get<AnimationRegistry>().findAnimation(animation)) {
        return cancel(anim->key, forModel, options);
    }
    return 0;
}

static void test3(View<ModelPoseStack> view) {
}

struct LOL : PrimaryComponent {};
static void test(View<AnimationPlayer> view) {
   // test3(view);
}

void AnimationPlayer::onUpdate(LevelUpdateView<AnimationPlayer>& view) {
    std::vector<std::pair<Entity, unsigned>> tracksToDelete;

    test(view);

    for (auto& [animEntity, track] : tracks) {
        std::vector<int> tracksToRepeat;
        auto& poseStack = *view.get<ModelPoseStack>(animEntity);

        auto now = std::chrono::high_resolution_clock::now();
        for (int trackIndex = 0; trackIndex < track.size(); ++trackIndex) {
            auto& [keyframes, animation,
                currTime, duration, speed,
                weight, blend, fadein,
                fadeout, repeating, animTarget]
            = track[trackIndex];

            const double fadeoutDuration = fadeout.duration / speed;
            const double fadeinDuration = fadein.duration / speed;
            const double fadeInInfluence = (fadeinDuration > 0.0001 && !repeating) ? Interpolate::lerp(0.0, 1.0, currTime / fadeinDuration, fadein.easing) : 1;
            const double fadeOutInfluence = (fadeoutDuration > 0.0001 && !repeating) ? Interpolate::lerp(0.0, 1.0, (duration - currTime) / fadeoutDuration, fadeout.easing) : 1;
            const double fadeInOutWeight = std::clamp(fadeInInfluence, 0.0, 1.0) * std::clamp(fadeOutInfluence, 0.0, 1.0);
            const double resultWeight = weight * fadeInOutWeight;

            for (auto& [target, frame] : keyframes) {
                MeshPose pose;
                pose.transform.translation = frame.position.at(currTime, frame.pHint);
                pose.transform.scale = frame.scale.at(currTime, frame.sHint);
                pose.transform.rotation = frame.rotation.at(currTime, frame.rHint);
                pose.tint = frame.tint.at(currTime, frame.tHint);

                switch (animTarget) {
                    case AnimationTarget::MODEL: {
                        auto& model = *view.get<Model>(animEntity);
                        poseStack.setPose(target, PoseID{static_cast<unsigned long long>(animation)}, pose, MeshAffected{frame.affected}, blend, resultWeight, ModelPart::MESH);
                     //   if (target == 0)
                        //    scene.getEntityManager().addState<SceneGraphNodeInvalidated>(animEntity);
                    //    else
                       //     scene.getEntityManager().addState<SceneGraphNodeInvalidated>(model.getMeshByMeshID(target));
                        break;
                    }
                    case AnimationTarget::SKELETON: {
                        poseStack.setPose(target, PoseID{static_cast<unsigned long long>(animation)}, pose, MeshAffected{frame.affected}, blend, resultWeight, ModelPart::BONE);
                    //    scene.getEntityManager().addState<SceneGraphNodeInvalidated>(animEntity);
                        break;
                    }
                    case AnimationTarget::SCENE_NODE: {
                        poseStack.setPose(0, PoseID{static_cast<unsigned long long>(animation)}, pose, MeshAffected{frame.affected}, blend, resultWeight, ModelPart::MESH);
                    //    scene.getEntityManager().addState<SceneGraphNodeInvalidated>(animEntity);
                        break;
                    }
                    default: std::unreachable();
                }
            }
            if (currTime >= duration) {
                if (repeating) {
                    tracksToRepeat.emplace_back(trackIndex);
                } else {
                    tracksToDelete.emplace_back(animEntity, trackIndex);
                }
            }
            currTime += 0.016 * speed; // removed deltaTime *;
        }
        for (auto& index : tracksToRepeat) {
            track[index].keyframes.clear();
            track[index].currentTime = 0;
            auto animation = track[index].animation;
            setKeyframesToAnimation(view, track[index], view.get<AnimationRegistry>().findAnimation(animation).getChannels(), animEntity);
        }
        tracksToRepeat.clear();
    }
    for (auto& [entity, index] : tracksToDelete) {
        clearAnimationPoseForEntity(view, entity, tracks[entity], index);
        if (auto& track = tracks[entity]; track.size() == 1) {
            clearAnimationPoseForEntity(view, entity, track, 0);
            tracks.erase(entity);
        } else {
            clearAnimationPoseForEntity(view, entity, track, index);
            track.erase(track.begin() + index);
        }
    }
}

bool AnimationPlayer::isAnimationPlayingFor(const AnimationKey animation, const Entity &entity) {
    const auto it = tracks.find(entity);

    if (it == tracks.end()) {
        return false;
    }
    return std::ranges::any_of(it->second, [&](const AnimationTrack& track) {
        return track.animation == animation;
    });
}

bool AnimationPlayer::isAnimationPlayingFor(Viewable auto view, const std::string& animation, const Entity &forModel) {
    AnimationMetadata* anim;

    if (anim = view.get<AnimationRegistry>().findAnimation(animation)) {
        return isAnimationPlayingFor(anim->key, forModel);
    }
    return false;
}

double AnimationPlayer::getAnimationProgress(const AnimationKey animation, const Entity &forModel) const {
    const auto it = tracks.find(forModel);
    if (it == tracks.end()) return 1;

    for (const auto& track : it->second) {
        if (track.animation== animation) {
            return track.currentTime / track.duration; // track.duration non-zero
        }
    }
    return 1;
}

