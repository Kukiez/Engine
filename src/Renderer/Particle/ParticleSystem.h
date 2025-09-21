#pragma once
#include <Icosphere.h>
#include <openGL/BufferObjects/IndirectDrawBuffer.h>
#include <openGL/BufferObjects/ShaderStorageBuffer.h>

#include "../GeometrySystem.h"
#include <Core/Model/Model.h>
#include <Model/SamplerCurve.h>
#include <openGL/shaders/UniformData.h>
#include <Renderer/RendererOps.h>
#include <Util/Random.h>

#include "../Renderers/MaterialSystem.h"

struct GPUParticle {
    glm::vec3 position;
    int age;
    glm::vec3 color;
    float alpha;
    glm::vec3 rotation;
    float scale;
    glm::vec3 velocity;
    int lifetime;
};

struct RenderInfo;

class ParticleSystem_ {
    ShaderStorageBuffer buffer[2];
    ShaderStorageBuffer aliveCount[2];
    IndirectDrawBuffer indirectDraw;
    IndirectDispatchBuffer indirectDispatch;
    ShaderStorageBuffer newParticles;
    size_t maxParticles = 0;

    static constexpr unsigned ZERO = 0;

    constexpr static auto PARTICLE_IN_INDEX = 1;
    constexpr static auto PARTICLE_OUT_INDEX = 2;

    bool InOutSSBO = 0;

    std::vector<GPUParticle> newParticleBuffer;
public:
    ParticleSystem_() = default;

    ParticleSystem_(const size_t max);

    void render(Viewable<ForwardRenderStage> auto view, const Geometry& geom) {
        auto now = std::chrono::high_resolution_clock::now();

        buffer[InOutSSBO].setToBindingPoint(PARTICLE_IN_INDEX);
        buffer[!InOutSSBO].setToBindingPoint(PARTICLE_OUT_INDEX);
        indirectDraw.setToBindingPoint(3);
        aliveCount[0].setToBindingPoint(4);
        indirectDispatch.setToBindingPoint(5);
        // 6 reserved

        ComputeShader *shader = &view.setActiveComputeShader("ParticleClearShader");
        UniformData uniforms(shader->id());
        uniforms.upload_nowrite("u_count", static_cast<int>(geom.indices.size()));
        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        shader = &view.setActiveComputeShader("ParticleEvalShader");
        uniforms.setProgram(shader->id());
        indirectDispatch.bind();
        glDispatchComputeIndirect(0);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        if (!newParticleBuffer.empty()) {
            newParticles.overwrite(newParticleBuffer.size() * sizeof(GPUParticle), BufferUsage::STREAM, newParticleBuffer.data());
            newParticles.setToBindingPoint(6);

            shader = &view.setActiveComputeShader("ParticleBufferAppendShader");
            uniforms.setProgram(shader->id());
            uniforms.upload_nowrite("particleCount", (int)newParticleBuffer.size());
            uniforms.upload_nowrite("maxParticles", (int)maxParticles);
            shader->dispatch(newParticleBuffer.size());
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


            shader = &view.setActiveComputeShader("ParticleBufferAppendCountsShader");
            uniforms.setProgram(shader->id());
            uniforms.upload_nowrite("particleCount", (int)newParticleBuffer.size());
            glDispatchCompute(1, 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


            newParticleBuffer.clear();
        }

        shader = &view.setActiveComputeShader("ParticleScaleShader");
        uniforms.setProgram(shader->id());
        uniforms.upload_nowrite("SCALE_TYPE", 1);
        uniforms.upload_nowrite("minScale", 1.f);
        uniforms.upload_nowrite("maxScale", 0.0f);
        glDispatchComputeIndirect(0);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);


        glActiveTexture(GL_TEXTURE0);
        SamplerCurve3 color;
        color.add(glm::vec3(0), 0);
        color.add(glm::vec3(1), 1);
        auto tex = color.toTexture(256);
        glBindTexture(GL_TEXTURE_1D, tex.id());

        shader = &view.setActiveComputeShader("ParticleColorShader");
        uniforms.setProgram(shader->id());
        uniforms.upload_nowrite("COLOR_TYPE", 1);
        uniforms.upload_nowrite("colorSampler", 0);
        glDispatchComputeIndirect(0);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);


        shader = &view.setActiveComputeShader("ParticleForceShader");
        uniforms.setProgram(shader->id());
        uniforms.upload_nowrite("FORCE_TYPE", 1);
        uniforms.upload_nowrite("minVelocity", glm::vec3(-10));
        uniforms.upload_nowrite("maxVelocity", glm::vec3(10));
        uniforms.upload_nowrite("axis", glm::vec3(0, 0, -1));
        uniforms.upload_nowrite("center", glm::vec3(0, 0, 0));
        uniforms.upload_nowrite("strength", 0.2f);
        uniforms.upload_nowrite("falloff", 0.066f);
        uniforms.upload_nowrite("u_pullStrength", -0.1f);
        glDispatchComputeIndirect(0);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);


        InOutSSBO = !InOutSSBO;
        view.setActiveShader("ParticleShader");
        buffer[InOutSSBO].setToBindingPoint(PARTICLE_IN_INDEX);
        geom.VAO.bind();
        indirectDraw.bind();
        glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0);

        auto* particle = new GPUParticle[16];

        for (int i = 0; i < 16; i++) {
            particle[i].position = glm::vec3(Random::Float(-50.0f, 50.0f), Random::Float(-15.0f, 15.0f), Random::Float(-50.0f, 50.0f));
            particle[i].age = 0;
            particle[i].color = glm::vec3(Random::Float(0.0f, 1.0f), Random::Float(0.0f, 1.0f), Random::Float(0.0f, 1.0f));
            particle[i].alpha = 1.0f;
            particle[i].rotation = glm::vec3(0.0f);
            particle[i].scale = Random::Float(0.0f, 1.0f);
            particle[i].velocity = glm::vec3(Random::Float(0.000f, 0.01f), Random::Float(0.000f, 0.01f), Random::Float(0.000f, 0.01f));
            particle[i].lifetime = Random::Int(100, 800);
        }
        addParticles(MemorySpan<GPUParticle>(particle, particle + 16));
        delete[] particle;
    }

    void addParticles(MemorySpan<GPUParticle> particles);
};