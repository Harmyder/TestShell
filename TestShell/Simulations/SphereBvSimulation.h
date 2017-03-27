#pragma once

#include "Internal\BaseSimulation.h"

class SphereBvSimulation : public BaseSimulation
{
public:
    SphereBvSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~SphereBvSimulation();

    void Init() override;
    void Step(float deltaTime) override;

private:
    std::unique_ptr<Viewer::RenderItemOpaqueRaii> sceneDescsVertices_;
    std::unique_ptr<Viewer::RenderItemOpaqueRaii> sceneDescsTypes_;
    std::unique_ptr<Viewer::RenderItemTransparentRaii> boundingVolumeDesc_;

    std::unique_ptr<Viewer::MaterialRaii> matRigid_;
    std::unique_ptr<Viewer::MaterialRaii> matCollider_;
    std::unique_ptr<Viewer::MaterialRaii> matBv_;
};

