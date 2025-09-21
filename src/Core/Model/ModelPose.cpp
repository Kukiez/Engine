#include "ModelPose.h"

#include <iostream>

#include "Model.h"
#include "Skeleton.h"


MeshPose ModelPoseStack::getFinalPose(unsigned boneOrEntity, const ModelPart part) const {
    MeshPose finalPose{};

    auto& map = getPoseMap(part);
    const auto it = map[boneOrEntity];
    if (it.empty()) return finalPose;

    auto& poseEntries = it;
    auto finalRotation = glm::quat(0, 0, 0, 0);
    auto finalPosition = glm::vec3(0);
    auto finalScale    = glm::vec3(0);
    auto finalColor    = glm::vec3(0);

    double positionWeight = 0;
    double rotationWeight = 0;
    double scaleWeight = 0;
    double colorWeight = 0;

    for (const auto& entry : poseEntries) {
        const auto& [transform, tint] = entry.pose;
        auto weight = static_cast<float>(entry.influence);

        switch (entry.blendMode) {
        case BlendMode::OVERRIDE: {
            if (entry.affected & MeshAffected::POSITION) {
                finalPosition += transform.translation * weight;
                positionWeight += entry.influence;
            }
            if (entry.affected & MeshAffected::SCALE) {
                finalScale += transform.scale * weight;
                scaleWeight += entry.influence;
            }
            if (entry.affected & MeshAffected::COLOR) {
                finalColor += tint * weight;
                colorWeight += entry.influence;
            }
            if (entry.affected & MeshAffected::ROTATION) {
                if (rotationWeight == 0.0) {
                    finalRotation = transform.rotation;
                } else {
                    finalRotation = glm::slerp(finalRotation, transform.rotation, static_cast<float>(weight / (rotationWeight + weight)));
                }
                rotationWeight += weight;
            }
            break;
        }

        case BlendMode::MULTIPLY: {
            if (entry.affected & MeshAffected::SCALE) {
                finalPose.transform.scale *= glm::pow(transform.scale, glm::vec3(weight));
            }
            if (entry.affected & MeshAffected::COLOR) {
                finalPose.tint *= glm::mix(glm::vec3(1.0f), tint, weight);
            }
            if (entry.affected & MeshAffected::ROTATION) {
                glm::quat delta = normalize(transform.rotation);
                glm::quat blendedDelta = slerp(glm::quat(1, 0, 0, 0), delta, weight);

                glm::quat before = normalize(finalPose.transform.rotation);

                finalPose.transform.rotation = normalize(blendedDelta * before);
            }
            if (entry.affected & MeshAffected::POSITION) {
                finalPose.transform.translation += transform.translation * weight;
            }
            break;
        }

        case BlendMode::ADDITIVE: {
            if (entry.affected & MeshAffected::POSITION) {
                finalPose.transform.translation += transform.translation * weight;
            }
            if (entry.affected & MeshAffected::SCALE) {
                finalPose.transform.scale += transform.scale * weight;
            }
            if (entry.affected & MeshAffected::COLOR) {
                finalPose.tint += tint * weight;
            }
            if (entry.affected & MeshAffected::ROTATION) {
                glm::quat delta = slerp(glm::quat(), transform.rotation, weight);
                finalPose.transform.rotation = normalize(finalPose.transform.rotation + delta);
            }
            break;
        }
        }
    }
    if (positionWeight > 0.0) {
        finalPosition /= positionWeight;
        finalPose.transform.translation = finalPosition;
    }
    if (rotationWeight > 0.0) {
        finalRotation = normalize(finalRotation);
        finalPose.transform.rotation = finalRotation;
    }
    if (scaleWeight > 0.0) {
        finalScale /= scaleWeight;
        finalPose.transform.scale = finalScale * finalPose.transform.scale;
    }
    if (colorWeight > 0.0) {
        finalColor /= colorWeight;
        finalPose.tint = finalColor;
    }
    return finalPose;
}

ModelPoseStack::ModelPoseStack(const ModelDefinition &asset) {
    meshPoses.assign(asset.size(), {});
    // if (auto& skeleton = asset.skeleton; skeleton.isValid())
    //     bonePoses.assign(skeleton.size(), {});
}

ModelPoseStack::ModelPoseStack() {
    meshPoses.assign(1, {});
}

ModelPoseStack::MeshMap &ModelPoseStack::getPoseMap(const ModelPart part) {
    switch (part) {
        case ModelPart::BONE:
            return bonePoses;
        case ModelPart::MESH:
            return meshPoses;
        default: std::unreachable();
    }
}

const ModelPoseStack::MeshMap &ModelPoseStack::getPoseMap(const ModelPart part) const {
    switch (part) {
        case ModelPart::BONE:
            return bonePoses;
        case ModelPart::MESH:
            return meshPoses;
        default: std::unreachable();
    }
}

void ModelPoseStack::setPose(const unsigned boneOrEntity, const PoseID poseID, const MeshPose &pose,
                             const MeshAffected affected, const BlendMode blendmode,
                             const double influence, const ModelPart part)
{
    auto& poseStack = getPoseMap(part)[boneOrEntity];

    const auto it = std::ranges::find_if(poseStack, [poseID](const MeshPoseEntry& entry) {
        return entry.id == poseID;
    });

    if (it != poseStack.end()) {
        auto& foundPose = *it;
        foundPose.pose = pose;
        foundPose.influence = influence;
        foundPose.blendMode = blendmode;
    } else {
        poseStack.emplace_back(pose, poseID, influence, affected, blendmode);
    }
}

void ModelPoseStack::removePose(const unsigned boneOrEntity, const PoseID poseID, const ModelPart part) {
    auto& poseStack = getPoseMap(part)[boneOrEntity];

    const auto it = std::ranges::find_if(poseStack, [poseID](const MeshPoseEntry& entry) {
        return entry.id == poseID;
    });

    if (it != poseStack.end()) {
        poseStack.erase(it);
    }
}

bool ModelPoseStack::hasPose(const unsigned boneOrEntity, const ModelPart part) {
    return !getPoseMap(part)[boneOrEntity].empty();
}
