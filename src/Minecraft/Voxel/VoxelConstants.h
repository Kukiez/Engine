#pragma once
#include <vector>

#include "openGL/BufferObjects/ShaderStorageBuffer.h"
#include <memory/hash.h>


class VoxelConstants {
    struct DiffuseNormalMR {
        unsigned diffuse = 0;
        unsigned metallicRoughness = 0;
        unsigned normal = 0;
        unsigned _;
    };
    static std::vector<DiffuseNormalMR> faces;
    static mem::unordered_stringmap<uint16_t> voxelFaceToIndex;
    static ShaderStorageBuffer shaderMaterialLinker;
public:
    static const char* NULL_FACE;
    static const char* ACACIA_LEAVES;
    static const char* ACACIA_LOG;
    static const char* ACACIA_PLANKS;
    static const char* AMETHYST_BLOCK;
    static const char* ANCIENT_DEBRIS;
    static const char* ANDESITE;
    static const char* ANVIL;
    static const char* ANVIL_TOP;
    static const char* BAMBOO_BLOCK;
    static const char* BAMBOO_PLANKS;
    static const char* BASALT_SIDE;
    static const char* BASALT_TOP;
    static const char* BEDROCK;
    static const char* BIRCH_LEAVES;
    static const char* BIRCH_LOG;
    static const char* BIRCH_PLANKS;
    static const char* BLACKSTONE;
    static const char* BLACKSTONE_TOP;
    static const char* BLACK_CONCRETE;
    static const char* BLACK_CONCRETE_POWDER;
    static const char* BLACK_GLAZED_TERRACOTTA;
    static const char* BLACK_TERRACOTTA;
    static const char* BLACK_WOOL;
    static const char* BLUE_CONCRETE_POWDER;
    static const char* BLUE_GLAZED_TERRACOTTA;
    static const char* BLUE_ICE;
    static const char* BLUE_TERRACOTTA;
    static const char* BLUE_WOOL;
    static const char* BONE_BLOCK_SIDE;
    static const char* BONE_BLOCK_TOP;
    static const char* BRICKS;
    static const char* BROWN_CONCRETE;
    static const char* BROWN_CONCRETE_POWDER;
    static const char* BROWN_GLAZED_TERRACOTTA;
    static const char* BROWN_MUSHROOM_BLOCK;
    static const char* BROWN_TERRACOTTA;
    static const char* BROWN_WOOL;
    static const char* BUDDING_AMETHYST;
    static const char* CALCITE;
    static const char* CHERRY_LOG;
    static const char* CHERRY_LOG_TOP;
    static const char* CHERRY_PLANKS;
    static const char* CHIPPED_ANVIL_TOP;
    static const char* CHISELED_DEEPSLATE;
    static const char* CHISELED_QUARTZ_BLOCK;
    static const char* CHISELED_QUARTZ_BLOCK_TOP;
    static const char* CHISELED_RED_SANDSTONE;
    static const char* CHISELED_SANDSTONE;
    static const char* CHISELED_STONE_BRICKS;
    static const char* CLAY;
    static const char* COAL_BLOCK;
    static const char* COARSE_DIRT;
    static const char* COBBLED_DEEPSLATE;
    static const char* COBBLESTONE;
    static const char* COMPOSTER_BOTTOM;
    static const char* COMPOSTER_COMPOST;
    static const char* COMPOSTER_READY;
    static const char* COMPOSTER_SIDE;
    static const char* COMPOSTER_TOP;
    static const char* COPPER_BLOCK;
    static const char* CRACKED_DEEPSLATE_BRICKS;
    static const char* CRACKED_NETHER_BRICKS;
    static const char* CRACKED_STONE_BRICKS;
    static const char* CRIMSON_PLANKS;
    static const char* CRIMSON_STEM;
    static const char* CRIMSON_STEM_TOP;
    static const char* CRYING_OBSIDIAN;
    static const char* CUT_COPPER;
    static const char* CUT_RED_SANDSTONE;
    static const char* CUT_SANDSTONE;
    static const char* CYAN_CONCRETE_POWDER;
    static const char* CYAN_GLAZED_TERRACOTTA;
    static const char* CYAN_STAINED_GLASS;
    static const char* CYAN_TERRACOTTA;
    static const char* CYAN_WOOL;
    static const char* DAMAGED_ANVIL_TOP;
    static const char* DARK_OAK_LEAVES;
    static const char* DARK_OAK_LOG;
    static const char* DARK_OAK_PLANKS;
    static const char* DARK_PRISMARINE;
    static const char* DAYLIGHT_DETECTOR_INVERTED_TOP;
    static const char* DAYLIGHT_DETECTOR_SIDE;
    static const char* DAYLIGHT_DETECTOR_TOP;
    static const char* DEEPSLATE;
    static const char* DEEPSLATE_BRICKS;
    static const char* DEEPSLATE_TILES;
    static const char* DEEPSLATE_TOP;
    static const char* DIAMOND_BLOCK;
    static const char* DIORITE;
    static const char* DIRT_PATH_TOP;
    static const char* DRAGON_EGG;
    static const char* DRIED_KELP_BOTTOM;
    static const char* DRIED_KELP_TOP;
    static const char* DRIPSTONE_BLOCK;
    static const char* EMERALD_BLOCK;
    static const char* END_STONE;
    static const char* END_STONE_BRICKS;
    static const char* EXPOSED_COPPER;
    static const char* FERN;
    static const char* GLOWSTONE;
    static const char* GOLD_BLOCK;
    static const char* GRANITE;
    static const char* GRASS;
    static const char* GRASS_BLOCK_TOP;
    static const char* GRAVEL;
    static const char* GRAY_CONCRETE;
    static const char* GRAY_CONCRETE_POWDER;
    static const char* GRAY_GLAZED_TERRACOTTA;
    static const char* GRAY_TERRACOTTA;
    static const char* GRAY_WOOL;
    static const char* GREEN_CONCRETE_POWDER;
    static const char* GREEN_GLAZED_TERRACOTTA;
    static const char* GREEN_TERRACOTTA;
    static const char* GREEN_WOOL;
    static const char* HAY_BLOCK_SIDE;
    static const char* HAY_BLOCK_TOP;
    static const char* IRON_BARS;
    static const char* IRON_BLOCK;
    static const char* JUNGLE_LEAVES;
    static const char* JUNGLE_LOG;
    static const char* JUNGLE_PLANKS;
    static const char* LAPIS_BLOCK;
    static const char* LIGHT_BLUE_CONCRETE_POWDER;
    static const char* LIGHT_BLUE_GLAZED_TERRACOTTA;
    static const char* LIGHT_BLUE_STAINED_GLASS;
    static const char* LIGHT_BLUE_TERRACOTTA;
    static const char* LIGHT_BLUE_WOOL;
    static const char* LIGHT_GRAY_CONCRETE;
    static const char* LIGHT_GRAY_CONCRETE_POWDER;
    static const char* LIGHT_GRAY_GLAZED_TERRACOTTA;
    static const char* LIGHT_GRAY_TERRACOTTA;
    static const char* LIGHT_GRAY_WOOL;
    static const char* LIME_CONCRETE_POWDER;
    static const char* LIME_GLAZED_TERRACOTTA;
    static const char* LIME_TERRACOTTA;
    static const char* LIME_WOOL;
    static const char* LODESTONE_SIDE;
    static const char* LODESTONE_TOP;
    static const char* MAGENTA_CONCRETE_POWDER;
    static const char* MAGENTA_GLAZED_TERRACOTTA;
    static const char* MAGENTA_STAINED_GLASS;
    static const char* MAGENTA_TERRACOTTA;
    static const char* MAGENTA_WOOL;
    static const char* MAGMA;
    static const char* MANGROVE_LEAVES;
    static const char* MANGROVE_LOG;
    static const char* MANGROVE_LOG_TOP;
    static const char* MANGROVE_PLANKS;
    static const char* MAP;
    static const char* MOSSY_COBBLESTONE;
    static const char* MOSSY_STONE_BRICKS;
    static const char* MOSS_BLOCK;
    static const char* MUD;
    static const char* MUD_BRICKS;
    static const char* MUSHROOM_BLOCK_INSIDE;
    static const char* MUSHROOM_STEM;
    static const char* MYCELIUM_TOP;
    static const char* NETHERITE_BLOCK;
    static const char* NETHERRACK;
    static const char* NETHER_BRICKS;
    static const char* NETHER_PORTAL;
    static const char* NETHER_WART_BLOCK;
    static const char* OAK_LEAVES;
    static const char* OAK_LOG;
    static const char* OAK_LOG_TOP;
    static const char* OAK_PLANKS;
    static const char* OBSIDIAN;
    static const char* ORANGE_CONCRETE_POWDER;
    static const char* ORANGE_GLAZED_TERRACOTTA;
    static const char* ORANGE_TERRACOTTA;
    static const char* ORANGE_WOOL;
    static const char* OXIDIZED_COPPER;
    static const char* PACKED_ICE;
    static const char* PACKED_MUD;
    static const char* PINK_CONCRETE;
    static const char* PINK_CONCRETE_POWDER;
    static const char* PINK_GLAZED_TERRACOTTA;
    static const char* PINK_STAINED_GLASS;
    static const char* PINK_TERRACOTTA;
    static const char* PINK_WOOL;
    static const char* PODZOL_TOP;
    static const char* POLISHED_ANDESITE;
    static const char* POLISHED_DEEPSLATE;
    static const char* POLISHED_DIORITE;
    static const char* POLISHED_GRANITE;
    static const char* PRISMARINE;
    static const char* PRISMARINE_BRICKS;
    static const char* PURPLE_CONCRETE_POWDER;
    static const char* PURPLE_GLAZED_TERRACOTTA;
    static const char* PURPLE_STAINED_GLASS;
    static const char* PURPLE_TERRACOTTA;
    static const char* PURPLE_WOOL;
    static const char* PURPUR_BLOCK;
    static const char* QUARTZ_BLOCK_BOTTOM;
    static const char* QUARTZ_BLOCK_SIDE;
    static const char* QUARTZ_BLOCK_TOP;
    static const char* QUARTZ_BRICKS;
    static const char* QUARTZ_PILLAR;
    static const char* QUARTZ_PILLAR_TOP;
    static const char* RAW_COPPER_BLOCK;
    static const char* RAW_GOLD_BLOCK;
    static const char* RAW_IRON_BLOCK;
    static const char* REDSTONE_BLOCK;
    static const char* RED_CONCRETE;
    static const char* RED_CONCRETE_POWDER;
    static const char* RED_GLAZED_TERRACOTTA;
    static const char* RED_MUSHROOM_BLOCK;
    static const char* RED_NETHER_BRICKS;
    static const char* RED_SAND;
    static const char* RED_SANDSTONE;
    static const char* RED_SANDSTONE_TOP;
    static const char* RED_TERRACOTTA;
    static const char* RED_WINE;
    static const char* RED_WOOL;
    static const char* ROOTED_DIRT;
    static const char* SAND;
    static const char* SANDSTONE;
    static const char* SANDSTONE_TOP;
    static const char* SCULK;
    static const char* SEA_LANTERN;
    static const char* SMOOTH_BASALT;
    static const char* SMOOTH_STONE;
    static const char* SMOOTH_STONE_SLAB_SIDE;
    static const char* SNOW;
    static const char* SOUL_SAND;
    static const char* SOUL_SOIL;
    static const char* SPAWNER;
    static const char* SPONGE;
    static const char* SPRUCE_LEAVES;
    static const char* SPRUCE_LOG;
    static const char* SPRUCE_PLANKS;
    static const char* STONE;
    static const char* STONE_BRICKS;
    static const char* STRIPPED_BAMBOO_BLOCK;
    static const char* SUSPICIOUS_GRAVEL_0;
    static const char* SUSPICIOUS_SAND_0;
    static const char* TERRACOTTA;
    static const char* TINTED_GLASS;
    static const char* TUFF;
    static const char* WARPED_PLANKS;
    static const char* WARPED_STEM;
    static const char* WARPED_STEM_TOP;
    static const char* WARPED_WART_BLOCK;
    static const char* WEATHERED_COPPER;
    static const char* WET_SPONGE;
    static const char* WHITE_CONCRETE;
    static const char* WHITE_CONCRETE_POWDER;
    static const char* WHITE_GLAZED_TERRACOTTA;
    static const char* WHITE_STAINED_GLASS;
    static const char* WHITE_TERRACOTTA;
    static const char* WHITE_WOOL;
    static const char* YELLOW_CONCRETE_POWDER;
    static const char* YELLOW_GLAZED_TERRACOTTA;
    static const char* YELLOW_TERRACOTTA;
    static const char* YELLOW_WOOL;

