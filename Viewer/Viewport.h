#pragma once

#include "Pile\Attribute\NonCopyable.h"
#include "Graphics\Interface\GraphicsHandle.h"
#include "Graphics\Consts.h"
#include <array>
#include "Vertex.h"

namespace Viewer
{
    class CameraController;
    class GameInput;

    class Viewport : Pile::NonCopyable
    {
    public:
        Viewport(HWND hWnd);
        ~Viewport();

        HWND GetHwnd() { return hwnd_; }

        uint_t CreateRenderIndexedItem(const std::vector<Vertex>& vertices, const std::vector<uint16>& indices, const XMFLOAT4X4& transform);
        uint_t CreateRenderItem(const std::vector<Vertex>& vertices, const XMFLOAT4X4& transform);

        void UpdateRenderIndexedItem(const std::vector<Vertex>& vertices, const std::vector<uint16>& indices, uint_t index);
        void UpdateRenderItem(const std::vector<Vertex>& vertices, uint_t index);

        void BeforeDraw();
        void AfterDraw();

        void DrawRenderItems();

    private:
        HWND hwnd_;

        std::vector<grRenderItem> renderItems_;
        std::vector<grRenderIndexedItem> renderIndexedItems_;
    };
}
