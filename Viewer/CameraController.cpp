#include "stdafx.h"

#include "CameraController.h"

#include "GameInput.h"

using namespace std;
using namespace DirectX;

namespace Viewer
{
    CameraController::CameraController(const GameInput& gameInput, UpVector upVector) : gameInput_(gameInput) {
        XMVECTOR up, north, east;
        if (upVector == UpVector::Y) up = g_XMIdentityR1;
        else if (upVector == UpVector::Z) up = g_XMIdentityR2;
        else throw "X can't be up axis";
        north = XMVector3Normalize(XMVector3Cross(up, g_XMIdentityR0));
        east = XMVector3Normalize(XMVector3Cross(north, up));
        XMStoreFloat3(&worldUp_, up);
        XMStoreFloat3(&worldNorth_, north);
        XMStoreFloat3(&worldEast_, east);
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
            strafe = +dT * kKeyboardSensitivity;
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

        XMVECTOR up = XMLoadFloat3(&worldUp_);
        XMVECTOR forward = XMLoadFloat3(&worldNorth_);
        XMVECTOR right = XMLoadFloat3(&worldEast_);
        XMMATRIX transform(right, up, forward, g_XMIdentityR3);
        transform = transform
            * XMMatrixRotationAxis(right, currentPitch_)
            * XMMatrixRotationAxis(up, currentYaw_);

        XMVECTOR translation = { strafe, ascent, walk, 0.f };
        translation = XMVectorSetW(XMVector3Transform(translation, transform), 1.f);

        XMStoreFloat3x3(&transform_, transform);
        XMStoreFloat3(&translation_, translation);
    }

    bool CameraController::IsTrackingMouse() const {
        return gameInput_.IsPressed(GameInput::Input::kMenuKey);
    }
}
