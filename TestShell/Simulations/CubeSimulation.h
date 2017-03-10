#pragma once

#include "Internal\BaseSimulation.h"

namespace Viewer
{
    struct StructRenderItemId;
}

class CubeSimulation : public BaseSimulation
{
public:
    CubeSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~CubeSimulation();

public:
    void Init() override;
    void Step(float deltaTime) override;
    void Quit() override;

private:
    std::unique_ptr<Viewer::StructRenderItemId> cube_;
};
