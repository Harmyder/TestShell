#pragma once

#include "Pile\Attribute\NonCopyable.h"
#include "Graphics\Interface\GraphicsHandle.h"
#include "Vertex.h"
#include <array>
#include <unordered_map>

namespace Viewer
{
    class CameraController;
    class GameInput;

    class Viewport : Pile::NonCopyable
    {
    public:
        Viewport(HWND hWnd);
        ~Viewport();

        HWND GetHwnd() const { return hwnd_; }
        uint32 GetWidth() const { return width_; }
        uint32 GetHeight() const { return height_; }

        enum class PredefinedGeometryType {
            kBox = 0,
            kSphere,
            kCylinder,
            kCone,
            kSize
        };

        enum class Material {
            kRed, kGreen, kBlue,
            kTurquesa,
            kEmerald,
            kJade,
            kObsidian,
            kSilver,
        };
        struct RenderItemDesc {
            RenderItemDesc(const std::string& name, const XMFLOAT4X4& transform, const std::string& material) :
                name(name), transform(transform), material(material) {}

            const std::string name;
            const XMFLOAT4X4 transform;
            const std::string material;
        };
        struct RenderItemVerticesDesc : RenderItemDesc {
            RenderItemVerticesDesc(const std::string& name, const std::vector<Vertex>& vertices, const XMFLOAT4X4& transform, const std::string& material) :
                RenderItemDesc(name, transform, material),
                vertices(vertices) {}

            const std::vector<Vertex>& vertices;
        };
        struct RenderItemTypeDesc : RenderItemDesc {
            RenderItemTypeDesc(const std::string& name, const PredefinedGeometryType type, const XMFLOAT4X4& transform, const std::string& material) :
                RenderItemDesc(name, transform, material),
                type(type) {}

            const PredefinedGeometryType type;
        };
        void CreateMaterial(Material material, const std::string& name);
        uint_t CreateRenderItem(const std::vector<RenderItemVerticesDesc>& viewportVerticesDescs, const std::vector<RenderItemTypeDesc>& viewportTypeDescs);

        void BeforeDraw();
        void AfterDraw();
        void BeforeHud();

        void DrawRenderItems();

        void DrawReferenceFrame();

    private:
        void PrepareGeometry();
        XMVECTOR Convert2DTo3D(uint32 x, uint32 y) const;
        grRenderItem CreateRenderItemInternal(const std::vector<RenderItemVerticesDesc>& viewportVerticesDescs, const std::vector<RenderItemTypeDesc>& viewportTypeDescs);

    private:
        HWND hwnd_;
        uint32 width_;
        uint32 height_;

        std::vector<grRenderItem> renderItems_;
        std::array<std::vector<Vertex>, (size_t)PredefinedGeometryType::kSize> geometries_;
        std::unordered_map<std::string, grMaterial> materials_;

        grRenderItem referenceFrame_;
    };

    using PredefinedGeometryType = Viewport::PredefinedGeometryType;
}
