#pragma once
#include "Common/Attribute/NamedObject.h"

class ISimulation : public Common::NamedByCopyObject
{
public:
    using Common::NamedByCopyObject::NamedByCopyObject;

    virtual void Init() = 0;

    virtual void BeforeStep() = 0;
    virtual void Step(float deltaTime) = 0;
    virtual void AfterStep() = 0;

    virtual bool IsOngoing() = 0;

    virtual ~ISimulation() {}
};
