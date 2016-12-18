#include "stdafx.h"

#include "UserInteractor.h"
#include <WindowsX.h>

#include "Pile/Timer/Timer.h"
#include "Menu/MenuWalker.h"
#include "GameInput.h"
#include "Viewport.h"

using namespace std;
using namespace Pile;

namespace Viewer
{
    UserInteractor* UserInteractor::s_Instance = NULL;

    UserInteractor& UserInteractor::GetInstance()
    {
        return *s_Instance;
    }

    UserInteractor::UserInteractor(HINSTANCE hInst) :
        gameInput_(make_unique<GameInput>()),
        viewport_(make_unique<Viewport>(CreateDemoWindow(hInst, 800, 600), *gameInput_)),
        timer_(make_unique<Timer>())
    {}

    UserInteractor::~UserInteractor() {}

    HWND UserInteractor::CreateDemoWindow(HINSTANCE instance, uint32 width, uint32 height)
    {
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

        return wnd;
    }

    LRESULT CALLBACK UserInteractor::WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        UserInteractor &userInteractor = UserInteractor::GetInstance();
        switch (msg)
        {
        case WM_ACTIVATE:
            if (wParam == WA_INACTIVE)
                userInteractor.OnLostFocus();
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

    void UserInteractor::OnLostFocus() {}

    void UserInteractor::OnMouseLDown(int x, int y)
    {
        gameInput_->OnMouseLDown(x, y);
    }

    void UserInteractor::OnMouseRDown(int x, int y)
    {
        gameInput_->OnMouseRDown(x, y);
    }

    void UserInteractor::OnMouseMove(int x, int y)
    {
        gameInput_->OnMouseMove(x, y);
    }

    void UserInteractor::OnMouseLUp(int x, int y)
    {
        gameInput_->OnMouseLUp(x, y);
    }

    void UserInteractor::OnMouseRUp(int x, int y)
    {
        gameInput_->OnMouseRUp(x, y);
    }

    void UserInteractor::OnKeyDown(int keyCode)
    {
        switch (keyCode)
        {
        case VK_LEFT:
            menuWalker_->GoParent();
            break;
        case VK_RIGHT:
            menuWalker_->GoChild();
            break;
        case VK_UP:
            menuWalker_->GoPrevious();
            break;
        case VK_DOWN:
            menuWalker_->GoNextHandle();
            break;
        default:
            gameInput_->OnKeyDown(keyCode);
        }
    }

    void UserInteractor::OnKeyUp(int keyCode)
    {
        gameInput_->OnKeyUp(keyCode);
    }

    void UserInteractor::OnWheelRotate(int delta)
    {
        gameInput_->OnWheelRotate(delta);
    }

    void UserInteractor::TreatMessages()
    {
        MSG  msg;

        while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    void UserInteractor::BeforeStep()
    {
        startTicks_ = timer_->GetCounts();
    }

    void UserInteractor::AfterStep()
    {
        WaitForDeltaTime();
        TreatMessages();
    }

    void UserInteractor::WaitForDeltaTime()
    {
        uint64 dtInTicks = static_cast<uint64>(dT_ *timer_->GetFrequency());
        uint64 currentDtInTicks = 0;
        do
        {
            const uint64 currentTicks = timer_->GetCounts();
            currentDtInTicks = currentTicks - startTicks_;
        } while (currentDtInTicks < dtInTicks);
    }
}
