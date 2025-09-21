#include "Skeleton.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

void SkeletonAsset::renameBone(const std::string_view bone, const std::string_view newName) {
    const auto it = boneNameToID.find(bone);
    const auto nIt = boneNameToID.find(newName);
    if (it == boneNameToID.end() || nIt != boneNameToID.end()) return;
    boneNameToID.emplace(newName, it->second);
    boneNameToID.erase(bone);
}

Skeleton SkeletonAsset::makeInstance() const {
    return Skeleton(this);
}

bool SkeletonAsset::hasBone(const std::string_view boneName) const {
    return boneNameToID.contains(boneName);
}

int SkeletonAsset::getHierarchySize(const aiNode *node) {
    int size = 1;

    for (unsigned i = 0; i < node->mNumChildren; i++) {
        size += getHierarchySize(node->mChildren[i]);
    }
    return size;
}

SkeletonAsset::SkeletonAsset(const aiScene *aiScene, std::unordered_map<std::string, BoneInfo> boneMap) {
    bonesCount = static_cast<int>(boneMap.size());
    const int size = getHierarchySize(aiScene->mRootNode);
    bones = new Bone[size];
    boneNameToID.reserve(size);
    loadBoneHierarchy(aiScene->mRootNode, boneMap, -1);
}

SkeletonAsset::SkeletonAsset(const SkeletonAsset &other) {
    if (!other.bones) return;

    bonesCount = other.bonesCount;
    nonBoneCount = other.nonBoneCount;
    rootBone = other.rootBone;
    const int totalBones = bonesCount + nonBoneCount;
    bones = new Bone[totalBones];
    for (int i = 0; i < totalBones; ++i) {
        bones[i] = other.bones[i];
    }
    boneNameToID = other.boneNameToID;
}

SkeletonAsset &SkeletonAsset::operator=(const SkeletonAsset &other) noexcept {
    if (this == &other) return *this;
    bonesCount = other.bonesCount;
    nonBoneCount = other.nonBoneCount;
    rootBone = other.rootBone;
    bones = new Bone[bonesCount];
    const int totalBones = bonesCount + nonBoneCount;
    bones = new Bone[totalBones];
    for (int i = 0; i < totalBones; ++i) {
        bones[i] = other.bones[i];
    }
    boneNameToID = other.boneNameToID;
    return *this;
}

SkeletonAsset::SkeletonAsset(SkeletonAsset&& other) noexcept {
    bonesCount = other.bonesCount;
    nonBoneCount = other.nonBoneCount;
    rootBone = other.rootBone;
    bones = other.bones;
    boneNameToID = std::move(other.boneNameToID);
    bonesCount = other.bonesCount;

    other.bones = nullptr;
    other.nonBoneCount = 0;
    other.rootBone = -1;
    other.bonesCount = 0;
}

SkeletonAsset &SkeletonAsset::operator=(SkeletonAsset &&other) noexcept {
    if (this == &other) return *this;
    delete[] bones;

    bonesCount = other.bonesCount;
    nonBoneCount = other.nonBoneCount;
    rootBone = other.rootBone;
    bones = other.bones;
    boneNameToID = std::move(other.boneNameToID);
    bonesCount = other.bonesCount;

    other.bones = nullptr;
    other.nonBoneCount = 0;
    other.rootBone = -1;
    other.bonesCount = 0;
    return *this;
}

SkeletonAsset::~SkeletonAsset() {
    delete[] bones;
}

#include <xmmintrin.h>

void mat4_mul_sse(const float* a, const float* b, float* result) {
    for (int i = 0; i < 4; ++i) {
        __m128 a_row = _mm_loadu_ps(&a[4 * i]);

        __m128 res = _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set1_ps(a[4 * i + 0]), _mm_loadu_ps(&b[0])),
                _mm_mul_ps(_mm_set1_ps(a[4 * i + 1]), _mm_loadu_ps(&b[4]))
            ),
            _mm_add_ps(
                _mm_mul_ps(_mm_set1_ps(a[4 * i + 2]), _mm_loadu_ps(&b[8])),
                _mm_mul_ps(_mm_set1_ps(a[4 * i + 3]), _mm_loadu_ps(&b[12]))
            )
        );

        _mm_storeu_ps(&result[4 * i], res);
    }
}


void SkeletonAsset::loadBoneHierarchy(const aiNode *node, const std::unordered_map<std::string, BoneInfo> &boneMap,
                                      const int parentID) {
    const std::string nodeName = node->mName.C_Str();

    Transform localTransform;
    glm::vec3 skew;
    glm::vec4 perspective;
    const glm::mat4 localMat = glm::transpose(glm::make_mat4(&node->mTransformation.a1));
    decompose(localMat, localTransform.scale, localTransform.rotation, localTransform.translation, skew, perspective);

    int thisBoneID;
    bool isRealBone = false;
    const auto it = boneMap.find(nodeName);
    if (it != boneMap.end()) {
        thisBoneID = it->second.id;
        isRealBone = true;
    } else {
        thisBoneID = bonesCount + nonBoneCount++;
    }
    new (bones + thisBoneID) Bone{
        .boneID = thisBoneID,
        .parentBoneID = parentID,
        .name = nodeName,
        .children = {},
        .localTransform = localTransform,
        .offsetMatrix = isRealBone ? it->second.offset : glm::mat4(1.0f),
        .skew = skew
    };
    boneNameToID.emplace(nodeName, thisBoneID);

    if (rootBone == -1) {
        rootBone = thisBoneID;
    }
    if (parentID >= 0) {
        bones[parentID].children.push_back(thisBoneID);
    }
    std::cout << "Bone: " << nodeName << std::endl;
    for (unsigned i = 0; i < node->mNumChildren; i++) {
        loadBoneHierarchy(node->mChildren[i], boneMap, thisBoneID);
    }
}

