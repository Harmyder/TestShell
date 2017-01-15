#pragma once
#include "ICameraControllerObserver.h"

namespace Viewer
{
    class Camera;
    class GameInput;

    class CameraController
    {
    public:
        enum class UpVector { Y, Z };
        CameraController(const GameInput& gameInput, UpVector upVector);

        void Update(float dT);
        bool IsTrackingMouse() const;

        XMFLOAT3X3 GetTransform() { return transform_; }
        XMFLOAT3 GetFrameTranslation() { return translation_; }

    public:
        void ObserverAdd(ICameraControllerObserver* o) { observers_.push_back(o); }
        void ObserverRemove(ICameraControllerObserver* o) { observers_.remove(o); }

    private:
        XMFLOAT3 worldUp_;
        XMFLOAT3 worldNorth_;
        XMFLOAT3 worldEast_;

        XMFLOAT3X3 transform_;
        XMFLOAT3 translation_;
       
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
