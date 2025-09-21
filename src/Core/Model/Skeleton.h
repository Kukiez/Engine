#pragma once
#include <assimp/scene.h>
#include <glm/gtx/matrix_decompose.hpp>
#include <util/stl.h>

#include "Transform.h"

struct BoneInfo {
    std::string boneName;
    int id;
    glm::mat4 offset;
    glm::mat4 local;
};

class SkeletonAsset {
    friend class Skeleton;

    struct Bone {
        int boneID;
        int parentBoneID;
        std::string name;
        std::vector<int> children;

        Transform localTransform; /* identity pose */
        glm::mat4 offsetMatrix;
        glm::vec3 skew;
    };
    int rootBone = -1;
    Bone* bones = nullptr;
    int bonesCount = 0;
    int nonBoneCount = 0;
    stl::unordered_stringmap<int> boneNameToID;

    void loadBoneHierarchy(const aiNode *node, const std::unordered_map<std::string, BoneInfo> &boneMap, int parentID);
    static int getHierarchySize(const aiNode *node);
public:
    SkeletonAsset() = default;
    SkeletonAsset(const aiScene *aiScene, std::unordered_map<std::string, BoneInfo> boneMap);
    SkeletonAsset(const SkeletonAsset& other);
    SkeletonAsset(SkeletonAsset&& other) noexcept;
    SkeletonAsset& operator=(const SkeletonAsset& other) noexcept;
    SkeletonAsset& operator=(SkeletonAsset&& other) noexcept;
    ~SkeletonAsset();

    Skeleton makeInstance() const;

    void renameBone(std::string_view bone, std::string_view newName);
    bool hasBone(std::string_view boneName) const;

    bool isValid() const {
        return bones != nullptr;
    }

    int size() const {
        return bonesCount;
    }
};
class Skeleton : public PrimaryComponent {
    struct BoneInstance {
        Transform localTransform;
        glm::mat4 globalTransform;
    };
    const SkeletonAsset* skeleton;
    BoneInstance* bones;
    std::vector<int> dirtyBones;

    void writeHierarchyRecursive(int boneID, const glm::mat4& parentMatrix, glm::mat4* writePointer);
    static int getHierarchySize(const aiNode* node);

    void markBoneDirty(int boneID);
    bool isBoneChildOf(int childBoneID, int parentBoneID) const;
public:
    explicit Skeleton(const SkeletonAsset* asset);

    Skeleton(const Skeleton& other) = delete;
    Skeleton(Skeleton&& other) noexcept;
    Skeleton& operator=(const Skeleton& other) = delete;
    Skeleton& operator=(Skeleton&& other) noexcept;

    ~Skeleton() {
        delete[] bones;
    }

    void setBoneTransform(std::string_view bone, const Transform& transform);

    void setBoneTransform(int boneID, const Transform &transform);

    void setBonePosition(std::string_view bone, const glm::vec3& position);
    void setBoneRotation(std::string_view bone, const glm::quat& rotation);
    void setBoneScale(std::string_view bone, const glm::vec3& scale);

    const Transform& getBoneTransform(std::string_view bone) const;
    const Transform& getBoneTransform(int boneID) const;

    void writeHierarchy(glm::mat4* bufferZeroIndex) {
        writeHierarchyRecursive(skeleton->rootBone, glm::mat4(1), bufferZeroIndex);
        dirtyBones.clear();
    }

    void writeDirtyBones(glm::mat4* bufferZeroIndex);

    const SkeletonAsset* getSkeletonAsset() const {
        return skeleton;
    }

    int getBoneID(const std::string_view bone) const {
        return skeleton->boneNameToID.find(bone)->second;
    }

    size_t getBoneCount() const {
        return skeleton->bonesCount;
    }
};
