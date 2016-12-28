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

        enum class PredefinedGeometryType {
            kBox = 0,
            kSphere,
            kSize
        };

        struct RenderItemDesc {
            RenderItemDesc(const std::string& name, const XMFLOAT4X4& transform) :
                name(name), transform(transform) {}

            const std::string& name;
            const XMFLOAT4X4 transform;
        };
        struct RenderItemVerticesDesc : RenderItemDesc {
            RenderItemVerticesDesc(const std::string& name, const std::vector<Vertex>& vertices, const XMFLOAT4X4& transform) :
                RenderItemDesc(name, transform),
                vertices(vertices) {}

            const std::vector<Vertex>& vertices;
        };
        struct RenderItemTypeDesc : RenderItemDesc {
            RenderItemTypeDesc(const std::string& name, const PredefinedGeometryType type, const XMFLOAT4X4& transform) :
                RenderItemDesc(name, transform),
                type(type) {}

            const PredefinedGeometryType type;
        };
        uint_t CreateRenderItem(const std::vector<RenderItemVerticesDesc>& viewportVerticesDescs, const std::vector<RenderItemTypeDesc>& viewportTypeDescs);

        void BeforeDraw();
        void AfterDraw();

        void DrawRenderItems();

    private:
        void PrepareGeometry();

    private:
        HWND hwnd_;

        std::vector<grRenderItem> renderItems_;
        std::array<std::vector<Vertex>, (size_t)PredefinedGeometryType::kSize> geometries_;
    };

    using PredefinedGeometryType = Viewport::PredefinedGeometryType;
}
