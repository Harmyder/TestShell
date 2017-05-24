#include "stdafx.h"
#include "Simulations/Particles/RandomWalk.h"

#include "Viewer/Viewport.h"
#include "Viewer/Raii.h"
#include "Common/Math/Vector/all.h"

using namespace Viewer;
using namespace std;
using namespace Common;

struct Particle {
    Vector3 position;
    float age;
};
vector<Particle> particles;

const Vector3 kEmitterPos(5.f, 5.f, 5.f);
const float kSpatialStep = .01f;
const float kTimeStep = 0.1f;
const float kLifeSpan = 10.f; // ugly! Think about exponential decay instead
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
    static uniform_int_distribution<int> d{ 1, 6 };
    return d(e);
}

RandomWalk::RandomWalk(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput) :
    BaseSimulation("RandomWalk", viewport, gameInput) {}
RandomWalk::~RandomWalk() {}

void RandomWalk::Init() {
    for (auto& p : particles) {
        p.age = 0.f;
        p.position = kEmitterPos;
    }
}

void RandomWalk::Step(float) {
    for (auto& p : particles) {
        p.position += kDirs[RollFairDie()];
        ++p.age;
    }
}
