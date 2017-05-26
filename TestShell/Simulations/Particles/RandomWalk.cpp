#include "stdafx.h"
#include "Simulations/Particles/RandomWalk.h"

#include "Viewer/Viewport.h"
#include "Viewer/Raii.h"
#include "Common/Math/Vector/all.h"

using namespace Viewer;
using namespace std;
using namespace Common;

const string name = "Diffusion";
const Vector3 kEmitterPos(kZero);
struct Particle {
    Vector3 position;
    float age;

    void Init() { age = 0.f; position = kEmitterPos; }
};
constexpr int kParticlesCount = 5000;
array<Particle, kParticlesCount> particles;

const float kSpatialStep = .01f;
const float kTimeStep = 0.1f;
const float kLifeSpan = 50.f; // ugly! Think about exponential decay instead
const Vector3 kDirs[6] = {
    Vector3(kSpatialStep, 0.f, 0.f),
    Vector3(-kSpatialStep, 0.f, 0.f),
    Vector3(0.f, kSpatialStep, 0.f),
    Vector3(0.f, -kSpatialStep, 0.f),
    Vector3(0.f, 0.f, kSpatialStep),
    Vector3(0.f, 0.f, -kSpatialStep)
};

int RollFairDie() {
    static std::random_device rd{};
    static default_random_engine e{ rd() };
    static uniform_int_distribution<int> d{ 0, 5 };
    return d(e);
}

RandomWalk::RandomWalk(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput) :
    BaseSimulation("RandomWalk", viewport, gameInput) {}
RandomWalk::~RandomWalk() {}

void RandomWalk::Init() {
    for (auto& p : particles) p.Init();
    matParticle_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kWhite(), "rigid"));

    RenderItemParticlesDesc desc(name,
        (uint8*)particles.data(), (uint32)particles.size(),
        Matrix4(kIdentity).Store4x3(), *matParticle_, Texture(), 0.01f);
    particles_ = make_unique<RenderItemParticlesRaii>(viewport_.CreateRenderItemParticles({ desc }, sizeof(VertexParticles)));
}

void RandomWalk::Step(float dT) {
    vector<XMFLOAT3> graphicsParticles;
    graphicsParticles.reserve(particles.size());
    for (auto& p : particles) {
        p.age += dT;
        if (p.age > kLifeSpan) p.Init();
        else p.position += kDirs[RollFairDie()];
        graphicsParticles.push_back(p.position.Store());
    }

    viewport_.UpdateRenderSubItemVertexData(*particles_, name, (uint8*)graphicsParticles.data());
}
