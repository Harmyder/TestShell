#pragma once

#include "Internal\BaseSimulation.h"

namespace Pipeline
{
    class InputScene;
}

class ClothSimulation : public BaseSimulation
{
public:
    ClothSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~ClothSimulation();

public:
    void Init() override;
    void Step(float deltaTime) override;

private:
    std::unique_ptr<Viewer::RenderItemOpaqueRaii> cloth_;
    std::unique_ptr<Viewer::MaterialRaii> matDeformable_;

    std::unique_ptr<Pipeline::InputScene> inputScene_;
};
