#pragma once

#include "Internal/ISimulation.h"

namespace Pipeline {
    class InputScene;
    class UserScene;
}

namespace Viewer {
    class Viewport;
    class GameInput;
    class CameraController;

    class RenderItemOpaqueRaii;
    class RenderItemTransparentRaii;
    class RenderItemWithInstancesRaii;
    class RenderItemParticlesRaii;
    class MaterialRaii;
    class TextureRaii;
}

#define CONS_DEC(C) C(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput, Viewer::CameraController& cameraCtrl)
#define CONS_DEF(C) C::C(Viewport& viewport, const GameInput& gameInput, CameraController& cameraCtrl) : BaseSimulation(viewport, gameInput, cameraCtrl) {}

class BaseSimulation : public ISimulation
{
public:
    CONS_DEC(BaseSimulation);
    ~BaseSimulation();

public:
    void ImportScene(const std::string& path, const std::string& filetitle);
    void InitBlankPhysicsData();

    void BeforeStep() override;
    void AfterStep() override;

    bool IsOngoing() override { return isOngoing_; }

protected:
    std::unique_ptr<Pipeline::UserScene> scene_;
    Viewer::Viewport& viewport_;
    const Viewer::GameInput& gameInput_;
    Viewer::CameraController& cameraCtrl_;
    bool isOngoing_ = true;
};
