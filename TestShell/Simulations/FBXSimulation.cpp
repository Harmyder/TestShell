#include "stdafx.h"

#include "Simulations/FbxSimulation.h"

FbxSimulation::FbxSimulation() :
    BaseSimulation("FbxSimulation")
{
}

void FbxSimulation::Init()
{
    BaseSimulation::Init();

    ImportScene("C:\\Yola\\Canoe\\FBX\\teapot001.fbx");
}

void FbxSimulation::Step(float deltaTime)
{
    deltaTime;
}

void FbxSimulation::Quit()
{
}