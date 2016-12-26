#include "stdafx.h"

#include "CameraController.h"

#include "GameInput.h"
#include "Graphics\Interface\GraphicsInterface.h"

using namespace std;
using namespace DirectX;

namespace Viewer
{
    CameraController::CameraController(const GameInput& gameInput, UpVector upVector) : gameInput_(gameInput) {
        if (upVector == UpVector::Y) worldUp_ = g_XMIdentityR1;
        else if (upVector == UpVector::Z) worldUp_ = g_XMIdentityR2;
        worldNorth_ = XMVector3Normalize(XMVector3Cross(worldUp_, g_XMIdentityR0));
        worldEast_ = XMVector3Normalize(XMVector3Cross(worldNorth_, worldUp_));
    }

    void CameraController::Update(float dT) {
        if (gameInput_.IsPressed(GameInput::Input::kMenuKey) &&
            gameInput_.IsPressed(GameInput::Input::kMouseLbutton))
        {
            float yaw = gameInput_.GetMousePressedDeltaX() * kMouseSensitivityX;
            float pitch = gameInput_.GetMousePressedDeltaY() * kMouseSensitivityY;
            currentYaw_ += yaw - lastYaw_;
            currentPitch_ += pitch - lastPitch_;
            lastYaw_ = yaw;
            lastPitch_ = pitch;
        }
        if (gameInput_.IsFirstReleased(GameInput::Input::kMouseLbutton)) {
            lastYaw_ = lastPitch_ = 0;
        }

        if (gameInput_.IsPressed(GameInput::Input::kKeyA)) {
            currentStrafe_ -= dT * kKeyboardSensitivity;
        }
        if (gameInput_.IsPressed(GameInput::Input::kKeyD)) {
            currentStrafe_ += dT * kKeyboardSensitivity;
        }
        if (gameInput_.IsPressed(GameInput::Input::kKeyS)) {
            currentWalk_ -= dT * kKeyboardSensitivity;
        }
        if (gameInput_.IsPressed(GameInput::Input::kKeyW)) {
            currentWalk_ += dT * kKeyboardSensitivity;
        }
        if (gameInput_.IsPressed(GameInput::Input::kKeyQ)) {
            currentAscent_ -= dT * kKeyboardSensitivity;
        }
        if (gameInput_.IsPressed(GameInput::Input::kKeyE)) {
            currentAscent_ += dT * kKeyboardSensitivity;
        }

        XMMATRIX transform(worldEast_, worldUp_, worldNorth_, g_XMIdentityR3);
        transform = transform
            * XMMatrixTranslation(currentStrafe_, currentAscent_, currentWalk_)
            * XMMatrixRotationAxis(worldUp_, currentYaw_)
            * XMMatrixRotationAxis(worldEast_, currentPitch_);

        grSetCameraAffineTransform(transform);
    }
}