void Skeleton::writeHierarchyRecursive(const int boneID, const glm::mat4 &parentMatrix, glm::mat4 *writePointer) {
    auto& [localTransform, globalTransform] = bones[boneID];
    globalTransform = parentMatrix * localTransform.createModel3DSkew(skeleton->bones[boneID].skew);

    if (skeleton->bones[boneID].boneID < skeleton->bonesCount) {
        new (writePointer + skeleton->bones[boneID].boneID) glm::mat4(
            globalTransform * skeleton->bones[boneID].offsetMatrix
        );
    }

    for (const int childID : skeleton->bones[boneID].children) {
        writeHierarchyRecursive(childID, globalTransform, writePointer);
    }
}

void Skeleton::markBoneDirty(const int boneID) {
    for (const int existingDirty : dirtyBones) {
        if (isBoneChildOf(boneID, existingDirty)) return;
    }
    std::erase_if(dirtyBones, [&](const int otherBone) {
        return isBoneChildOf(otherBone, boneID);
    });
    dirtyBones.push_back(boneID);
}

bool Skeleton::isBoneChildOf(int childBoneID, const int parentBoneID) const {
    while (childBoneID != -1) {
        if (childBoneID == parentBoneID) return true;
        childBoneID = skeleton->bones[childBoneID].parentBoneID;
    }
    return false;
}

Skeleton::Skeleton(const SkeletonAsset *asset) {
    bones = new BoneInstance[asset->bonesCount + asset->nonBoneCount];
    skeleton = asset;

    for (int i = 0; i < asset->bonesCount + asset->nonBoneCount; ++i) {
        bones[i].localTransform = asset->bones[i].localTransform;
        bones[i].globalTransform = glm::mat4(1.0f);
    }
    dirtyBones.emplace_back(skeleton->rootBone);
}

Skeleton::Skeleton(Skeleton &&other) noexcept {
    bones = other.bones;
    skeleton = other.skeleton;
    other.bones = nullptr;
}

Skeleton& Skeleton::operator=(Skeleton &&other) noexcept {
    if (this == &other) return *this;

    bones = other.bones;
    skeleton = other.skeleton;
    other.bones = nullptr;
    return *this;
}

void Skeleton::setBoneTransform(const std::string_view bone, const Transform &transform) {
    const auto it = skeleton->boneNameToID.find(bone);

    if (it == skeleton->boneNameToID.end()) {
        std::cerr << "Bone " << bone << " not found, Bones: " << skeleton->boneNameToID.size() << std::endl;
        return;
    }
    bones[it->second].localTransform = transform;
    markBoneDirty(it->second);
}

void Skeleton::setBoneTransform(const int boneID, const Transform &transform) {
    if (boneID > getBoneCount()) std::exit(55);

    bones[boneID].localTransform = transform;
    markBoneDirty(boneID);
}

void Skeleton::setBonePosition(const std::string_view bone, const glm::vec3 &position) {
    const auto it = skeleton->boneNameToID.find(bone);
    if (it == skeleton->boneNameToID.end()) {
        std::cerr << "Bone " << bone << " not found, Bones: " << skeleton->boneNameToID.size() << std::endl;
        return;
    }

    bones[it->second].localTransform.translation = position;
    markBoneDirty(it->second);
}

void Skeleton::setBoneRotation(const std::string_view bone, const glm::quat &rotation) {
    const auto it = skeleton->boneNameToID.find(bone);
    if (it == skeleton->boneNameToID.end()) {
        std::cerr << "Bone " << bone << " not found, Bones: " << skeleton->boneNameToID.size() << std::endl;
        return;
    }

    bones[it->second].localTransform.rotation = rotation;
    markBoneDirty(it->second);
}

void Skeleton::setBoneScale(const std::string_view bone, const glm::vec3& scale) {
    const auto it = skeleton->boneNameToID.find(bone);
    if (it == skeleton->boneNameToID.end()) {
        std::cerr << "Bone " << bone << " not found, Bones: " << skeleton->boneNameToID.size() << std::endl;
        return;
    }
    bones[it->second].localTransform.scale = scale;
    markBoneDirty(it->second);
}

const Transform & Skeleton::getBoneTransform(const std::string_view bone) const {
    if (const auto it = skeleton->boneNameToID.find(bone); it != skeleton->boneNameToID.end()) {
        return bones[it->second].localTransform;
    }
    static Transform failsafe{};
    // TODO Error message
    return failsafe;
}

const Transform & Skeleton::getBoneTransform(const int boneID) const {
    return bones[boneID].localTransform;
}

void Skeleton::writeDirtyBones(glm::mat4 *bufferZeroIndex) {
    for (const int dirtyBone : dirtyBones) {
        std::cout << "Dirty Bone: " << dirtyBone << std::endl;
        glm::mat4 parentMatrix = skeleton->bones[dirtyBone].parentBoneID != -1 ? bones[skeleton->bones[dirtyBone].parentBoneID].globalTransform : glm::mat4(1.0f);
        writeHierarchyRecursive(dirtyBone, parentMatrix, bufferZeroIndex);
    }
    dirtyBones.clear();
}
