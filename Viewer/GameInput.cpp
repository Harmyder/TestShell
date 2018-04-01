#include "stdafx.h"
#include "GameInput.h"

#include <Windows.h>

namespace Viewer
{
    void GameInput::PreUpdate() {
        memcpy(buttons_[FOLLOWED], buttons_[FIRST], sizeof(buttons_[FIRST]));
        buttons_[FIRST][Input::kMouseWheel] = false;
    }

    void GameInput::OnLostFocus() {
        memset(buttons_[FIRST], 0, sizeof(buttons_[FIRST]));
        memset(buttons_[FOLLOWED], 0, sizeof(buttons_[FOLLOWED]));
    }

    void GameInput::OnMouseLDown(int x, int y) {
        mousePrevious_ = mouseCurrent_ = { x, y };
        buttons_[FIRST][Input::kMouseLbutton] = true;
    }

    void GameInput::OnMouseRDown(int x, int y) {
        mousePrevious_ = mouseCurrent_ = { x, y };
        buttons_[FIRST][Input::kMouseRbutton] = true;
    }

    void GameInput::OnMouseMove(int x, int y) {
        mousePrevious_ = mouseCurrent_;
        mouseCurrent_ = { x,y };
    }

    void GameInput::OnMouseLUp(int, int) {
        buttons_[FIRST][Input::kMouseLbutton] = false;
    }

    void GameInput::OnMouseRUp(int, int) {
        buttons_[FIRST][Input::kMouseRbutton] = false;
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
            buttons_[FIRST][Input::kMenuKey] = value;
            break;
        case VK_ESCAPE:
            buttons_[FIRST][Input::kEscapeKey] = value;
            break;
        case VK_SPACE:
            buttons_[FIRST][Input::kSpaceKey] = value;
            break;
        }

        const int keyA = 65;
        const int keyZ = 90;
        if (keyA <= keyCode && keyCode <= keyZ) {
            int distance = keyCode - keyA;
            buttons_[FIRST][Input::kKeyA + distance] = value;
        }
    }

    void GameInput::OnMouseSet(int x, int y) {
        mousePrevious_ = mouseCurrent_ = { x, y };
    }

    void GameInput::OnWheelRotate(int delta) {
        buttons_[FOLLOWED][Input::kMouseWheel] = false;
        wheelDelta_ += delta;
    }
}
