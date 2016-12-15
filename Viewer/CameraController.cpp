#include "stdafx.h"

#include "CameraController.h"

#include "Camera.h"
#include "GameInput.h"

using namespace std;
using namespace DirectX;

namespace Viewer
{
    CameraController::CameraController(Camera& camera, const GameInput& gameInput) : camera_(camera), gameInput_(gameInput) {}

    void CameraController::Update(float dT) {
        dT;
        camera_.SetTransform(XMMatrixIdentity());
    }
}
