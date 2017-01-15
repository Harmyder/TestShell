#pragma once
#include "ICameraControllerObserver.h"

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
        bool IsTrackingMouse() const;

    public:
        void ObserverAdd(ICameraControllerObserver* o) { observers_.push_back(o); }
        void ObserverRemove(ICameraControllerObserver* o) { observers_.remove(o); }

    private:
        XMVECTOR worldUp_;
        XMVECTOR worldNorth_;
        XMVECTOR worldEast_;
        
        const float kMouseSensitivityX = .003f;
        const float kMouseSensitivityY = .003f;
        const float kKeyboardSensitivity = 3.f;

        const GameInput& gameInput_;

        float lastPitch_ = 0.f;
        float lastYaw_ = 0.f;
        
        float currentPitch_ = 0.f;
        float currentYaw_ = 0.f;

        std::list<ICameraControllerObserver*> observers_;
    };
}
