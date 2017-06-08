#include "stdafx.h"
#include "BaseBulletSimulation.h"

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

namespace Exploring_Bullet
{
    BaseBulletSimulation::BaseBulletSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput, Viewer::CameraController& cameraCtrl) :
        viewport_(viewport),
        gameInput_(gameInput),
        cameraCtrl_(cameraCtrl)
    {}

    BaseBulletSimulation::~BaseBulletSimulation() {}

    void BaseBulletSimulation::ImportScene(const string& path, const string& filetitle) {
        SceneManager &manager = SceneManager::GetInstance();
        manager.Load(path, filetitle);

        InputScene *inputScene = manager.GetScene();
        UserSceneFactory factory;
        scene_ = make_unique<UserScene>();
        factory.BuildScene(*scene_, *inputScene);
    }

    void BaseBulletSimulation::BeforeStep() {
    }

    void BaseBulletSimulation::AfterStep() {
        if (gameInput_.IsFirstPressed(GameInput::Input::kSpaceKey)) {
            isOngoing_ = false;
        }
    }
}