    static std::string extractBaseName(const std::string& path) {
        size_t lastSlash = path.find_last_of("/\\");
        size_t extension = path.find_last_of(".");
        std::string result = path.substr(lastSlash + 1, extension - lastSlash - 1);
        std::ranges::transform(result, result.begin(), toupper);
        return result;
    }

    static void init(const std::vector<TextureData>& diffuse,
                     const std::vector<TextureData>& normal,
                     const std::vector<TextureData>& spec)
    {
        faces.assign(diffuse.size() + 1, DiffuseNormalMR());

        for (int i = 0; i < diffuse.size(); i++) {
            std::string item = extractBaseName(diffuse[i].path);
            faces[i + 1].diffuse = i;
            voxelFaceToIndex.emplace(item, i + 1);
        }

        for (int i = 0; i < normal.size(); i++) {
            std::string item = extractBaseName(normal[i].path);
            if (item != NULL_FACE) {
                item.pop_back(); item.pop_back();
            }
            auto it = voxelFaceToIndex.find(item);
            if (it != voxelFaceToIndex.end()) {
                faces[it->second].normal = i;
            } else {
                std::cerr << "normal map '" << item << "' has no matching diffuse." << std::endl;
            }
        }

        for (int i = 0; i < spec.size(); i++) {
            std::string item = extractBaseName(spec[i].path);
            if (item != NULL_FACE) {
                item.pop_back(); item.pop_back();
            }
            auto it = voxelFaceToIndex.find(item);
            if (it != voxelFaceToIndex.end()) {
                faces[it->second].metallicRoughness = i;
            } else {
                std::cerr << "specular map '" << item << "' has no matching diffuse." << std::endl;
            }
        }
        shaderMaterialLinker.allocate(sizeof(DiffuseNormalMR) * faces.size(), BufferUsage::STATIC, faces.data());
    }

    static uint16_t getFace(const std::string_view face) {
        if (const auto it = voxelFaceToIndex.find(face); it != voxelFaceToIndex.end()) {
            return it->second;
        }
        return 0;
    }

    static std::array<uint16_t, 6> toVoxel(const std::string_view face) {
        auto voxel = getFace(face);
        return std::array{
            voxel, voxel, voxel, voxel, voxel, voxel
        };
    }

    static const ShaderStorageBuffer& getMaterialLinker() {
        return shaderMaterialLinker;
    }
};
