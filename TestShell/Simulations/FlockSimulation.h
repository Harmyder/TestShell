#pragma once
#include "Internal\BaseSimulation.h"

namespace Viewer
{
    struct StructRenderItemWithInstancesId;
}

class FlockSimulation : public BaseSimulation
{
public:
    FlockSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~FlockSimulation();

    void Init() override;
    void Step(float deltaTime) override;
    void Quit() override;

private:
    std::unique_ptr<Viewer::StructRenderItemWithInstancesId> flock_;
};