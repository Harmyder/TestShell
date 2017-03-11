#pragma once

#include "Common\Attribute\NamedObject.h"

namespace Pipeline {
    class UserScene;
}

namespace Viewer {
    class Viewport;
    class GameInput;
}

class BaseSimulation : Common::NamedByCopyObject
{
public:
    BaseSimulation(const char* name, Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~BaseSimulation();

public:
    virtual void Init() {}
    void ImportScene(const std::string& path, const std::string& filetitle);

    virtual void BeforeStep();
    virtual void Step      (float deltaTime) = 0;
    virtual void AfterStep();

    bool IsOngoing() { return isOngoing_; }
    virtual void Quit() = 0;

protected:
    std::unique_ptr<Pipeline::UserScene> scene_;
    Viewer::Viewport& viewport_;
    const Viewer::GameInput& gameInput_;
    bool isOngoing_ = true;
};
