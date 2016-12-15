#pragma once

#include "Pile\Attribute\NonCopyable.h"

namespace Viewer
{
    class Camera;
    class CameraController;

    class Viewport : Pile::NonCopyable
    {
    public:
        Viewport(HWND hWnd);
        ~Viewport();



    private:
        HWND hwnd_;
        std::unique_ptr<CameraController> cameraCtrl_;
        std::unique_ptr<Camera> camera_;
    };
}
