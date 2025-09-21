#pragma once
#include <EntityManager.h>
#include <Scene.h>
#include <vector>
#include <openGL/BufferObjects/ShaderStorageBuffer.h>
#include <Renderer/Renderer.h>
#include <String/FontLoader.h>

#include "../EventSystem.h"
#include "../ResourceSystem.h"
#include "../Components/UIComponents.h"
#include <String/TemplateString.h>
#include <stacktrace>
#include <openGL/shaders/UniformData.h>
#include <Renderer/Rasterizer/Rasterizer.h>

#include "RenderInfo.h"

#define STD430_ALIGN alignas(16)

struct CharInfo {
    glm::vec3 pos;
    float degrees;
    glm::vec4 color;
    glm::vec2 rotation;
    int layer;
    int visible;
};

struct RichTextRenderParser {
    static int processGradientEndNode(auto begin, auto end, PlaceholderSystem* placeholderSystem) {
        int len = 0;

        for (auto it = begin; it != end; ++it) {
            std::visit([&len, &begin, &end, placeholderSystem]<typename T>(T&& v) {
                if constexpr (std::is_same_v<std::decay_t<T>, TextNode>) {
                    len += v.text.size();
                } else if constexpr (std::is_same_v<std::decay_t<T>, PlaceholderNode>) {
                    len += v.parse(*placeholderSystem).size();
                } else if constexpr (std::is_same_v<std::decay_t<T>, GradientNodeEnd>) {
                    begin = end;
                }
            }, *it);
        }
        return len;
    }

    static size_t processTextNode(const std::string& text, TextState& state, auto*& writePointer) {
        for (auto it = text.begin(); it != text.end(); ++it) {
            if (*it > 128 || *it < 0) continue;

            const Character ch = state.font->characters[*it];
            state.xpos = state.x + ch.bearing.x * state.scale;
            state.ypos = state.y + (256.0f - ch.bearing.y) * state.scale;

            if (state.activeGradient) {
                state.color = state.activeGradient->advance();
            }

            new (writePointer) CharInfo{
                glm::vec3(state.xpos, state.ypos, state.scale * 256),
                state.rotation,
                state.color,
                glm::vec2(state.beginX, state.beginY),
                static_cast<int>(ch.ID),
                1
            };
            ++writePointer;
            state.x += (ch.advance >> 6) * state.scale;
        }
        return text.size();
    }

    static void parseText(PlaceholderSystem* placeholderSystem, const Font* font, RichText &uitext, const UITransform& transform, CharInfo*& writePointer) {
        auto& text = uitext;

        TextState state;
        state.beginX = transform.world.pos.x;
        state.beginY = transform.world.pos.y;
        state.beginZ = 0.0f;
        state.zpos = 0.0f;
        state.x = transform.world.pos.x;
        state.y = transform.world.pos.y;
        state.z = 0.0f;
        state.rotation = transform.world.rotation;
        state.scale = transform.world.scale.x / 256.0f;
        state.color = glm::vec4(1);
        state.font = font;
        state.offset = 0;

        for (auto node = text.begin(); node != text.end(); ++node) {
            std::visit([&]<typename T>(T&& arg) {
                if constexpr (std::is_same_v<std::decay_t<T>, TextNode>) {
                    processTextNode(arg.text, state,  writePointer);
                } else if constexpr (std::is_same_v<std::decay_t<T>, PlaceholderNode>) {
                    processTextNode( arg.parse(*placeholderSystem), state, writePointer);
                } else if constexpr (std::is_same_v<std::decay_t<T>, GradientNode>) {
                    arg.process(state);
                    state.activeGradient->length = processGradientEndNode(node, text.end(), placeholderSystem);
                    state.activeGradient->current = 0;
                } else {
                    arg.process(state);
                }
            }, *node);
        }
    }
};

class TextBufferBlock {
    struct FreeSpot {
        int index;
        int length;
    };

    void reallocateBlock(const int overflow);
    FreeSpot findFreeSpot(const int length);
    void removeFreeSpot(const std::unordered_map<size_t, FreeSpot>::iterator &iterator);
    void removeFreeSpotBatchedPointer(CharInfo* basePointer, const std::vector<size_t>& spots);

    friend class TextRenderingSystem;

