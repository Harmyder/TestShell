#pragma once

#include "Pile\Attribute\NonCopyable.h"
#include "Graphics\Interface\GraphicsHandle.h"
#include "Graphics\Consts.h"
#include <array>
#include "Vertex.h"

namespace Viewer
{
    class Camera;
    class CameraController;
    class GameInput;

    class Viewport : Pile::NonCopyable
    {
    public:
        Viewport(HWND hWnd, const GameInput& gameInput);
        ~Viewport();

        uint_t CreateRenderItem(const std::vector<Vertex>& vertices, const std::vector<uint16>& indices);
        void UpdateRenderItem(const std::vector<Vertex>& vertices, const std::vector<uint16>& indices, uint_t index);

        void BeforeDraw();
        void AfterDraw();

        void DrawRenderItems();

    private:
        HWND hwnd_;
        std::unique_ptr<Camera> camera_;
        std::unique_ptr<CameraController> cameraCtrl_;

        std::vector<grRenderItem> renderItems_;
    };
}
