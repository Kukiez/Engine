//
// Created by dkuki on 6/17/2025.
//

#include "VoxelWorld.h"

#include <util/glm_double.h>

#include <ECS/ECS.h>

const char* VoxelConstants::NULL_FACE = "__NULL__";
const char* VoxelConstants::ACACIA_LEAVES = "ACACIA_LEAVES";
const char* VoxelConstants::ACACIA_LOG = "ACACIA_LOG";
const char* VoxelConstants::ACACIA_PLANKS = "ACACIA_PLANKS";
const char* VoxelConstants::AMETHYST_BLOCK = "AMETHYST_BLOCK";
const char* VoxelConstants::ANCIENT_DEBRIS = "ANCIENT_DEBRIS";
const char* VoxelConstants::ANDESITE = "ANDESITE";
const char* VoxelConstants::ANVIL = "ANVIL";
const char* VoxelConstants::ANVIL_TOP = "ANVIL_TOP";
const char* VoxelConstants::BAMBOO_BLOCK = "BAMBOO_BLOCK";
const char* VoxelConstants::BAMBOO_PLANKS = "BAMBOO_PLANKS";
const char* VoxelConstants::BASALT_SIDE = "BASALT_SIDE";
const char* VoxelConstants::BASALT_TOP = "BASALT_TOP";
const char* VoxelConstants::BEDROCK = "BEDROCK";
const char* VoxelConstants::BIRCH_LEAVES = "BIRCH_LEAVES";
const char* VoxelConstants::BIRCH_LOG = "BIRCH_LOG";
const char* VoxelConstants::BIRCH_PLANKS = "BIRCH_PLANKS";
const char* VoxelConstants::BLACKSTONE = "BLACKSTONE";
const char* VoxelConstants::BLACKSTONE_TOP = "BLACKSTONE_TOP";
const char* VoxelConstants::BLACK_CONCRETE = "BLACK_CONCRETE";
const char* VoxelConstants::BLACK_CONCRETE_POWDER = "BLACK_CONCRETE_POWDER";
const char* VoxelConstants::BLACK_GLAZED_TERRACOTTA = "BLACK_GLAZED_TERRACOTTA";
const char* VoxelConstants::BLACK_TERRACOTTA = "BLACK_TERRACOTTA";
const char* VoxelConstants::BLACK_WOOL = "BLACK_WOOL";
const char* VoxelConstants::BLUE_CONCRETE_POWDER = "BLUE_CONCRETE_POWDER";
const char* VoxelConstants::BLUE_GLAZED_TERRACOTTA = "BLUE_GLAZED_TERRACOTTA";
const char* VoxelConstants::BLUE_ICE = "BLUE_ICE";
const char* VoxelConstants::BLUE_TERRACOTTA = "BLUE_TERRACOTTA";
const char* VoxelConstants::BLUE_WOOL = "BLUE_WOOL";
const char* VoxelConstants::BONE_BLOCK_SIDE = "BONE_BLOCK_SIDE";
const char* VoxelConstants::BONE_BLOCK_TOP = "BONE_BLOCK_TOP";
const char* VoxelConstants::BRICKS = "BRICKS";
const char* VoxelConstants::BROWN_CONCRETE = "BROWN_CONCRETE";
const char* VoxelConstants::BROWN_CONCRETE_POWDER = "BROWN_CONCRETE_POWDER";
const char* VoxelConstants::BROWN_GLAZED_TERRACOTTA = "BROWN_GLAZED_TERRACOTTA";
const char* VoxelConstants::BROWN_MUSHROOM_BLOCK = "BROWN_MUSHROOM_BLOCK";
const char* VoxelConstants::BROWN_TERRACOTTA = "BROWN_TERRACOTTA";
const char* VoxelConstants::BROWN_WOOL = "BROWN_WOOL";
const char* VoxelConstants::BUDDING_AMETHYST = "BUDDING_AMETHYST";
const char* VoxelConstants::CALCITE = "CALCITE";
const char* VoxelConstants::CHERRY_LOG = "CHERRY_LOG";
const char* VoxelConstants::CHERRY_LOG_TOP = "CHERRY_LOG_TOP";
const char* VoxelConstants::CHERRY_PLANKS = "CHERRY_PLANKS";
const char* VoxelConstants::CHIPPED_ANVIL_TOP = "CHIPPED_ANVIL_TOP";
const char* VoxelConstants::CHISELED_DEEPSLATE = "CHISELED_DEEPSLATE";
const char* VoxelConstants::CHISELED_QUARTZ_BLOCK = "CHISELED_QUARTZ_BLOCK";
const char* VoxelConstants::CHISELED_QUARTZ_BLOCK_TOP = "CHISELED_QUARTZ_BLOCK_TOP";
const char* VoxelConstants::CHISELED_RED_SANDSTONE = "CHISELED_RED_SANDSTONE";
const char* VoxelConstants::CHISELED_SANDSTONE = "CHISELED_SANDSTONE";
const char* VoxelConstants::CHISELED_STONE_BRICKS = "CHISELED_STONE_BRICKS";
const char* VoxelConstants::CLAY = "CLAY";
const char* VoxelConstants::COAL_BLOCK = "COAL_BLOCK";
const char* VoxelConstants::COARSE_DIRT = "COARSE_DIRT";
const char* VoxelConstants::COBBLED_DEEPSLATE = "COBBLED_DEEPSLATE";
const char* VoxelConstants::COBBLESTONE = "COBBLESTONE";
const char* VoxelConstants::COMPOSTER_BOTTOM = "COMPOSTER_BOTTOM";
const char* VoxelConstants::COMPOSTER_COMPOST = "COMPOSTER_COMPOST";
const char* VoxelConstants::COMPOSTER_READY = "COMPOSTER_READY";
const char* VoxelConstants::COMPOSTER_SIDE = "COMPOSTER_SIDE";
const char* VoxelConstants::COMPOSTER_TOP = "COMPOSTER_TOP";
const char* VoxelConstants::COPPER_BLOCK = "COPPER_BLOCK";
const char* VoxelConstants::CRACKED_DEEPSLATE_BRICKS = "CRACKED_DEEPSLATE_BRICKS";
const char* VoxelConstants::CRACKED_NETHER_BRICKS = "CRACKED_NETHER_BRICKS";
const char* VoxelConstants::CRACKED_STONE_BRICKS = "CRACKED_STONE_BRICKS";
const char* VoxelConstants::CRIMSON_PLANKS = "CRIMSON_PLANKS";
const char* VoxelConstants::CRIMSON_STEM = "CRIMSON_STEM";
const char* VoxelConstants::CRIMSON_STEM_TOP = "CRIMSON_STEM_TOP";
const char* VoxelConstants::CRYING_OBSIDIAN = "CRYING_OBSIDIAN";
const char* VoxelConstants::CUT_COPPER = "CUT_COPPER";
const char* VoxelConstants::CUT_RED_SANDSTONE = "CUT_RED_SANDSTONE";
const char* VoxelConstants::CUT_SANDSTONE = "CUT_SANDSTONE";
const char* VoxelConstants::CYAN_CONCRETE_POWDER = "CYAN_CONCRETE_POWDER";
const char* VoxelConstants::CYAN_GLAZED_TERRACOTTA = "CYAN_GLAZED_TERRACOTTA";
const char* VoxelConstants::CYAN_STAINED_GLASS = "CYAN_STAINED_GLASS";
const char* VoxelConstants::CYAN_TERRACOTTA = "CYAN_TERRACOTTA";
const char* VoxelConstants::CYAN_WOOL = "CYAN_WOOL";
const char* VoxelConstants::DAMAGED_ANVIL_TOP = "DAMAGED_ANVIL_TOP";
const char* VoxelConstants::DARK_OAK_LEAVES = "DARK_OAK_LEAVES";
const char* VoxelConstants::DARK_OAK_LOG = "DARK_OAK_LOG";
const char* VoxelConstants::DARK_OAK_PLANKS = "DARK_OAK_PLANKS";
const char* VoxelConstants::DARK_PRISMARINE = "DARK_PRISMARINE";
const char* VoxelConstants::DAYLIGHT_DETECTOR_INVERTED_TOP = "DAYLIGHT_DETECTOR_INVERTED_TOP";
const char* VoxelConstants::DAYLIGHT_DETECTOR_SIDE = "DAYLIGHT_DETECTOR_SIDE";
const char* VoxelConstants::DAYLIGHT_DETECTOR_TOP = "DAYLIGHT_DETECTOR_TOP";
const char* VoxelConstants::DEEPSLATE = "DEEPSLATE";
const char* VoxelConstants::DEEPSLATE_BRICKS = "DEEPSLATE_BRICKS";
const char* VoxelConstants::DEEPSLATE_TILES = "DEEPSLATE_TILES";
const char* VoxelConstants::DEEPSLATE_TOP = "DEEPSLATE_TOP";
const char* VoxelConstants::DIAMOND_BLOCK = "DIAMOND_BLOCK";
const char* VoxelConstants::DIORITE = "DIORITE";
const char* VoxelConstants::DIRT_PATH_TOP = "DIRT_PATH_TOP";
const char* VoxelConstants::DRAGON_EGG = "DRAGON_EGG";
const char* VoxelConstants::DRIED_KELP_BOTTOM = "DRIED_KELP_BOTTOM";
const char* VoxelConstants::DRIED_KELP_TOP = "DRIED_KELP_TOP";
const char* VoxelConstants::DRIPSTONE_BLOCK = "DRIPSTONE_BLOCK";
const char* VoxelConstants::EMERALD_BLOCK = "EMERALD_BLOCK";
const char* VoxelConstants::END_STONE = "END_STONE";
const char* VoxelConstants::END_STONE_BRICKS = "END_STONE_BRICKS";
const char* VoxelConstants::EXPOSED_COPPER = "EXPOSED_COPPER";
const char* VoxelConstants::FERN = "FERN";
const char* VoxelConstants::GLOWSTONE = "GLOWSTONE";
const char* VoxelConstants::GOLD_BLOCK = "GOLD_BLOCK";
const char* VoxelConstants::GRANITE = "GRANITE";
const char* VoxelConstants::GRASS = "GRASS";
const char* VoxelConstants::GRASS_BLOCK_TOP = "GRASS_BLOCK_TOP";
const char* VoxelConstants::GRAVEL = "GRAVEL";
const char* VoxelConstants::GRAY_CONCRETE = "GRAY_CONCRETE";
const char* VoxelConstants::GRAY_CONCRETE_POWDER = "GRAY_CONCRETE_POWDER";
const char* VoxelConstants::GRAY_GLAZED_TERRACOTTA = "GRAY_GLAZED_TERRACOTTA";
const char* VoxelConstants::GRAY_TERRACOTTA = "GRAY_TERRACOTTA";
const char* VoxelConstants::GRAY_WOOL = "GRAY_WOOL";
const char* VoxelConstants::GREEN_CONCRETE_POWDER = "GREEN_CONCRETE_POWDER";
const char* VoxelConstants::GREEN_GLAZED_TERRACOTTA = "GREEN_GLAZED_TERRACOTTA";
const char* VoxelConstants::GREEN_TERRACOTTA = "GREEN_TERRACOTTA";
const char* VoxelConstants::GREEN_WOOL = "GREEN_WOOL";
const char* VoxelConstants::HAY_BLOCK_SIDE = "HAY_BLOCK_SIDE";
const char* VoxelConstants::HAY_BLOCK_TOP = "HAY_BLOCK_TOP";
const char* VoxelConstants::IRON_BARS = "IRON_BARS";
const char* VoxelConstants::IRON_BLOCK = "IRON_BLOCK";
const char* VoxelConstants::JUNGLE_LEAVES = "JUNGLE_LEAVES";
const char* VoxelConstants::JUNGLE_LOG = "JUNGLE_LOG";
const char* VoxelConstants::JUNGLE_PLANKS = "JUNGLE_PLANKS";
const char* VoxelConstants::LAPIS_BLOCK = "LAPIS_BLOCK";
const char* VoxelConstants::LIGHT_BLUE_CONCRETE_POWDER = "LIGHT_BLUE_CONCRETE_POWDER";
const char* VoxelConstants::LIGHT_BLUE_GLAZED_TERRACOTTA = "LIGHT_BLUE_GLAZED_TERRACOTTA";
const char* VoxelConstants::LIGHT_BLUE_STAINED_GLASS = "LIGHT_BLUE_STAINED_GLASS";
const char* VoxelConstants::LIGHT_BLUE_TERRACOTTA = "LIGHT_BLUE_TERRACOTTA";
const char* VoxelConstants::LIGHT_BLUE_WOOL = "LIGHT_BLUE_WOOL";
const char* VoxelConstants::LIGHT_GRAY_CONCRETE = "LIGHT_GRAY_CONCRETE";
const char* VoxelConstants::LIGHT_GRAY_CONCRETE_POWDER = "LIGHT_GRAY_CONCRETE_POWDER";
const char* VoxelConstants::LIGHT_GRAY_GLAZED_TERRACOTTA = "LIGHT_GRAY_GLAZED_TERRACOTTA";
const char* VoxelConstants::LIGHT_GRAY_TERRACOTTA = "LIGHT_GRAY_TERRACOTTA";
const char* VoxelConstants::LIGHT_GRAY_WOOL = "LIGHT_GRAY_WOOL";
const char* VoxelConstants::LIME_CONCRETE_POWDER = "LIME_CONCRETE_POWDER";
const char* VoxelConstants::LIME_GLAZED_TERRACOTTA = "LIME_GLAZED_TERRACOTTA";
const char* VoxelConstants::LIME_TERRACOTTA = "LIME_TERRACOTTA";
const char* VoxelConstants::LIME_WOOL = "LIME_WOOL";
const char* VoxelConstants::LODESTONE_SIDE = "LODESTONE_SIDE";
const char* VoxelConstants::LODESTONE_TOP = "LODESTONE_TOP";
const char* VoxelConstants::MAGENTA_CONCRETE_POWDER = "MAGENTA_CONCRETE_POWDER";
const char* VoxelConstants::MAGENTA_GLAZED_TERRACOTTA = "MAGENTA_GLAZED_TERRACOTTA";
const char* VoxelConstants::MAGENTA_STAINED_GLASS = "MAGENTA_STAINED_GLASS";
const char* VoxelConstants::MAGENTA_TERRACOTTA = "MAGENTA_TERRACOTTA";
const char* VoxelConstants::MAGENTA_WOOL = "MAGENTA_WOOL";
const char* VoxelConstants::MAGMA = "MAGMA";
const char* VoxelConstants::MANGROVE_LEAVES = "MANGROVE_LEAVES";
const char* VoxelConstants::MANGROVE_LOG = "MANGROVE_LOG";
const char* VoxelConstants::MANGROVE_LOG_TOP = "MANGROVE_LOG_TOP";
const char* VoxelConstants::MANGROVE_PLANKS = "MANGROVE_PLANKS";
const char* VoxelConstants::MAP = "MAP";
const char* VoxelConstants::MOSSY_COBBLESTONE = "MOSSY_COBBLESTONE";
const char* VoxelConstants::MOSSY_STONE_BRICKS = "MOSSY_STONE_BRICKS";
const char* VoxelConstants::MOSS_BLOCK = "MOSS_BLOCK";
const char* VoxelConstants::MUD = "MUD";
const char* VoxelConstants::MUD_BRICKS = "MUD_BRICKS";
const char* VoxelConstants::MUSHROOM_BLOCK_INSIDE = "MUSHROOM_BLOCK_INSIDE";
const char* VoxelConstants::MUSHROOM_STEM = "MUSHROOM_STEM";
const char* VoxelConstants::MYCELIUM_TOP = "MYCELIUM_TOP";
const char* VoxelConstants::NETHERITE_BLOCK = "NETHERITE_BLOCK";
const char* VoxelConstants::NETHERRACK = "NETHERRACK";
const char* VoxelConstants::NETHER_BRICKS = "NETHER_BRICKS";
const char* VoxelConstants::NETHER_PORTAL = "NETHER_PORTAL";
const char* VoxelConstants::NETHER_WART_BLOCK = "NETHER_WART_BLOCK";
const char* VoxelConstants::OAK_LEAVES = "OAK_LEAVES";
const char* VoxelConstants::OAK_LOG = "OAK_LOG";
const char* VoxelConstants::OAK_LOG_TOP = "OAK_LOG_TOP";
const char* VoxelConstants::OAK_PLANKS = "OAK_PLANKS";
const char* VoxelConstants::OBSIDIAN = "OBSIDIAN";
const char* VoxelConstants::ORANGE_CONCRETE_POWDER = "ORANGE_CONCRETE_POWDER";
const char* VoxelConstants::ORANGE_GLAZED_TERRACOTTA = "ORANGE_GLAZED_TERRACOTTA";
const char* VoxelConstants::ORANGE_TERRACOTTA = "ORANGE_TERRACOTTA";
const char* VoxelConstants::ORANGE_WOOL = "ORANGE_WOOL";
const char* VoxelConstants::OXIDIZED_COPPER = "OXIDIZED_COPPER";
const char* VoxelConstants::PACKED_ICE = "PACKED_ICE";
const char* VoxelConstants::PACKED_MUD = "PACKED_MUD";
const char* VoxelConstants::PINK_CONCRETE = "PINK_CONCRETE";
const char* VoxelConstants::PINK_CONCRETE_POWDER = "PINK_CONCRETE_POWDER";
const char* VoxelConstants::PINK_GLAZED_TERRACOTTA = "PINK_GLAZED_TERRACOTTA";
const char* VoxelConstants::PINK_STAINED_GLASS = "PINK_STAINED_GLASS";
const char* VoxelConstants::PINK_TERRACOTTA = "PINK_TERRACOTTA";
const char* VoxelConstants::PINK_WOOL = "PINK_WOOL";
const char* VoxelConstants::PODZOL_TOP = "PODZOL_TOP";
const char* VoxelConstants::POLISHED_ANDESITE = "POLISHED_ANDESITE";
const char* VoxelConstants::POLISHED_DEEPSLATE = "POLISHED_DEEPSLATE";
const char* VoxelConstants::POLISHED_DIORITE = "POLISHED_DIORITE";
const char* VoxelConstants::POLISHED_GRANITE = "POLISHED_GRANITE";
const char* VoxelConstants::PRISMARINE = "PRISMARINE";
const char* VoxelConstants::PRISMARINE_BRICKS = "PRISMARINE_BRICKS";
const char* VoxelConstants::PURPLE_CONCRETE_POWDER = "PURPLE_CONCRETE_POWDER";
const char* VoxelConstants::PURPLE_GLAZED_TERRACOTTA = "PURPLE_GLAZED_TERRACOTTA";
const char* VoxelConstants::PURPLE_STAINED_GLASS = "PURPLE_STAINED_GLASS";
const char* VoxelConstants::PURPLE_TERRACOTTA = "PURPLE_TERRACOTTA";
const char* VoxelConstants::PURPLE_WOOL = "PURPLE_WOOL";
const char* VoxelConstants::PURPUR_BLOCK = "PURPUR_BLOCK";
const char* VoxelConstants::QUARTZ_BLOCK_BOTTOM = "QUARTZ_BLOCK_BOTTOM";
const char* VoxelConstants::QUARTZ_BLOCK_SIDE = "QUARTZ_BLOCK_SIDE";
const char* VoxelConstants::QUARTZ_BLOCK_TOP = "QUARTZ_BLOCK_TOP";
const char* VoxelConstants::QUARTZ_BRICKS = "QUARTZ_BRICKS";
const char* VoxelConstants::QUARTZ_PILLAR = "QUARTZ_PILLAR";
const char* VoxelConstants::QUARTZ_PILLAR_TOP = "QUARTZ_PILLAR_TOP";
const char* VoxelConstants::RAW_COPPER_BLOCK = "RAW_COPPER_BLOCK";
const char* VoxelConstants::RAW_GOLD_BLOCK = "RAW_GOLD_BLOCK";
const char* VoxelConstants::RAW_IRON_BLOCK = "RAW_IRON_BLOCK";
const char* VoxelConstants::REDSTONE_BLOCK = "REDSTONE_BLOCK";
const char* VoxelConstants::RED_CONCRETE = "RED_CONCRETE";
const char* VoxelConstants::RED_CONCRETE_POWDER = "RED_CONCRETE_POWDER";
const char* VoxelConstants::RED_GLAZED_TERRACOTTA = "RED_GLAZED_TERRACOTTA";
const char* VoxelConstants::RED_MUSHROOM_BLOCK = "RED_MUSHROOM_BLOCK";
const char* VoxelConstants::RED_NETHER_BRICKS = "RED_NETHER_BRICKS";
const char* VoxelConstants::RED_SAND = "RED_SAND";
const char* VoxelConstants::RED_SANDSTONE = "RED_SANDSTONE";
const char* VoxelConstants::RED_SANDSTONE_TOP = "RED_SANDSTONE_TOP";
const char* VoxelConstants::RED_TERRACOTTA = "RED_TERRACOTTA";
const char* VoxelConstants::RED_WINE = "RED_WINE";
const char* VoxelConstants::RED_WOOL = "RED_WOOL";
const char* VoxelConstants::ROOTED_DIRT = "ROOTED_DIRT";
const char* VoxelConstants::SAND = "SAND";
const char* VoxelConstants::SANDSTONE = "SANDSTONE";
const char* VoxelConstants::SANDSTONE_TOP = "SANDSTONE_TOP";
const char* VoxelConstants::SCULK = "SCULK";
const char* VoxelConstants::SEA_LANTERN = "SEA_LANTERN";
const char* VoxelConstants::SMOOTH_BASALT = "SMOOTH_BASALT";
const char* VoxelConstants::SMOOTH_STONE = "SMOOTH_STONE";
const char* VoxelConstants::SMOOTH_STONE_SLAB_SIDE = "SMOOTH_STONE_SLAB_SIDE";
const char* VoxelConstants::SNOW = "SNOW";
const char* VoxelConstants::SOUL_SAND = "SOUL_SAND";
const char* VoxelConstants::SOUL_SOIL = "SOUL_SOIL";
const char* VoxelConstants::SPAWNER = "SPAWNER";
const char* VoxelConstants::SPONGE = "SPONGE";
const char* VoxelConstants::SPRUCE_LEAVES = "SPRUCE_LEAVES";
const char* VoxelConstants::SPRUCE_LOG = "SPRUCE_LOG";
const char* VoxelConstants::SPRUCE_PLANKS = "SPRUCE_PLANKS";
const char* VoxelConstants::STONE = "STONE";
const char* VoxelConstants::STONE_BRICKS = "STONE_BRICKS";
const char* VoxelConstants::STRIPPED_BAMBOO_BLOCK = "STRIPPED_BAMBOO_BLOCK";
const char* VoxelConstants::SUSPICIOUS_GRAVEL_0 = "SUSPICIOUS_GRAVEL_0";
const char* VoxelConstants::SUSPICIOUS_SAND_0 = "SUSPICIOUS_SAND_0";
const char* VoxelConstants::TERRACOTTA = "TERRACOTTA";
const char* VoxelConstants::TINTED_GLASS = "TINTED_GLASS";
const char* VoxelConstants::TUFF = "TUFF";
const char* VoxelConstants::WARPED_PLANKS = "WARPED_PLANKS";
const char* VoxelConstants::WARPED_STEM = "WARPED_STEM";
const char* VoxelConstants::WARPED_STEM_TOP = "WARPED_STEM_TOP";
const char* VoxelConstants::WARPED_WART_BLOCK = "WARPED_WART_BLOCK";
const char* VoxelConstants::WEATHERED_COPPER = "WEATHERED_COPPER";
const char* VoxelConstants::WET_SPONGE = "WET_SPONGE";
const char* VoxelConstants::WHITE_CONCRETE = "WHITE_CONCRETE";
const char* VoxelConstants::WHITE_CONCRETE_POWDER = "WHITE_CONCRETE_POWDER";
const char* VoxelConstants::WHITE_GLAZED_TERRACOTTA = "WHITE_GLAZED_TERRACOTTA";
const char* VoxelConstants::WHITE_STAINED_GLASS = "WHITE_STAINED_GLASS";
const char* VoxelConstants::WHITE_TERRACOTTA = "WHITE_TERRACOTTA";
const char* VoxelConstants::WHITE_WOOL = "WHITE_WOOL";
const char* VoxelConstants::YELLOW_CONCRETE_POWDER = "YELLOW_CONCRETE_POWDER";
const char* VoxelConstants::YELLOW_GLAZED_TERRACOTTA = "YELLOW_GLAZED_TERRACOTTA";
const char* VoxelConstants::YELLOW_TERRACOTTA = "YELLOW_TERRACOTTA";
const char* VoxelConstants::YELLOW_WOOL = "YELLOW_WOOL";

