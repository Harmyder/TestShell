#include "stdafx.h"

#include "Simulations/FbxSimulation.h"
#include "Viewer\Viewport.h"

using namespace Viewer;
using namespace std;

FbxSimulation::FbxSimulation() :
    BaseSimulation("FbxSimulation")
{
}

void FbxSimulation::Init(Viewport &viewport)
{
    BaseSimulation::Init(viewport);

    viewport.CreateMaterial(Viewport::Material::kEmerald, "rigid");
    viewport.CreateMaterial(Viewport::Material::kSilver, "collider");

    const string path = "..\\..\\FBX\\";
    const string filetitle = "Box";
    ImportScene(path, filetitle);
}

void FbxSimulation::Step(float deltaTime)
{
    deltaTime;
}

void FbxSimulation::Quit()
{
}