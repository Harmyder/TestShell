#pragma once

#include "Internal\BaseSimulation.h"

class CubeSimulation : public BaseSimulation
{
public:
    CubeSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~CubeSimulation();

public:
    void Init() override;
    void Step(float deltaTime) override;

private:
    std::unique_ptr<Viewer::RenderItemOpaqueRaii> cube_;
    std::unique_ptr<Viewer::MaterialRaii> matCollider_;
};
