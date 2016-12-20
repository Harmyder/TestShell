#pragma once

#include "Internal\BaseSimulation.h"

class FbxSimulation : public BaseSimulation
{
public:
    FbxSimulation();

public:
    void Init(Viewer::Viewport &viewport) override;
    void Step(float deltaTime) override;
    void Quit() override;
};
