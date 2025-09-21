#include "ParticleSystem.h"

#include <openGL/shaders/UniformData.h>
#include <Util/Random.h>
#include <Core/Model/SamplerCurve.h>
#include "../RenderingDetails.h"
#include "../Renderers/RenderInfo.h"
#include "Renderer/Renderers/Renderer.h"

ParticleSystem_::ParticleSystem_(const size_t max) : maxParticles(16000){
    auto* particle = new GPUParticle[16000];

    for (int i = 0; i < maxParticles; i++) {
        particle[i].position = glm::vec3(Random::Float(-50.0f, 50.0f), 0.0f, Random::Float(-50.0f, 50.0f));
        particle[i].age = 0;
        particle[i].color = glm::vec3(Random::Float(0.0f, 1.0f), Random::Float(0.0f, 1.0f), Random::Float(0.0f, 1.0f));
        particle[i].alpha = 1.0f;
        particle[i].rotation = glm::vec3(0.0f);
        particle[i].scale = Random::Float(0.0f, 1.0f);
        particle[i].velocity = glm::vec3(Random::Float(0.000f, 0.01f), Random::Float(0.000f, 0.01f), Random::Float(0.000f, 0.01f));
        particle[i].lifetime = Random::Int(100, 500);
    }
    buffer[0].allocate(sizeof(GPUParticle) * maxParticles, BufferUsage::STATIC, particle);
    buffer[1].allocate(sizeof(GPUParticle) * maxParticles, BufferUsage::STATIC, particle);

    DrawElementsIndirectCommand cmd = {};
    cmd.instanceCount = 160'000;

    indirectDraw.allocate(1, sizeof(DrawElementsIndirectCommand), &cmd);
    indirectDispatch.allocate(1, sizeof(DispatchIndirectCommand));

    aliveCount[0].allocate(sizeof(unsigned), BufferUsage::STATIC, &ZERO);
    aliveCount[1].allocate(sizeof(unsigned), BufferUsage::STATIC, &ZERO);

    newParticles.allocate(sizeof(int), BufferUsage::STATIC);
}

/**
 * Index [0] -> Default UBO
 * Index [1] -> In Particle Buffer
 * Index [2] -> Out Particle Buffer
 * Index [3] -> Indirect Draw Buffer (instanceCount: particles that are alive)
 * Index [4] -> Alive Count (Amount of Particles that were alive last frame)
 * Index [5] -> Indirect Dispatch (Dispatch Shader holding amount of workgroups)
 * Index [6] -> New Particles Buffer
 */

void ParticleSystem_::addParticles(MemorySpan<GPUParticle> particles)
{
    newParticleBuffer.insert(newParticleBuffer.end(), particles.begin(), particles.end());
}
