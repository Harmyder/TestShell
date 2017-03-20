#include "stdafx.h"

#include "Internal\BaseSimulation.h"
#include "Internal\BlankPhysicsData.h"
#include "Pipeline\SceneManager\SceneManager.h"
#include "Pipeline\UserLevel\UserScene.h"
#include "Pipeline\UserLevel\UserCollider.h"
#include "Pipeline\UserLevel\UserMesh.h"
#include "Pipeline\UserLevel\UserSceneFactory.h"
#include "Pipeline\InputLevel\InputMesh.h"
#include "Viewer\Viewport.h"
#include "Viewer\GameInput.h"

using namespace std;
using namespace Pipeline;
using namespace Viewer;
using namespace Common;

BaseSimulation::BaseSimulation(const char* name, Viewer::Viewport& viewport, const Viewer::GameInput& gameInput) :
    Common::NamedByCopyObject(name),
    viewport_(viewport),
    gameInput_(gameInput)
{}

BaseSimulation::~BaseSimulation() {}

void BaseSimulation::ImportScene(const string& path, const string& filetitle) {
    SceneManager &manager = SceneManager::GetInstance();
    manager.Load(path, filetitle);

    InputScene *inputScene = manager.GetScene();
    UserSceneFactory factory;
    scene_ = make_unique<UserScene>();
    factory.BuildScene(*scene_, *inputScene);
}

void BaseSimulation::InitBlankPhysicsData() {
    for (uint_t i = 0; i < scene_->GetMeshesCount(); ++i) {
        auto pd = make_unique<BlankPhysicsData>();
        pd->SetTransform(OrthogonalTransform(XMLoadFloat4x4(&scene_->GetMesh(i).GetInput().GetTransform())));
        scene_->GetMeshNonConst(i).InitPhysicsData(move(pd));
    }
}

void BaseSimulation::BeforeStep() {
}

void BaseSimulation::AfterStep() {
    if (gameInput_.IsFirstPressed(GameInput::Input::kSpaceKey)) {
        isOngoing_ = false;
    }
}