#include <Core/Collision/CollisionComponents.h>

std::vector<VoxelConstants::DiffuseNormalMR> VoxelConstants::faces;
mem::unordered_stringmap<uint16_t> VoxelConstants::voxelFaceToIndex;
ShaderStorageBuffer VoxelConstants::shaderMaterialLinker;

BVH<BVHCollisionNode<AABB>> VoxelWorldSystem::buildCollisionMesh(Chunk &chunk)
{
    return BVH<BVHCollisionNode<AABB>>(chunk.quads, 4);
}

std::vector<Quad> VoxelWorldSystem::greedyMesh(const VoxelVolume &volume, glm::ivec3 dims)
{
    auto f = [&](int x, int y, int z, Face f) {
        if (x < 0 || y < 0 || z < 0 || x >= dims.x || y >= dims.y || z >= dims.z)
            return 0u;
        return volume.at(x, y, z, int(f));
    };

    std::vector<Quad> quads;
    quads.reserve(32);

    for (int d = 0; d < 3; ++d) {
        int u = (d + 1) % 3;
        int v = (d + 2) % 3;
        glm::ivec3 x{};
        glm::ivec3 q{};
        q[d] = 1;

        Face posFace = static_cast<Face>(d * 2 + 1);
        Face negFace = static_cast<Face>(d * 2 + 0);

        bool dirMask[256];
        unsigned textureMask[256];

        for (x[d] = -1; x[d] < dims[d]; ) {
            int n = 0;

            for (x[v] = 0; x[v] < dims[v]; ++x[v]) {
                for (x[u] = 0; x[u] < dims[u]; ++x[u]) {
                    glm::vec3 neightbor = x + q;
                    unsigned a = f(x.x, x.y, x.z, posFace);
                    unsigned b = f(neightbor.x, neightbor.y, neightbor.z, negFace);
                    dirMask[n] = (a && !b);
                    unsigned currentTex = 0;
                    if (a == 0 && b > 0) {
                        currentTex = b;
                    } else if (a > 0 && b == 0) {
                        currentTex = a;
                    } else {
                        currentTex = 0;
                    }
                    textureMask[n] = currentTex;
                    ++n;
                }
            }
            
            ++x[d];
            n = 0;
            for (int j = 0; j < dims[v]; ++j) {
                for (int i = 0; i < dims[u]; ) {
                    if (textureMask[n]) {
                        const bool currentDir = dirMask[n];
                        const unsigned currentTex = textureMask[n];
                        int w;
                        for (w = 1; i + w < dims[u] && currentTex == textureMask[n + w] && textureMask[n + w] != 0 && dirMask[n + w] == currentDir; ++w);

                        int h;
                        for (h = 1; j + h < dims[v]; ++h) {
                            int hIdx = n + h * dims[u];
                            for (int k = 0; k < w; ++k) {
                                int idx = hIdx + k;
                                if (dirMask[idx] != currentDir || currentTex != textureMask[idx]) {
                                    goto Done;
                                }
                            }
                        }
                    Done:
                        x[u] = i;
                        x[v] = j;

                        glm::ivec3 axis{}; 
                        axis[u] = w;
                        axis[v] = h;

                        Face face = static_cast<Face>(d * 2 + (currentDir ? 1 : 0));
                        quads.emplace_back(face, AABB::fromTo(x, x + axis), currentTex);

                        for (int l = 0; l < h; ++l) {
                            int idx = n + l * dims[u];
                            for (int k = 0; k < w; ++k) {
                                textureMask[idx + k] = 0;
                                dirMask[idx + k] = false;
                            }
                        }
                            
                        i += w;
                        n += w;
                    } else {
                        ++i;
                        ++n;
                    }
                }
            }
        }
    }
    return quads;
}

