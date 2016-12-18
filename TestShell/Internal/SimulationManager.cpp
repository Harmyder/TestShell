#include "stdafx.h"

#include "Internal\SimulationManager.h"

#include "Internal\SimulationFactory.h"

#include "Simulations\FbxSimulation.h"

SimulationManager::~SimulationManager()
{
    assert(factories_ == NULL);
}

void SimulationManager::Init()
{
    RegisterSimulations();
}

void SimulationManager::Close()
{
    UnregisterSimulations();
}

void SimulationManager::RegisterSimulations()
{
    factories_ = new std::vector<BaseFactory*>;
    RegisterFactory(new SimulationFactory<FbxSimulation>("Fbx objects"));
}

void SimulationManager::UnregisterSimulations()
{
    for(uint32 i = 0; i < factories_->size(); ++i)
    {
        delete (*factories_)[i];
    }
    delete factories_;
    factories_ = NULL;
}

void SimulationManager::RegisterFactory(BaseFactory *factory)
{
    factories_->push_back(factory);
}

uint_t SimulationManager::GetFactoriesCount() const
{
    return factories_->size();
}

BaseFactory *SimulationManager::GetFactory(uint_t i) const
{
    return (*factories_)[i];
}
