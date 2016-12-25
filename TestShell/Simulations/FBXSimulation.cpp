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

//    ImportScene("C:\\Yola\\TestShell\\FBX\\Box.fbx");
    ImportScene("C:\\Yola\\TestShell\\FBX\\teapot001.fbx");
}

void FbxSimulation::Step(float deltaTime)
{
    deltaTime;
}

void FbxSimulation::Quit()
{
}