#pragma once

namespace Viewer
{
    class Camera;
    class GameInput;

    class CameraController
    {
    public:
        CameraController(Camera& camera, const GameInput& gameInput);

        void Update(float dT);

    private:
        Camera& camera_;
        const GameInput& gameInput_;

        float currentPitch_ = 0.f;
        float currentYaw_ = 0.f;
    };
}
