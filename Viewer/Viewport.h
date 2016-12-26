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

        struct RenderItemDesc {
            RenderItemDesc(const std::string& name, const std::vector<Vertex>& vertices, const XMFLOAT4X4& transform) :
                name(name), vertices(vertices), transform(transform) {}

            const std::string& name;
            const std::vector<Vertex>& vertices;
            const XMFLOAT4X4& transform;
        };
        uint_t CreateRenderItem(const std::vector<RenderItemDesc>& viewportDescs);

        void BeforeDraw();
        void AfterDraw();

        void DrawRenderItems();

    private:
        HWND hwnd_;

        std::vector<grRenderItem> renderItems_;
    };
}
