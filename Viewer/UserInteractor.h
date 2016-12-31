#pragma once

#include <Windows.h>

#include "Pile\Attribute\NonCopyable.h"

namespace Pile {
    class Timer;
}

namespace Viewer
{
    class MenuWalker;
    class Viewport;
    class GameInput;
    class CameraController;

    class UserInteractor : public Pile::NonCopyable
    {
    public:
        static UserInteractor &GetInstance();
        UserInteractor(HINSTANCE hInst);
        ~UserInteractor();

        Viewport& GetViewport() { return *viewport_; }

    private:
        static UserInteractor *s_Instance;

    public:
        void SetMenuWalker(MenuWalker *menuWalker) { menuWalker_ = menuWalker; }

        void SetDeltaTime(float dT) { dT_ = dT; }
        float GetDeltaTime() const { return dT_; }

        void BeforeStep();
        void AfterStep();

        void BeforeRender();
        void Render();
        void AfterRender();

    private:
        HWND CreateDemoWindow(HINSTANCE instance, uint32 width, uint32 height);
        void TreatMessages();
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
        MenuWalker *menuWalker_;

        std::unique_ptr<GameInput> gameInput_;
        std::unique_ptr<Viewport> viewport_;
        std::unique_ptr<CameraController> cameraCtrl_;

        std::unique_ptr<Pile::Timer> timer_;
        float dT_;
        uint64 startTicks_;
    };
}