AABB quadToAABB(const std::array<glm::ivec3, 4>& corners) {
    glm::ivec3 minCorner = corners[0];
    glm::ivec3 maxCorner = corners[0];

    for (int i = 1; i < 4; ++i) {
        minCorner = glm::min(minCorner, corners[i]);
        maxCorner = glm::max(maxCorner, corners[i]);
    }
    return AABB::fromTo(glm::vec3(minCorner), glm::vec3(maxCorner));
}

glm::ivec2 getWidthHeight(Face face, const AABB& aabb) {
    auto min = aabb.min();
    auto max = aabb.max();
    glm::ivec2 aSize;
    switch (face) {
        case Face::Y_POS:
        case Face::Y_NEG:
            aSize = glm::ivec2(max.x - min.x, max.z - min.z);
            break;
        case Face::Z_POS:
        case Face::Z_NEG:
            aSize = glm::ivec2(max.x - min.x, max.y - min.y);
            break;
        case Face::X_NEG:
        case Face::X_POS:
            aSize = glm::ivec2(max.z - min.z, max.y - min.y);
            break;
    }
    return aSize;
}

glm::vec3 faceOffset(Face face) {
    switch (face) {
        case Face::X_POS: return {1, 0, 0};
        case Face::X_NEG: return {0, 0, 0};
        case Face::Y_POS: return {0, 1, 0};
        case Face::Y_NEG: return {0, 0, 0};
        case Face::Z_POS: return {0, 0, 1};
        case Face::Z_NEG: return {0, 0, 0};
    }
    return {0,0,0};
}

