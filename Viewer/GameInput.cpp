#include "stdafx.h"
#include "GameInput.h"

#include <Windows.h>

namespace Viewer
{
    void GameInput::PreUpdate() {
        memcpy(buttons_[1], buttons_[0], sizeof(buttons_[0]));
        buttons_[0][Input::kMouseWheel] = false;
    }

    void GameInput::OnMouseLDown(int x, int y) {
        buttons_[0][Input::kMouseLbutton] = true;
        mousePressed_ = { x,y };
    }

    void GameInput::OnMouseRDown(int x, int y) {
        buttons_[0][Input::kMouseRbutton] = true;
        mousePressed_ = { x,y };
    }

    void GameInput::OnMouseMove(int x, int y) {
        mouseCurrent_ = { x,y };
    }

    void GameInput::OnMouseLUp(int x, int y) {
        buttons_[1][Input::kMouseLbutton] = true;
        mouseCurrent_ = { x,y };
    }

    void GameInput::OnMouseRUp(int x, int y) {
        buttons_[1][Input::kMouseRbutton] = true;
        mouseCurrent_ = { x,y };
    }

    void GameInput::OnKeyDown(int keyCode) {
        switch (keyCode) {
        case VK_MENU:
            buttons_[0][Input::kMenuKey] = true;
            break;
        case VK_ESCAPE:
            buttons_[0][Input::kEscapeKey] = true;
            break;
        }
    }

    void GameInput::OnKeyUp(int keyCode) {
        switch (keyCode) {
        case VK_MENU:
            buttons_[0][Input::kMenuKey] = true;
            break;
        case VK_ESCAPE:
            buttons_[0][Input::kEscapeKey] = true;
            break;
        }
    }

    void GameInput::OnWheelRotate(int delta) {
        buttons_[1][Input::kMouseWheel] = false;
        wheelDelta_ += delta;
    }
}
