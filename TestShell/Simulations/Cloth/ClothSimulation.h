#pragma once

#include "Internal\BaseSimulation.h"

namespace Pipeline
{
    class InputScene;
}
namespace Viewer
{
    struct StructRenderItemId;
    class MaterialRaii;
}

class ClothSimulation : public BaseSimulation
{
public:
    ClothSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~ClothSimulation();

public:
    void Init() override;
    void Step(float deltaTime) override;
    void Quit() override;

private:
    std::unique_ptr<Viewer::StructRenderItemId> cloth_;
    std::unique_ptr<Viewer::MaterialRaii> matDeformable_;

    std::unique_ptr<Pipeline::InputScene> inputScene_;
};