glm::mat4 getFaceRotation(Face face) {
    switch (face) {
        case Face::Y_NEG: // base orientation
            return glm::mat4(1.0f);

        case Face::Y_POS: // rotate 180° around X
            return glm::rotate(glm::mat4(1.0f), glm::radians(180.f), glm::vec3(1, 0, 0));

        case Face::Z_POS: // rotate -90° around X
            return glm::rotate(glm::mat4(1.0f), glm::radians(-90.f), glm::vec3(1, 0, 0));

        case Face::Z_NEG: // rotate +90° around X
            return glm::rotate(glm::mat4(1.0f), glm::radians(90.f), glm::vec3(1, 0, 0));

        case Face::X_POS: // rotate +90° around Z
            return glm::rotate(glm::mat4(1.0f), glm::radians(90.f), glm::vec3(0, 0, 1));

        case Face::X_NEG: // rotate -90° around Z
            return glm::rotate(glm::mat4(1.0f), glm::radians(-90.f), glm::vec3(0, 0, 1));
    }
    return glm::mat4(1.0f);
}

glm::vec3 faceRotationOffset(Face face, glm::vec2 aSize) {
    switch (face) {
        case Face::Y_POS: return {0, 0, -1};
        case Face::Y_NEG: return {0, 0, 0};
        case Face::Z_POS: return {0, 0, 0};
        case Face::Z_NEG: return {0, 0, -1};
        case Face::X_POS: return {0, 0, 0};
        case Face::X_NEG: return {-1, 0, 0};
    }
    return {};
}

