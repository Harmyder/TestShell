#pragma once

#include "TestShell/Internal/ISimulation.h"
#include "Common/Attribute/NamedObject.h"

namespace Pipeline {
    class UserScene;
}

namespace Viewer {
    class Viewport;
    class GameInput;

    class RenderItemOpaqueRaii;
    class RenderItemTransparentRaii;
    class RenderItemWithInstancesRaii;
}

namespace Exploring_Bullet
{
    class BaseBulletSimulation : public ISimulation, Common::NamedByCopyObject
    {
    public:
        BaseBulletSimulation(const char* name, Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
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
        bool isOngoing_ = true;
    };
}
