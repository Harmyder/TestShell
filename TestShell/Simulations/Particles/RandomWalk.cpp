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
const Vector3 kDeadPos(kInfinity);
struct Particle {
    Vector3 position;
    float age;

    void Init() { age = 0.f; position = kEmitterPos; }
    void Die() { age = -1.f; position = kDeadPos; }
    bool IsAlive() { return age >= 0.f; }
};
constexpr int kParticlesCount = 10000;
constexpr int kEmitPerSecond = 500;
array<Particle, kParticlesCount> particles;
array<uint16, kParticlesCount> indicesDead; int deadCount;

const float kParticleSize = .01f;
const float kSpatialStep = .001f;
const float kTimeStep = .005f;
const float kExtinctRate = .01f; // per second
const Vector3 kDirs[6] = {
    Vector3(kSpatialStep, 0.f, 0.f),
    Vector3(-kSpatialStep, 0.f, 0.f),
    Vector3(0.f, kSpatialStep, 0.f),
    Vector3(0.f, -kSpatialStep, 0.f),
    Vector3(0.f, 0.f, kSpatialStep),
    Vector3(0.f, 0.f, -kSpatialStep)
};

int Rand(int lower, int upper) {
    static std::random_device rd{};
    static default_random_engine e{ rd() };
    uniform_int_distribution<int> d{ lower, upper };
    return d(e);
}

int RollBiasedDie(const array<int, 6>& odds) {
    const int r = Rand(0, accumulate(cbegin(odds), cend(odds), 0) - 1);
    int i = 0;
    int curr = odds[i];
    for (i = 1; r >= curr; ++i) curr += odds[i];
    return i - 1;
}

CONS_DEF(RandomWalk);
RandomWalk::~RandomWalk() {}

void RandomWalk::Init() {
    for (auto& p : particles) p.Die();
    iota(begin(indicesDead), end(indicesDead), (uint16)0);
    deadCount = kParticlesCount;
    matParticle_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kWhite(), "rigid"));

    const uint8 alpha = 25;
    const uint32 texData = (alpha << 24) + (153 << 16) + (153 << 8) + 0;
    texParticle_ = make_unique<TextureRaii>(viewport_.CreateTextureFromHandmadeData(L"particle", 1, 1, ResourceFormat::kR8G8B8A8_UNORM(), &texData, false));

    RenderItemParticlesDesc desc(name,
        (uint8*)particles.data(), (uint32)particles.size(),
        Matrix4(kIdentity).Store4x3(), *matParticle_, *texParticle_, kParticleSize);
    particles_ = make_unique<RenderItemParticlesRaii>(viewport_.CreateRenderItemParticles({ desc }, sizeof(VertexParticles)));
}

void RandomWalk::Step(float dT) {
    vector<XMFLOAT3> graphicsParticles;
    graphicsParticles.reserve(particles.size());
    timeLeftout_ += dT;
    const int emit = min((int)(dT * kEmitPerSecond), deadCount);
    for (int i = 0; i < emit; ++i) {
        const int index = indicesDead[deadCount - i - 1];
        particles[index].Init();
    }
    deadCount -= emit;

    while (timeLeftout_ >= kTimeStep) {
        for (int i = 0; i < (int)particles.size(); ++i) {
            auto& p = particles[i];
            if (p.IsAlive()) {
                if (Rand(0, 10000) < (int)(kExtinctRate * dT * 10001)) {
                    p.Die();
                    indicesDead[deadCount++] = (uint16)i;
                }
                else {
                    p.age += dT;
                    p.position += kDirs[RollBiasedDie({ 3,1,1,1,1,1 })];
                }
            }
        }
        timeLeftout_ -= kTimeStep;
    }
    for (auto& p : particles) {
        graphicsParticles.push_back(p.position.Store());
    }

    viewport_.UpdateRenderSubItemVertexData(*particles_, name, (uint8*)graphicsParticles.data());
}
