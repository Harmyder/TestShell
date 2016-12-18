#pragma once

#include "Pile\Attribute\NonCopyable.h"
#include "RenderItem.h"
#include "Graphics\Consts.h"
#include <array>

namespace Graphics {
    class GraphicsCore;
}

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

        void CreateRenderItem(const std::vector<Vertex>& vertices, const std::vector<uint16>& indices, RenderItem& renderItem);
        void UpdateRenderItem(const std::vector<Vertex>& vertices, const std::vector<uint16>& indices, RenderItem& renderItem);
        RenderItem& GetRenderItem(uint32 i) { return renderItems_[i]; }

    private:
        HWND hwnd_;
        std::unique_ptr<Camera> camera_;
        std::unique_ptr<CameraController> cameraCtrl_;

        std::unique_ptr<Graphics::GraphicsCore> graphicsCore_;
        std::array<RenderItem, Graphics::kSceneObjectsCountAllowed> renderItems_;
    };
}
