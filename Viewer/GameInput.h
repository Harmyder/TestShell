#pragma once

#include "Pile/Attribute/NonCopyable.h"

namespace Viewer
{
    class GameInput : public Pile::NonCopyable
    {
    public:
        struct InputEnum {
            enum Input {
                kMenuKey,
                kEscapeKey,

                kKeyA,
                kKeyS,
                kKeyD,
                kKeyW,
                kKeyE,
                kKeyQ,

                kMouseRbutton,
                kMouseLbutton,
                kMouseWheel,

                kNumInputs
            };
        };
        using Input = InputEnum::Input;

        bool IsPressed(Input i) const { return buttons_[0][i]; }
        bool IsFirstPressed(Input i) const { return buttons_[0][i] && !buttons_[1][i]; }
        bool IsFirstReleased(Input i) const { return buttons_[1][i] && !buttons_[0][i]; }

        float GetDurationPressed(Input i) { return holdDuration_[(uint32)i]; }

        int GetMousePressedDeltaX() const { return mousePressed_.x - mouseCurrent_.x; }
        int GetMousePressedDeltaY() const { return mousePressed_.y - mouseCurrent_.y; }
        int GetMouseWheelDelta() const { return wheelDelta_; }

        void PreUpdate();
        void OnLostFocus();
        void OnMouseLDown(int x, int y);
        void OnMouseRDown(int x, int y);
        void OnMouseMove(int x, int y);
        void OnMouseLUp(int x, int y);
        void OnMouseRUp(int x, int y);
        void OnKeyDown(int keyCode);
        void OnKeyUp(int keyCode);
        void OnWheelRotate(int delta);

        void KeyUpDown(int keyCode, bool value);

    private:
        struct Point { int x, y; };
        Point mousePressed_;
        Point mouseCurrent_;

        int wheelDelta_;

        bool buttons_[2][Input::kNumInputs];
        float holdDuration_[Input::kNumInputs] = { 0.0f };
    };
}
