#include "stdafx.h"

#include "Internal/BaseSimulation.h"
#include "Pipeline/SceneManager/SceneManager.h"
#include "Pipeline/UserLevel/Scene/UserScene.h"
#include "Pipeline/UserLevel/Collider/UserCollider.h"
#include "Pipeline/UserLevel/Mesh/UserMesh.h"
#include "Pipeline/UserLevel/Factory/UserSceneFactory.h"

using namespace std;
using namespace Pipeline;

BaseSimulation::BaseSimulation(const char* name) : Pile::NamedObject(name) {}

BaseSimulation::~BaseSimulation() {}

void BaseSimulation::Init()
{
    scene_ = make_unique<UserScene>();
}

void BaseSimulation::ImportScene(const char *filename)
{
    SceneManager &manager = SceneManager::GetInstance();
    manager.Load(filename);

    InputScene *inputScene = manager.GetScene();
    UserSceneFactory factory;
    factory.Build(*scene_, *inputScene);
}

void BaseSimulation::BeforeStep()
{
}

void BaseSimulation::AfterStep()
{
}