glm::mat4 createInstanceModel(const Quad& q,glm::vec3 position, glm::vec3 min, glm::ivec2 aSize) {
    glm::vec3 scale;
    switch (q.face) {
        case Face::Y_NEG:
        case Face::Y_POS:
            scale = glm::vec3(aSize.x, 1, aSize.y);
            break;
        case Face::X_POS:
        case Face::X_NEG:
            scale = glm::vec3(1, aSize.y, aSize.x);
            break;
        case Face::Z_POS:
        case Face::Z_NEG:
            scale = glm::vec3(aSize.x, aSize.y, 1);
            break;
    }
    const auto translation = glm::translate(glm::mat4(1.f), position + min);
    const auto rotation = getFaceRotation(q.face);
    const auto scaleMat = glm::scale(glm::mat4(1.f), scale);
    return (translation * scaleMat * rotation) * glm::translate(glm::mat4(1.0f), faceRotationOffset(q.face, aSize));
}

void VoxelWorldSystem::buildChunkMesh(const VoxelVolume& volume, Chunk &mesh)
{
    auto now = std::chrono::high_resolution_clock::now();
    auto quads = greedyMesh(volume, glm::ivec3(16));
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Greedy meshing took " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - now).count() << " seconds" << std::endl;

    mesh.drawCommands.clear();
    mesh.quads.clear();

    auto ptr = vertexPool.vInstancePtr;
    if (mesh.commands > 0) {
        vertexPool.free(mesh.startingIndex, mesh.commands);
    }
    auto idx = vertexPool.allocate(quads.size());

    mesh.startingIndex = idx;
    mesh.commands = quads.size();
    for (const Quad& q : quads) {
        const AABB& aabb = q.aabb;

        glm::ivec2 aSize = getWidthHeight(q.face, aabb);

        ptr[idx++] = QuadDrawInstance(
            createInstanceModel(q, glm::vec3(mesh.coords) * 16.f, aabb.min(), aSize),
            q.texture, q.face, aSize
        );
        mesh.quads.emplace_back(aabb, 0);
    }
}

