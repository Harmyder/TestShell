#include "stdafx.h"
#include "GameInput.h"

#include <Windows.h>

namespace Viewer
{
    void GameInput::PreUpdate() {
        memcpy(buttons_[1], buttons_[0], sizeof(buttons_[0]));
        buttons_[0][Input::kMouseWheel] = false;
    }

    void GameInput::OnLostFocus() {
        memset(buttons_[0], 0, sizeof(buttons_[0]));
        memset(buttons_[1], 0, sizeof(buttons_[1]));
    }

    void GameInput::OnMouseLDown(int x, int y) {
        mousePressed_ = { x, y };
        buttons_[0][Input::kMouseLbutton] = true;
    }

    void GameInput::OnMouseRDown(int x, int y) {
        mousePressed_ = { x, y };
        buttons_[0][Input::kMouseRbutton] = true;
    }

    void GameInput::OnMouseMove(int x, int y) {
        mouseCurrent_ = { x,y };
        OutputDebugString("OnMouseMove\n");
    }

    void GameInput::OnMouseLUp(int, int) {
        buttons_[0][Input::kMouseLbutton] = false;
    }

    void GameInput::OnMouseRUp(int, int) {
        buttons_[0][Input::kMouseRbutton] = false;
    }

    void GameInput::OnKeyDown(int keyCode) {
        KeyUpDown(keyCode, true);
    }

    void GameInput::OnKeyUp(int keyCode) {
        KeyUpDown(keyCode, false);
    }

    void GameInput::KeyUpDown(int keyCode, bool value) {
        switch (keyCode) {
        case VK_MENU:
            buttons_[0][Input::kMenuKey] = value;
            break;
        case VK_ESCAPE:
            buttons_[0][Input::kEscapeKey] = value;
            break;
        case 0x41:
            buttons_[0][Input::kKeyA] = value;
            break;
        case 0x53:
            buttons_[0][Input::kKeyS] = value;
            break;
        case 0x44:
            buttons_[0][Input::kKeyD] = value;
            break;
        case 0x57:
            buttons_[0][Input::kKeyW] = value;
            break;
        case 0x45:
            buttons_[0][Input::kKeyE] = value;
            break;
        case 0x51:
            buttons_[0][Input::kKeyQ] = value;
            break;
        }
    }


    void GameInput::OnWheelRotate(int delta) {
        buttons_[1][Input::kMouseWheel] = false;
        wheelDelta_ += delta;
    }
}
