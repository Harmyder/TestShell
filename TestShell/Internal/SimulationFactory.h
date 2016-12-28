#ifndef __SIMULATION_FACTORY__
#define __SIMULATION_FACTORY__

#include "Pile/Attribute/NamedObject.h"

class BaseSimulation;

class BaseFactory : public Pile::NamedByCopyObject
{
public:
    BaseFactory(const char *name) : NamedByCopyObject(name) {}

    virtual std::unique_ptr<BaseSimulation> Create() = 0;
    virtual ~BaseFactory() {}
};

template <typename T>
class SimulationFactory : public BaseFactory
{
public:
    SimulationFactory(const char *name) : BaseFactory(name) {}

    virtual std::unique_ptr<BaseSimulation> Create() {
        return std::make_unique<T>();
    }
};

#endif // __SIMULATION_FACTORY__
