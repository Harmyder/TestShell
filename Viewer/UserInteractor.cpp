#include "stdafx.h"

#include "UserInteractor.h"
#include <WindowsX.h>

#include "Pile\Timer\Timer.h"
#include "GameInput.h"
#include "Viewport.h"
#include "CameraController.h"
using namespace std;
using namespace Pile;

constexpr uint32 kWidth = 800;
constexpr uint32 kHeight = 600;

namespace Viewer
{
    UserInteractor* UserInteractor::s_Instance = NULL;

    UserInteractor& UserInteractor::GetInstance() {
        return *s_Instance;
    }

    UserInteractor::UserInteractor(HINSTANCE hInst) :
        gameInput_(make_unique<GameInput>()),
        viewport_(make_unique<Viewport>(CreateDemoWindow(hInst, kWidth, kHeight))),
        cameraCtrl_(make_unique<CameraController>(*gameInput_, CameraController::UpVector::Y)),
        timer_(make_unique<Timer>())
    {
        assert(s_Instance == NULL);
        s_Instance = this;
    }

    UserInteractor::~UserInteractor() {}

    HWND UserInteractor::CreateDemoWindow(HINSTANCE instance, uint32 width, uint32 height) {
        RECT rc = { 0, 0, (LONG)width, (LONG)height };
        char *szWindowClass = "Window class";

        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = UserInteractor::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = instance;
        wcex.hIcon = NULL;
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = 0;
        wcex.lpszClassName = szWindowClass;
        wcex.hIconSm = NULL;
        RegisterClassEx(&wcex);

        HWND wnd = CreateWindow(szWindowClass, "Demo Window", WS_OVERLAPPEDWINDOW,
            0, 0, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, instance, NULL);

        ShowWindow(wnd, SW_SHOWDEFAULT);

        return wnd;
    }

    LRESULT CALLBACK UserInteractor::WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        UserInteractor &userInteractor = UserInteractor::GetInstance();
        switch (msg)
        {
        case WM_ACTIVATE:
            if (wParam == WA_INACTIVE)
                userInteractor.OnLostFocus();
            else if (wParam == WA_ACTIVE)
                userInteractor.OnGotFocus();
            return 0;
        case WM_LBUTTONDOWN:
            userInteractor.OnMouseLDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        case WM_RBUTTONDOWN:
            userInteractor.OnMouseRDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        case WM_LBUTTONUP:
            userInteractor.OnMouseLUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        case WM_RBUTTONUP:
            userInteractor.OnMouseRUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        case WM_MOUSEMOVE:
            userInteractor.OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        case WM_MOUSEWHEEL:
            userInteractor.OnWheelRotate(GET_WHEEL_DELTA_WPARAM(wParam));
            return 0;
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
            if (lParam & (1 << 30))
            {
                // this is a repeat
            }
            else
            {
                // first press
                userInteractor.OnKeyDown((int)wParam);
            }
            return 0;
        case WM_SYSKEYUP:
        case WM_KEYUP:
            userInteractor.OnKeyUp((int)wParam);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            break;
        }
        return DefWindowProc(wnd, msg, wParam, lParam);
    }

    void UserInteractor::OnLostFocus() {
        gameInput_->OnLostFocus();
    }

    void UserInteractor::OnGotFocus() {
    }

    void UserInteractor::OnMouseLDown(int x, int y) {
        SetCapture(viewport_->GetHwnd());
        gameInput_->OnMouseLDown(x, y);
    }

    void UserInteractor::OnMouseRDown(int x, int y) {
        SetCapture(viewport_->GetHwnd());
        gameInput_->OnMouseRDown(x, y);
    }

    void UserInteractor::OnMouseMove(int x, int y) {
        POINT p = { x, y };
        if (cameraCtrl_->IsTrackingMouse()) {
            if (cameraCtrl_->IsStartedTrackingMouse()) SetCapture(viewport_->GetHwnd());
            const int width = viewport_->GetWidth();
            const int height = viewport_->GetHeight();

            if (x < 0) x = width;
            if (x > width) x = 0;

            if (y < 0) y = height;
            if (y > height) y = 0;

            if (p.x != x || p.y != y)
            {
                gameInput_->OnMouseSet(x, y);
                POINT screen = {x, y};
                ClientToScreen(viewport_->GetHwnd(), &screen);
                SetCursorPos(screen.x, screen.y);
            }
        }
        else {
            if (cameraCtrl_->IsStopedTrackingMouse()) ReleaseCapture();
        }
        gameInput_->OnMouseMove(x, y);
    }

    void UserInteractor::OnMouseLUp(int x, int y) {
        ReleaseCapture();
        gameInput_->OnMouseLUp(x, y);
    }

    void UserInteractor::OnMouseRUp(int x, int y) {
        ReleaseCapture();
        gameInput_->OnMouseRUp(x, y);
    }

    void UserInteractor::OnKeyDown(int keyCode) {
        gameInput_->OnKeyDown(keyCode);
    }

    void UserInteractor::OnKeyUp(int keyCode) {
        gameInput_->OnKeyUp(keyCode);
    }

    void UserInteractor::OnWheelRotate(int delta) {
        gameInput_->OnWheelRotate(delta);
    }

    void UserInteractor::TreatMessages() {
        MSG  msg;

        while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    void UserInteractor::BeforeStep() {
        uint64 newStartTicks = timer_->GetCounts();
        const float frameTime = static_cast<float>((newStartTicks - startTicks_) * timer_->GetInvFrequency());
        startTicks_ = newStartTicks;

        cameraCtrl_->Update(frameTime);
        gameInput_->PreUpdate();
    }

    void UserInteractor::AfterStep() {
        WaitForDeltaTime();
        gameInput_->ClearDeltas();
        TreatMessages();
    }

    void UserInteractor::DrawHUD() {
        viewport_->BeforeTransparent(); // Transparent don't use depth buffer
        viewport_->DrawReferenceFrame();
    }

    void UserInteractor::WaitForDeltaTime() {
        uint64 dtInTicks = static_cast<uint64>(dT_ *timer_->GetFrequency());
        uint64 currentDtInTicks = 0;
        do
        {
            const uint64 currentTicks = timer_->GetCounts();
            currentDtInTicks = currentTicks - startTicks_;
        } while (currentDtInTicks < dtInTicks);
    }

    void UserInteractor::BeforeRender() {
        viewport_->BeforeDraw();
    }

    void UserInteractor::Render() {
        viewport_->BeforeLine();
        viewport_->DrawGrid();
        
        viewport_->BeforeOpaque();
        viewport_->DrawRenderItemsOpaque();

        viewport_->BeforeTransparent();
        viewport_->DrawRenderItemsTransparent();
    }

    void UserInteractor::AfterRender() {
        DrawHUD();
        viewport_->AfterDraw();
    }
}
