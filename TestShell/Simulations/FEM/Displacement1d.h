#pragma once
#include "Internal\BaseSimulation.h"

class Displacement1d : public BaseSimulation
{
public:
    Displacement1d(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~Displacement1d();

public:
    void Init() override;
    void Step(float deltaTime) override;

private:
    std::unique_ptr<Viewer::RenderItemOpaqueRaii> rod_;
    std::unique_ptr<Viewer::MaterialRaii> mat_;

    std::function<float(float)> force_;
};


