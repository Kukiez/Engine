#pragma once
#include <openGL/BufferObjects/ShaderStorageBuffer.h>

#include "ECS/Component/Component.h"

struct VoxelFaceData {
    union {
        struct {
            uint16_t negX;
            uint16_t posX;
            uint16_t negY;
            uint16_t posY;
            uint16_t negZ;
            uint16_t posZ;
        };
        uint16_t faces[6];
    };


    VoxelFaceData() {
        memset(faces, 0, sizeof(faces));
    }

    static const VoxelFaceData& Air() {
        static VoxelFaceData air{};
        return air;
    }

    explicit VoxelFaceData(const uint16_t* faces) {
        posX = faces[0];
        negX = faces[1];
        posY = faces[2];
        negY = faces[3];
        posZ = faces[4];
        negZ = faces[5];
    }

    VoxelFaceData& operator = (const uint16_t* faces) {
        memcpy(this->faces, faces, sizeof(this->faces));
        return *this;
    }

    bool isAir() const {
        return posX == 0 && negX == 0 &&
               posY == 0 && negY == 0 &&
               posZ == 0 && negZ == 0;
    }
};

struct VoxelVolume : PrimaryComponent {
    using voxel_type = VoxelFaceData;
    voxel_type voxels[16 * 16 * 16];

    VoxelVolume() {
        memset(voxels, 0, sizeof(voxels));
    }

    voxel_type at(const int x, const int y, const int z) const /* in local space of 0-16 */ {
        return voxels[x + y * 16 + z * 16 * 16];
    }

    unsigned at(const int x, const int y, const int z, const int face) const {
        return voxels[x + y * 16 + z * 16 * 16].faces[face];
    }

    voxel_type& at(const int x, const int y, const int z) /* in local space of 0-16 */ {
        return voxels[x + y * 16 + z * 16 * 16];
    }

    static voxel_type air() {
        return voxel_type::Air();
    }

    void addVoxel(const int x, const int y, const int z, const uint16_t tex) {
        auto& faces = at(x, y, z).faces;
        for (int i = 0; i < 6; ++i) {
            faces[i] = tex;
        }
    }

    void addVoxel(const int x, const int y, const int z, const uint16_t* tex) {
        auto& faces = at(x, y, z).faces;
        memcpy(faces, tex, sizeof(voxel_type));
    }

    void removeVoxel(const int x, const int y, const int z) /* in local space of 0-16 */ {
        memset(&at(x, y, z), 0, sizeof(voxel_type));
    }

    static bool isOutsideBounds(const int x, const int y, const int z) {
        return x < 0 || y < 0 || z < 0 || x >= 16 || y >= 16 || z >= 16;
    }

    voxel_type atSafe(const int x, const int y, const int z) const /* in local space of 0-16 */ {
        if (isOutsideBounds(x, y, z)) return {};
        return at(x, y, z);
    }
};

#include "VoxelConstants.h"