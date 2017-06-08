#include "stdafx.h"

#include "CameraController.h"

#include "GameInput.h"

using namespace std;
using namespace DirectX;
using namespace Common;

namespace Viewer
{
    CameraController::CameraController(const GameInput& gameInput, UpVector upVector) : gameInput_(gameInput) {
        if (upVector == UpVector::Y) up_ = Vector3(g_XMIdentityR1);
        else if (upVector == UpVector::Z) up_ = Vector3(g_XMIdentityR2);
        else throw "X can't be up axis";
        north_ = XMVector3Normalize(Cross(up_, Vector3(g_XMIdentityR0)));
        east_ = XMVector3Normalize(Cross(north_, up_));
    }

    void CameraController::Update(float dT) {
        if (gameInput_.IsPressed(GameInput::Input::kMenuKey))
        {
            if (gameInput_.IsFirstPressed(GameInput::Input::kMenuKey)) {
                for_each(begin(observers_), end(observers_), [](ICameraControllerObserver* o) { o->HandleStartTracking(); });
            }
            float yaw = -gameInput_.GetMouseDeltaX() * kMouseSensitivityX;
            float pitch = -gameInput_.GetMouseDeltaY() * kMouseSensitivityY;
            currentYaw_ += yaw;
            currentPitch_ += pitch;

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
            walk = -dT * kKeyboardSensitivity;
        }
        if (gameInput_.IsPressed(GameInput::Input::kKeyW)) {
            walk = dT * kKeyboardSensitivity;
        }
        if (gameInput_.IsPressed(GameInput::Input::kKeyQ)) {
            ascent = -dT * kKeyboardSensitivity;
        }
        if (gameInput_.IsPressed(GameInput::Input::kKeyE)) {
            ascent = dT * kKeyboardSensitivity;
        }
        Vector3& forward = north_;
        Vector3& right = east_;
        Matrix3 transform(right, up_, forward);
        transform = transform
            * Matrix3(XMMatrixRotationAxis(up_, currentYaw_))
            * Matrix3(XMMatrixRotationAxis(right, currentPitch_));
        Vector3 translation(strafe, ascent, walk);
        transform_ = AffineTransform(transform, transform_ * translation);
    }

    bool CameraController::IsTrackingMouse() const {
        return gameInput_.IsPressed(GameInput::Input::kMenuKey);
    }
}
