#pragma once

#include "Internal\BaseSimulation.h"

class SphereBvSimulation : public BaseSimulation
{
public:
    SphereBvSimulation() : BaseSimulation("SphereBvSimulation") {}

    void Init(Viewer::Viewport &viewport) override;
    void Step(float deltaTime) override;
    void Quit() override;
};

