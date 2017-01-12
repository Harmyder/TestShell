#pragma once

#include "Internal\BaseSimulation.h"

namespace Viewer
{
    struct StructRenderItemId;
}

class SphereBvSimulation : public BaseSimulation
{
public:
    SphereBvSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~SphereBvSimulation();

    void Init() override;
    void Step(float deltaTime) override;
    void Quit() override;

private:
    std::unique_ptr<Viewer::StructRenderItemId> sceneDescsVertices_;
    std::unique_ptr<Viewer::StructRenderItemId> sceneDescsTypes_;
    std::unique_ptr<Viewer::StructRenderItemId> boundingVolumeDesc_;
};

