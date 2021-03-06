#include "stdafx.h"

#include "Internal\SimulationManager.h"

#include "Internal\SimulationFactory.h"

#include "Simulations/HeatEquation2d.h"
#include "Simulations/TextureDemo.h"
#include "Simulations/FlockSimulation.h"
#include "Simulations/SphereBvSimulation.h"
#include "Simulations/FbxSimulation.h"
#include "Simulations/CubeSimulation.h"
#include "Simulations/MovingObjsInRi.h"
#include "Simulations/Cloth/ClothSimulation.h"
#include "Simulations/Particles/RandomWalk.h"

#include "Exploring_Bullet/Simulations/FallingCube.h"
#include "Exploring_Bullet/Simulations/SimpleCloth.h"

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
    RegisterFactory(new SimulationFactory<ClothSimulation>("Cloth Simulation"));
    RegisterFactory(new SimulationFactory<CubeSimulation>("The Cube"));
    RegisterFactory(new SimulationFactory<MovingObjsInRi>("Solar System"));
    RegisterFactory(new SimulationFactory<RandomWalk>("Diffusion with drift and reaction"));
    RegisterFactory(new SimulationFactory<Exploring_Bullet::FallingCube>("Bullet: Falling Cube"));
    RegisterFactory(new SimulationFactory<FbxSimulation>("Fbx Loading"));
    RegisterFactory(new SimulationFactory<HeatEquation2d>("Heat diffusion over plate"));
    RegisterFactory(new SimulationFactory<TextureDemo>("Texture Demo"));
    RegisterFactory(new SimulationFactory<FlockSimulation>("Flock"));
    RegisterFactory(new SimulationFactory<Exploring_Bullet::SimpleCloth>("Bullet: Simple Cloth"));
    RegisterFactory(new SimulationFactory<SphereBvSimulation>("Sphere Bounding Volume"));
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
