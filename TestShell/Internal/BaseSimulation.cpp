#include "stdafx.h"

#include "Internal\BaseSimulation.h"
#include "Pipeline\SceneManager\SceneManager.h"
#include "Pipeline\UserLevel\UserScene.h"
#include "Pipeline\UserLevel\UserCollider.h"
#include "Pipeline\UserLevel\UserMesh.h"
#include "Pipeline\UserLevel\UserSceneFactory.h"
#include "Viewer\Viewport.h"
#include "Viewer\GameInput.h"

using namespace std;
using namespace Pipeline;
using namespace Viewer;

BaseSimulation::BaseSimulation(const char* name, Viewer::Viewport& viewport, const Viewer::GameInput& gameInput) :
    Pile::NamedByCopyObject(name),
    viewport_(viewport),
    gameInput_(gameInput)
{
    scene_ = make_unique<UserScene>(viewport);
}

BaseSimulation::~BaseSimulation() {}

void BaseSimulation::ImportScene(const string& path, const string& filetitle) {
    SceneManager &manager = SceneManager::GetInstance();
    manager.Load(path, filetitle);

    InputScene *inputScene = manager.GetScene();
    UserSceneFactory factory;
    factory.Build(*scene_, *inputScene);
}

void BaseSimulation::BeforeStep() {
}

void BaseSimulation::AfterStep() {
    if (gameInput_.IsFirstPressed(GameInput::Input::kSpaceKey)) {
        isOngoing_ = false;
    }
}
