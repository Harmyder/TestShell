#pragma once

#include "Pile\Attribute\NamedObject.h"

namespace Pipeline {
    class UserScene;
}

namespace Viewer {
    class Viewport;
}

class BaseSimulation : Pile::NamedObject
{
public:
    BaseSimulation(const char* name);
    ~BaseSimulation();

public:
    virtual void Init(Viewer::Viewport& viewport);
    void ImportScene(const char *filename);

    virtual void BeforeStep() ;
    virtual void Step      (float deltaTime) = 0;
    virtual void AfterStep ();

    virtual void Quit() = 0;

protected:
    std::unique_ptr<Pipeline::UserScene> scene_;
    Viewer::Viewport *viewport_;
};
