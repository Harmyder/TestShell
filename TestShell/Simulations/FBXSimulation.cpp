#include "stdafx.h"

#include "Simulations/FbxSimulation.h"
#include "Viewer\Viewport.h"

using namespace Viewer;

FbxSimulation::FbxSimulation() :
    BaseSimulation("FbxSimulation")
{
}

void FbxSimulation::Init(Viewport &viewport)
{
    BaseSimulation::Init(viewport);

    viewport.CreateMaterial(Viewport::Material::kEmerald, "rigid");
    viewport.CreateMaterial(Viewport::Material::kSilver, "collider");

//    ImportScene("C:\\Yola\\TestShell\\FBX\\", "Box");
    ImportScene("C:\\Yola\\TestShell\\FBX\\", "teapot001");
}

void FbxSimulation::Step(float deltaTime)
{
    deltaTime;
}

void FbxSimulation::Quit()
{
}