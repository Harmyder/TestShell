#pragma once
#include "Internal\BaseSimulation.h"

class Displacement1d : public BaseSimulation
{
public:
    CONS_DEC(Displacement1d);
    ~Displacement1d();

public:
    void Init() override;
    void Step(float deltaTime) override;

private:
    std::unique_ptr<Viewer::RenderItemOpaqueRaii> rod_;
    std::unique_ptr<Viewer::MaterialRaii> mat_;

    std::function<float(float)> force_;
};