void VoxelWorldSystem::buildVolume(const VoxelVolume& volume, Chunk& chunk) {
    buildChunkMesh(volume, chunk);
    buildCollisionMesh(chunk);
    chunk.dirty = true;
}

void VoxelWorldSystem::onUpdate(LevelUpdateView<VoxelWorldSystem>& view) {
    for (const auto& dirty : dirtyChunks) {
        auto& chunk = chunks[dirty];
        auto& volume = *view.get<VoxelVolume>(chunk.voxelVolumeEntity);
        buildVolume(volume, chunk);
    }
    dirtyChunks.clear();
}

RenderPassContext VoxelWorldSystem::onRenderNew(RenderInfo& info, RenderScene &rScene, const std::unordered_set<glm::ivec3, IVec3Hash> &visibleChunks)
{
    RenderPassContext voxelCtx;
    voxelCtx.shader = "VoxelShader";
    unsigned materialsID = rScene.materials.getMaterialID(MaterialConstants::MINECRAFT_BLOCKS_DIFFUSE);
    unsigned normalsID = rScene.materials.getMaterialID(MaterialConstants::MINECRAFT_BLOCKS_NORMAL);
    unsigned specID = rScene.materials.getMaterialID(MaterialConstants::MINECRAFT_BLOCKS_SPECULAR);

    voxelCtx.materials.materials.emplace_back("materials", materialsID, MaterialHandle::Material::Type::TEXTURE_ARRAY_2D);
    voxelCtx.materials.materials.emplace_back("normalMap", normalsID, MaterialHandle::Material::Type::TEXTURE_ARRAY_2D);
    voxelCtx.materials.materials.emplace_back("specMap", specID, MaterialHandle::Material::Type::TEXTURE_ARRAY_2D);

    voxelCtx.buffers.push_back(BufferHandle(BufferHandleTarget::SSBO, FacesUBO.id(), 1));
    voxelCtx.buffers.push_back(BufferHandle(BufferHandleTarget::SSBO, VoxelConstants::getMaterialLinker().id(), 2));

    auto& instances = voxelCtx.instances;
    instances.reserve(visibleChunks.size());

    auto ptr = vertexPool.vInstancePtr;
    for (const auto& visible : visibleChunks) {
        const auto it = chunks.find(visible);
        if (it == chunks.end()) {
            continue;
        }
        auto& chunk = it->second;
        Renderable renderable;
        renderable.VAO = vertexPool.poolVAO.id();
        renderable.cmd.baseInstance = chunk.startingIndex;
        renderable.cmd.instanceCount = chunk.commands;
        renderable.cmd.baseVertex = 0;
        renderable.cmd.firstIndex = 0;
        renderable.cmd.count = 6;
        instances.push_back(renderable);    
    }

    voxelCtx.buffers.push_back(BufferHandle(BufferHandleTarget::SSBO, vertexPool.instancePool.id(), 3));

    voxelCtx.isSorted = true;
    return voxelCtx;
}