#ifndef __SIMULATION_FACTORY__
#define __SIMULATION_FACTORY__

#include "Common/Attribute/NamedObject.h"

namespace Viewer
{
    class Viewport;
    class GameInput;
}

class ISimulation;

class BaseFactory : public Common::NamedByCopyObject
{
public:
    BaseFactory(const char *name) : NamedByCopyObject(name) {}

    virtual std::unique_ptr<ISimulation> Create(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput) = 0;
    virtual ~BaseFactory() {}
};

template <typename T>
class SimulationFactory : public BaseFactory
{
public:
    SimulationFactory(const char *name) : BaseFactory(name) {}

    std::unique_ptr<ISimulation> Create(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput) override {
        return std::make_unique<T>(viewport, gameInput);
    }
};

#endif // __SIMULATION_FACTORY__
