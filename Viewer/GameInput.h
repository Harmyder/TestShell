#pragma once

#include "Common/Attribute/NonCopyable.h"

namespace Viewer
{
    class GameInput : public Common::NonCopyable
    {
    public:
        struct InputEnum {
            enum Input {
                kMenuKey,
                kSpaceKey,
                kEscapeKey,

                kKeyA,
                kKeyB,
                kKeyC,
                kKeyD,
                kKeyE,
                kKeyF,
                kKeyG,
                kKeyH,
                kKeyI,
                kKeyJ,
                kKeyK,
                kKeyL,
                kKeyM,
                kKeyN,
                kKeyO,
                kKeyP,
                kKeyQ,
                kKeyR,
                kKeyS,
                kKeyT,
                kKeyU,
                kKeyV,
                kKeyW,
                kKeyX,
                kKeyY,
                kKeyZ,

                kMouseRbutton,
                kMouseLbutton,
                kMouseWheel,

                kNumInputs
            };
        };
        using Input = InputEnum::Input;

        bool IsPressed(Input i) const { return buttons_[FIRST][i]; }
        bool IsFirstPressed(Input i) const { return buttons_[FIRST][i] && !buttons_[FOLLOWED][i]; }
        bool IsFirstReleased(Input i) const { return buttons_[FOLLOWED][i] && !buttons_[FIRST][i]; }

        float GetDurationPressed(Input i) { return holdDuration_[(uint32)i]; }

        int GetMouseDeltaX() const { return mousePrevious_.x - mouseCurrent_.x; }
        int GetMouseDeltaY() const { return mousePrevious_.y - mouseCurrent_.y; }
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

        void OnMouseSet(int x, int y);

        void ClearDeltas() {
            mousePrevious_ = mouseCurrent_;
            wheelDelta_ = 0;
        }

    private:
        struct Point { int x, y; };
        Point mousePrevious_;
        Point mouseCurrent_;

        enum { FIRST = 0, FOLLOWED = 1 };

        int wheelDelta_;

        bool buttons_[2][Input::kNumInputs];
        float holdDuration_[Input::kNumInputs] = { 0.0f };
    };
}
