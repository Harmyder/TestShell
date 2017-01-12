#pragma once

#include "Internal\BaseSimulation.h"

namespace Viewer
{
    struct StructRenderItemId;
}

class FbxSimulation : public BaseSimulation
{
public:
    FbxSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~FbxSimulation();

public:
    void Init() override;
    void Step(float deltaTime) override;
    void Quit() override;

private:
    std::unique_ptr<Viewer::StructRenderItemId> sceneDescsVertices_;
    std::unique_ptr<Viewer::StructRenderItemId> sceneDescsTypes_;
};
