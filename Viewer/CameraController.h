#pragma once
#include "ICameraControllerObserver.h"
#include "Common/Math/Vector/all.h"

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

        const Common::AffineTransform& GetTransform() const { return transform_; }
        const Common::Matrix3& GetRotation() const { return transform_.GetLinear(); }
        const Common::Vector3& GetTranslation() const { return transform_.GetTranslation(); }

        void SetTranslation(Common::Vector3 translation) { transform_.SetTranslation(translation); }
        void SetYaw(float yaw) { currentYaw_ = yaw; }
        void SetPitch(float pitch) { currentPitch_ = pitch; }

    public:
        void ObserverAdd(ICameraControllerObserver* o) { observers_.push_back(o); }
        void ObserverRemove(ICameraControllerObserver* o) { observers_.remove(o); }

    private:
        Common::Vector3 up_;
        Common::Vector3 north_;
        Common::Vector3 east_;

        Common::AffineTransform transform_;
       
        const float kMouseSensitivityX = .003f;
        const float kMouseSensitivityY = .003f;
        const float kKeyboardSensitivity = 3.f;

        const GameInput& gameInput_;

        float currentPitch_ = 0.f;
        float currentYaw_ = 0.f;

        std::list<ICameraControllerObserver*> observers_;
    };
}
