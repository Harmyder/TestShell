#pragma once

#include "Pile\Attribute\NamedObject.h"

namespace Pipeline {
    class UserScene;
}

namespace Viewer {
    class Viewport;
}

class BaseSimulation : Pile::NamedByCopyObject
{
public:
    BaseSimulation(const char* name);
    ~BaseSimulation();

public:
    virtual void Init(Viewer::Viewport& viewport);
    void ImportScene(const std::string& path, const std::string& filetitle);

    virtual void BeforeStep() ;
    virtual void Step      (float deltaTime) = 0;
    virtual void AfterStep ();

    virtual void Quit() = 0;

protected:
    std::unique_ptr<Pipeline::UserScene> scene_;
    Viewer::Viewport *viewport_;
};