    std::vector<FreeSpot> freeSpots; // unoccupied index-length's
    std::unordered_map<size_t, FreeSpot> hashes; // occupied index-length's

    ShaderStorageBuffer block;
    PlaceholderSystem* placeholderSystem = nullptr;
    ResourceSystem* resourceSystem = nullptr;

    int highestIndex = 0;
    glm::vec4 scissors = glm::vec4(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    int yOffset = 0;
    int xOffset = 0;
public:
    TextBufferBlock() {}

    explicit TextBufferBlock(PlaceholderSystem* placeholderSystem, ResourceSystem* resourceSystem, const int bytes)
    : placeholderSystem(placeholderSystem), resourceSystem(resourceSystem) {
        block.allocate(bytes, BufferUsage::DYNAMIC);
        freeSpots.emplace_back(0, bytes / sizeof(CharInfo));
    }

    TextBufferBlock(const TextBufferBlock& other) = delete;
    TextBufferBlock& operator=(const TextBufferBlock& other) = delete;

    TextBufferBlock(TextBufferBlock&& other) noexcept
        : freeSpots(std::move(other.freeSpots)), hashes(std::move(other.hashes)), block(std::move(other.block)), placeholderSystem(other.placeholderSystem),
          resourceSystem(other.resourceSystem), highestIndex(other.highestIndex), scissors(other.scissors) {}

    TextBufferBlock& operator=(TextBufferBlock&& other) noexcept {
        if (this != &other) {
            freeSpots = std::move(other.freeSpots);
            hashes = std::move(other.hashes);
            block = std::move(other.block);
            placeholderSystem = other.placeholderSystem;
            resourceSystem = other.resourceSystem;
            highestIndex = other.highestIndex;
            scissors = other.scissors;
        }
        return *this;
    }

    bool find(const size_t hash) const {
        return hashes.contains(hash);
    }

    void render(UIRichText& text, const UITransform& transform) {
        const int length = text.text.length(*placeholderSystem);

        FreeSpot spot;

        if (const auto it = hashes.find(text.hash()); it != hashes.end()) {
            if (it->second.length != length) {
                removeFreeSpot(it);
                spot = findFreeSpot(length);
                hashes.emplace(text.hash(), spot);
            } else {
                spot = it->second;
            }
        } else {
            spot = findFreeSpot(length);
            hashes.emplace(text.hash(), spot);
        }
        const auto mapped = block.map(BufferAccessMode::WRITE_ONLY);
        CharInfo* writePointer = mapped.get<CharInfo>() + spot.index;

        RichTextRenderParser::parseText(placeholderSystem, resourceSystem->getFont(text.font), text.text, transform, writePointer);
    }

    void renderPositionOffset(const UIRichText& text, const glm::vec2& offset) {
        if (const auto it = hashes.find(text.hash()); it != hashes.end()) {
            auto [index, length] = it->second;

            const auto mapped = block.map(BufferAccessMode::WRITE_ONLY);
            CharInfo* writePointer = mapped.get<CharInfo>() + index;

            for (int i = 0; i < length; ++i) {
                writePointer->pos += glm::vec3(offset.x, offset.y, 0);
                ++writePointer;
            }
        }
    }

    void remove(const UIRichText& text) {
        if (const auto it = hashes.find(text.hash()); it != hashes.end()) {
            removeFreeSpot(it);
        } else {
            std::cout << "String Not Found: " << text.text << std::endl;
            printStackTrace();
        }
    }

    void setBorder(const float x, const float y, const float w, const float h) {
        scissors.x = x;
        scissors.y = y;
        scissors.z = w;
        scissors.w = h;
    }

    void setYOffset(const float y) {
        yOffset = y;
    }

    void setXOffset(const float x) {
        xOffset = x;
    }


    void move(TextBufferBlock* to, const UIRichText& text) {
        if (const auto it = hashes.find(text.hash()); it != hashes.end()) {
            {
                FreeSpot src = it->second;
                FreeSpot dst = to->findFreeSpot(src.length);

                const auto self = block.map(BufferAccessMode::WRITE_ONLY);
                const auto other = to->block.map(BufferAccessMode::WRITE_ONLY);

                const auto* srcPtr = self.get<CharInfo>() + src.index;
                auto* dstPtr = other.get<CharInfo>() + dst.index;

                memcpy(dstPtr, srcPtr, sizeof(CharInfo) * src.length);
            }
            removeFreeSpot(it);
        }
    }

    void moveBatched(TextBufferBlock* to, const std::vector<UIRichText*>& texts) {
        const auto self = block.map(BufferAccessMode::WRITE_ONLY);
        const auto other = to->block.map(BufferAccessMode::WRITE_ONLY);

        for (auto& text : texts) {
            if (const auto it = hashes.find(text->hash()); it != hashes.end()) {
                FreeSpot src = it->second;
                FreeSpot dst = to->findFreeSpot(src.length);

                auto* srcPtr = self.get<CharInfo>() + src.index;
                auto* dstPtr = other.get<CharInfo>() + dst.index;

                memcpy(dstPtr, srcPtr, sizeof(CharInfo) * src.length);

                for (int i = 0; i < src.length; ++i) {
                    srcPtr->visible = false;
                    srcPtr++;
                }
                to->hashes.emplace(text->hash(), dst);
                hashes.erase(text->hash());
            }
        }
    }

    /**
     * @param visible whether to set the characters as visible(true) or invisible(false)
     * @param pred Sets all texts that match the predicate to visible
     */
    void setVisibleStateOnAllTexts(const bool visible, const std::function<bool(size_t)>& pred) {
        const auto mapped = block.map(BufferAccessMode::WRITE_ONLY);
        CharInfo* writePointer = mapped.get<CharInfo>();

        for (const auto &[hash, spot] : hashes) {
            const auto& [index, length] = spot;

            auto* ptr = writePointer + index;

            if (pred(hash) && ptr->visible != visible) {
                for (int i = 0; i < length; ++i) {
                    (ptr + i)->visible = visible;
                }
            }
        }
    }

    void sortByVisible() {
        const auto mapped = block.map(BufferAccessMode::READ_WRITE);
        CharInfo* writePointer = mapped.get<CharInfo>();

        std::vector<CharInfo> visible;
        std::vector<CharInfo> hidden;

        for (const auto &[index, length]: hashes | std::views::values) {
            if (const auto* ptr = writePointer + index; ptr->visible) {
                for (int i = 0; i < length; ++i) {
                    visible.push_back(ptr[i]);
                }
            } else {
                for (int i = 0; i < length; ++i) {
                    hidden.push_back(ptr[i]);
                }
            }
        }
        memcpy(writePointer, visible.data(), sizeof(CharInfo) * visible.size());
        memcpy(writePointer + visible.size(), hidden.data(), sizeof(CharInfo) * hidden.size());

        highestIndex = visible.size();
    }

    void printActiveText() {
        const auto mapped = block.map(BufferAccessMode::READ_ONLY);
        const CharInfo* writePointer = mapped.get<CharInfo>();

        std::cout << "ShaderStorageBuffer [CharInfo] Dump\n";
        for (auto &[index, length]: hashes | std::views::values) {
            const auto* tmp = writePointer + index;

            if (tmp->visible)
            for (int i = 0; i < length; ++i) {
                std::cout << static_cast<char>((tmp + i)->visible);
            }
            std::cout << std::endl;
        }
    }
};


class TextRenderingSystem final : public RenderPass::MainPass {
    static void renderBlock(UniformData& uniforms, Rasterizer& rasterizer, const TextBufferBlock& block) {
        Rasterizer::Scissors scissors = rasterizer.enableScissors(Scope::SCOPED);
        scissors.set(block.scissors);

        block.block.setToBindingPoint(1);
        uniforms.overwrite("xOffset", normalizeXOffset(block.xOffset));
        uniforms.overwrite("yOffset", normalizeYOffset(block.yOffset));
        uniforms.upload_all();
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, block.highestIndex);
    }

