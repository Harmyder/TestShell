#pragma once

#include "Internal\BaseSimulation.h"

class CubeSimulation : public BaseSimulation
{
public:
    CONS_DEC(CubeSimulation);
    ~CubeSimulation();

public:
    void Init() override;
    void Step(float deltaTime) override;

private:
    std::unique_ptr<Viewer::RenderItemOpaqueRaii> cube_;
    std::unique_ptr<Viewer::MaterialRaii> matCollider_;
};
