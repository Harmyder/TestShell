#pragma once

namespace Viewer
{
    class Camera;
    class GameInput;

    __declspec(align(16)) class CameraController
    {
    public:
        enum class UpVector { Y, Z };
        CameraController(const GameInput& gameInput, UpVector upVector);

        void Update(float dT);

    private:
        XMVECTOR worldUp_;
        XMVECTOR worldNorth_;
        XMVECTOR worldEast_;
        
        const float kMouseSensitivityX = .0005f;
        const float kMouseSensitivityY = .0005f;
        const float kKeyboardSensitivity = 3.f;

        const GameInput& gameInput_;

        float lastPitch_ = 0.f;
        float lastYaw_ = 0.f;
        
        float currentPitch_ = 0.f;
        float currentYaw_ = 0.f;
        float currentStrafe_ = 0.f;
        float currentWalk_ = 0.f;
        float currentAscent_ = 0.f;
    };
}
