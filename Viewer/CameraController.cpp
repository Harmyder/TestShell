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
        if (gameInput_.IsPressed(GameInput::Input::kMenuKey))
        {
            if (gameInput_.IsFirstPressed(GameInput::Input::kMenuKey)) {
                for_each(begin(observers_), end(observers_), [](ICameraControllerObserver* o) { o->HandleStartTracking(); });
            }
            float yaw = gameInput_.GetMouseDeltaX() * kMouseSensitivityX;
            float pitch = gameInput_.GetMouseDeltaY() * kMouseSensitivityY;
            currentYaw_ += yaw;
            currentPitch_ += pitch;
            lastYaw_ = yaw;
            lastPitch_ = pitch;

            currentPitch_ = XMMin(XM_PIDIV2, currentPitch_);
            currentPitch_ = XMMax(-XM_PIDIV2, currentPitch_);

            if (currentYaw_ > XM_PI)
                currentYaw_ -= XM_2PI;
            else if (currentYaw_ <= -XM_PI)
                currentYaw_ += XM_2PI;
        }
        else {
            if (gameInput_.IsFirstReleased(GameInput::Input::kMenuKey)) {
                for_each(begin(observers_), end(observers_), [](ICameraControllerObserver* const o) { o->HandleStopTracking(); });
            }
        }

        float strafe = 0;
        float walk = 0;
        float ascent = 0;
        if (gameInput_.IsPressed(GameInput::Input::kKeyA)) {
            strafe = -dT * kKeyboardSensitivity;
        }
        if (gameInput_.IsPressed(GameInput::Input::kKeyD)) {
            strafe = dT * kKeyboardSensitivity;
        }
        if (gameInput_.IsPressed(GameInput::Input::kKeyS)) {
            walk = dT * kKeyboardSensitivity;
        }
        if (gameInput_.IsPressed(GameInput::Input::kKeyW)) {
            walk = -dT * kKeyboardSensitivity;
        }
        if (gameInput_.IsPressed(GameInput::Input::kKeyQ)) {
            ascent = -dT * kKeyboardSensitivity;
        }
        if (gameInput_.IsPressed(GameInput::Input::kKeyE)) {
            ascent = dT * kKeyboardSensitivity;
        }

        XMMATRIX transform(worldEast_, worldUp_, worldNorth_, g_XMIdentityR3);
        transform = transform
            * XMMatrixRotationAxis(worldEast_, currentPitch_)
            * XMMatrixRotationAxis(worldUp_, currentYaw_);

        XMVECTOR frameTranslation = { strafe, ascent, walk, 0.f };
        frameTranslation = XMVectorSetW(XMVector3Transform(frameTranslation, transform), 1.f);
        XMVECTOR translation = frameTranslation + grGetCameraPosition();
        
        grSetCameraAffineTransform(transform, translation);
    }

    bool CameraController::IsTrackingMouse() const {
        return gameInput_.IsPressed(GameInput::Input::kMenuKey);
    }
}
