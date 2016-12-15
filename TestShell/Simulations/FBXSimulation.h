#pragma once

#include "Internal\BaseSimulation.h"

class FbxSimulation : public BaseSimulation
{
public:
    FbxSimulation();

public:
    void Init() override;
    void Step(float deltaTime) override;
    void Quit() override;
};
