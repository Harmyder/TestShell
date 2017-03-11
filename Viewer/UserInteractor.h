#pragma once

#include <Windows.h>

#include "Common\Attribute\NonCopyable.h"
#include "ICameraControllerObserver.h"

namespace Common {
    class Timer;
}

namespace Viewer
{
    class MenuWalker;
    class Viewport;
    class GameInput;
    class CameraController;

    class UserInteractor : public Common::NonCopyable, public ICameraControllerObserver
    {
    public:
        static UserInteractor &GetInstance();
        UserInteractor(HINSTANCE hInst);
        ~UserInteractor();

        Viewport& GetViewport() { return *viewport_; }
        GameInput& GetGameInput() { return *gameInput_; }

    private:
        static UserInteractor *s_Instance;

    public:
        void SetDeltaTime(float dT) { dT_ = dT; }
        float GetDeltaTime() const { return dT_; }

        void BeforeStep();
        bool AfterStep();

        void BeforeRender();
        void Render();
        void AfterRender();

    public:
        void HandleStartTracking() override;
        void HandleStopTracking() override;

    private:
        HWND CreateDemoWindow(HINSTANCE instance, uint32 width, uint32 height);
        bool TreatMessages();
        void WaitForDeltaTime();
        void DrawHUD();

    private:
        static LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void OnLostFocus();
        void OnGotFocus();
        void OnMouseLDown(int x, int y);
        void OnMouseRDown(int x, int y);
        void OnMouseMove(int x, int y);
        void OnMouseLUp(int x, int y);
        void OnMouseRUp(int x, int y);
        void OnKeyDown(int keyCode);
        void OnKeyUp(int keyCode);
        void OnWheelRotate(int delta);

    private:
        std::unique_ptr<GameInput> gameInput_;
        std::unique_ptr<Viewport> viewport_;
        std::unique_ptr<CameraController> cameraCtrl_;

        std::unique_ptr<Common::Timer> timer_;
        float dT_;
        uint64 startTicks_;
    };
}
