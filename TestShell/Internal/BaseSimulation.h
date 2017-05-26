#pragma once

#include "Internal/ISimulation.h"

namespace Pipeline {
    class InputScene;
    class UserScene;
}

namespace Viewer {
    class Viewport;
    class GameInput;

    class RenderItemOpaqueRaii;
    class RenderItemTransparentRaii;
    class RenderItemWithInstancesRaii;
    class RenderItemParticlesRaii;
    class MaterialRaii;
}

class BaseSimulation : public ISimulation
{
public:
    BaseSimulation(const char* name, Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
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
    bool isOngoing_ = true;
};
