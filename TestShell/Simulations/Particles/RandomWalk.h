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
};