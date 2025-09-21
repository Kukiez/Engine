#pragma once
#include <glm/vec3.hpp>
#include "Transform.h"

class ModelDefinition;
using PoseID = uint64_t;

struct MeshPose {
    Transform transform{};
    glm::vec3 tint = glm::vec3(1);

    static PoseID hash(const std::string_view str) {
        return std::hash<std::string_view>()(str);
    }

    static MeshPose identity() {
        return MeshPose();
    }
};

inline MeshPose operator*(const MeshPose& parentPose, const MeshPose& childPose) {
    MeshPose result;
    result.transform = parentPose.transform * childPose.transform;
    result.tint = parentPose.tint * childPose.tint;
    return result;
}

struct MeshSocket : PrimaryComponent {
    std::string parentMesh;
    MeshPose localOffset;
    float forward;
    float side;
    float up;

    MeshSocket(const std::string& parentMesh, MeshPose localOffset, const float forward = 0, const float side = 0, const float up = 0)
    : parentMesh(parentMesh), localOffset(localOffset), forward(forward), side(side), up(up)  {}

    explicit MeshSocket(MeshPose localOffset, const float forward = 0, const float side = 0, const float up = 0)
    : localOffset(localOffset), forward(forward), side(side), up(up)  {}
};

enum class MeshAffected {
    POSITION = 1,
    SCALE = 2,
    ROTATION = 4,
    COLOR = 8
};

enum class BlendMode {
    ADDITIVE,
    OVERRIDE,
    MULTIPLY
};

struct MeshPoseEntry {
    MeshPose pose;
    PoseID id;
    double influence = 1;
    MeshAffected affected;
    BlendMode blendMode = BlendMode::ADDITIVE;

    bool operator==(const MeshPoseEntry & other) const {
        return id == other.id;
    }

    bool operator == (const PoseID& other) const {
        return id == other;
    }
};

constexpr bool operator&(MeshAffected lhs, MeshAffected rhs) {
    return static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs);
}

enum class ModelPart {
    MESH,
    BONE,
    SOCKET
};

class SkeletonAsset;

class ModelPoseStack : public PrimaryComponent {
    using MeshMap = std::vector<std::vector<MeshPoseEntry>>;
    MeshMap meshPoses;
    MeshMap bonePoses;
public:
    ModelPoseStack(const ModelDefinition& asset);
    ModelPoseStack();

    MeshMap& getPoseMap(ModelPart part);
    const MeshMap& getPoseMap(ModelPart part) const;

    void setPose(unsigned boneOrEntity, PoseID poseID, const MeshPose& pose, MeshAffected affected, BlendMode blendmode, double influence, ModelPart part);
    void removePose(unsigned boneOrEntity, PoseID poseID, ModelPart part);

    bool hasPose(unsigned boneOrEntity, ModelPart part);

    MeshPose getFinalPose(unsigned boneOrEntity, ModelPart part) const;
};
