#include "stdafx.h"

#include "Internal/BaseSimulation.h"
#include "Pipeline/SceneManager/SceneManager.h"
#include "Pipeline/UserLevel/UserScene.h"
#include "Pipeline/UserLevel/UserCollider.h"
#include "Pipeline/UserLevel/UserMesh.h"
#include "Pipeline/UserLevel/UserSceneFactory.h"
#include "Viewer\Viewport.h"

using namespace std;
using namespace Pipeline;
using namespace Viewer;

BaseSimulation::BaseSimulation(const char* name) : Pile::NamedByCopyObject(name) {}

BaseSimulation::~BaseSimulation() {}

void BaseSimulation::Init(Viewport &viewport)
{
    viewport_ = &viewport;
    scene_ = make_unique<UserScene>(viewport);
}

void BaseSimulation::ImportScene(const string& path, const string& filetitle)
{
    SceneManager &manager = SceneManager::GetInstance();
    manager.Load(path, filetitle);

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

