#pragma once
#include "Internal\BaseSimulation.h"

class FlockSimulation : public BaseSimulation
{
public:
    FlockSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~FlockSimulation();

    void Init() override;
    void Step(float deltaTime) override;

private:
    std::unique_ptr<Viewer::RenderItemWithInstancesRaii> flock_;
    std::array<std::unique_ptr<Viewer::MaterialRaii>, 3> mats_;
};