    ResourceSystem* res = nullptr;
    PlaceholderSystem* placeholderSystem = nullptr;

    VertexArrayObject VAO;
    VertexBufferObject VBO;

    TextBufferBlock globalTextAllocator;

    std::vector<TextBufferBlock*> textBlocks;

    struct ViewportParent {
        UIParent* parent;
        Entity entity;
    } defaultParent;

    Scene* scene = nullptr;
public:
    TextRenderingSystem(Scene& scene, const Entity& entity) : scene(&scene) {
        constexpr float vertex_data[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f
        };

        defaultParent.entity = entity;
        VAO.allocate();
        VBO.allocate(sizeof(vertex_data), BufferUsage::DYNAMIC);
        VBO.uploadData(vertex_data);
        VAO.addAttribute(2, 2, 0);
    }

    void onEnable(Scene& scene) {
        res = &scene.getResourceSystem();
        placeholderSystem = &scene.getPlaceholderSystem();

        globalTextAllocator = TextBufferBlock(placeholderSystem, res, 50'000);

        defaultParent.parent = scene.getEntityManager().getComponent<UIParent>(defaultParent.entity);

        scene.getEventSystem().subscribe<NewComponentSignal, UIRichText>(
            [this, &scene](const Entity& entity, void* component) {
                auto* text = static_cast<UIRichText*>(component);
                auto* transform = scene.getEntityManager().getComponent<UITransform>(entity);

                if (text->parent == nullentity) text->parent = defaultParent.entity;

                auto* parent = scene.getEntityManager().getComponent<UIParent>(text->parent);
                parent->children.emplace_back(entity, InteractionTarget::UI_RICH_TEXT);
                transform->world = UITransform::computeWorldTransform(parent->transform, transform->local);
                transform->world.scale = transform->local.scale;
                transform->world.rotation = transform->local.rotation;
                text->setHash(entity.id());
                globalTextAllocator.render(*text, *transform);
            }
        );

        scene.getEventSystem().subscribe<UITextUpdateSignal>([this, &scene](void* s) {
            const auto& entity = signal_cast<UITextUpdateSignal>(s).entity;
            auto& text         = scene.getEntityManager().getComponent<UIRichText&>(entity);
            const auto& parent = scene.getEntityManager().getComponent<UIParent&>(text.parent);
            auto& transform    = scene.getEntityManager().getComponent<UITransform&>(entity);

            transform.world = UITransform::computeWorldTransform(parent.transform, transform.local);
            transform.world.scale = transform.local.scale;
            transform.world.rotation = transform.local.rotation;

            for (const auto& block : textBlocks) {
                if (block->find(text.hash())) {
                    block->render(text, transform);
                    return;
                }
            }
            globalTextAllocator.render(text, transform);
        });
    }

    static float normalizeXOffset(const float x) {
        return -2.0f / WINDOW_WIDTH * -x;
    }

    static float normalizeYOffset(const float y) {
        return -2.0f / WINDOW_HEIGHT * y;
    }

    void onRender(RenderInfo& info) override {
        auto blending = info.rasterizer.enableBlending(Scope::SCOPED);
        blending.setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        auto& renderer = info.renderer;
        VAO.bind();
        renderer.setActiveShader("TextShader");

        UniformData Uniforms(renderer.getActiveShader()->id());
        Uniforms.write("text", 0);
        Uniforms.write("xOffset", 0.0f);
        Uniforms.write("yOffset", 0.0f);
        Uniforms.upload_all();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, res->getFont("fonts.jetbrainsmono.regular")->texture.id());
        renderBlock(Uniforms, info.rasterizer, globalTextAllocator);

        for (const auto& textBlock : textBlocks) {
            renderBlock(Uniforms, info.rasterizer, *textBlock);
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glEnable(GL_DEPTH_TEST);
    }

    TextBufferBlock* createBlock(const int chars) {
        textBlocks.emplace_back(new TextBufferBlock(placeholderSystem, res, chars * sizeof(CharInfo)));
        return textBlocks.back();
    }

    void remove(const Entity& textEntity, TextBufferBlock* hint = nullptr) {
        const auto* text = scene->getEntityManager().getComponent<UIRichText>(textEntity);

        if (hint) {
            hint->remove(*text);
            return;
        }
        globalTextAllocator.remove(*text);
    }

    void add(const Entity& textEntity, TextBufferBlock* at = nullptr) {
        auto* text = scene->getEntityManager().getComponent<UIRichText>(textEntity);
        auto* transform = scene->getEntityManager().getComponent<UITransform>(textEntity);

        if (at) {
            at->render(*text, *transform);
            return;
        }
        globalTextAllocator.render(*text, *transform);
    }

    TextBufferBlock& getGlobalAllocatorBlock() {
        return globalTextAllocator;
    }
};
