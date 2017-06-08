#pragma once

#include "TestShell/Internal/ISimulation.h"
#include "Common/Attribute/NamedObject.h"

namespace Pipeline {
    class UserScene;
    class InputScene;
    class UserMesh;
}

namespace Viewer {
    class Viewport;
    class GameInput;
    class CameraController;

    struct StructRenderItemId;
    struct StructRenderItemWithInstancesId;

    class MaterialRaii;
    class RenderItemOpaqueRaii;
    class RenderItemTransparentRaii;
    class RenderItemWithInstancesRaii;
}

namespace Exploring_Bullet
{
#define BCONS_DEC(C) C(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput, Viewer::CameraController& cameraCtrl)
#define BCONS_DEF(C) C::C(Viewport& viewport, const GameInput& gameInput, CameraController& cameraCtrl) : BaseBulletSimulation(viewport, gameInput, cameraCtrl) {}
    
    class BaseBulletSimulation : public ISimulation
    {
    public:
        BCONS_DEC(BaseBulletSimulation);
        ~BaseBulletSimulation();

    public:
        void ImportScene(const std::string& path, const std::string& filetitle);

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
}
