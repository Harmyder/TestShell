#pragma once
#include "Internal\BaseSimulation.h"

class RandomWalk : public BaseSimulation
{
public:
    RandomWalk(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~RandomWalk();

public:
    void Init() override;
    void Step(float deltaTime) override;

private:
    std::unique_ptr<Viewer::MaterialRaii> matParticle_;
    std::unique_ptr<Viewer::RenderItemParticlesRaii> particles_;
    std::unique_ptr<Viewer::TextureRaii> texParticle_;
    float timeLeftout_ = 0.f;
};