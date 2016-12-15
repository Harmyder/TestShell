#pragma once

#include "Pile\Attribute\NamedObject.h"

namespace Pipeline {
    class UserScene;
}

class BaseSimulation : Pile::NamedObject
{
public:
    BaseSimulation(const char* name);
    ~BaseSimulation();

public:
    virtual void Init();
    void ImportScene(const char *filename);

    virtual void BeforeStep () ;
    virtual void Step       (float deltaTime) = 0;
    virtual void AfterStep   ();

    virtual void Quit() = 0;

protected:
    std::unique_ptr<Pipeline::UserScene> scene_;
};
