#pragma once

#include "Pile\DefineNamespaceEnumType.h"
#include "Pile\Attribute\NonCopyable.h"
#include "Graphics\Interface\GraphicsHandle.h"
#include "Vertex.h"
#include <array>
#include <unordered_map>

namespace Viewer
{
    class CameraController;
    class GameInput;

    namespace PrimitiveTopology {
        DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
        Type kInvalid();
        Type kTriangleList();
        Type kLineList();
    }

    enum class PredefinedGeometryType {
        kBox = 0,
        kSphere,
        kCylinder,
        kCone,
        kSize
    };

    namespace Material {
        DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
        Type kInvalid();
        Type kRed();
        Type kGreen();
        Type kBlue();
        Type kTurquesa();
        Type kEmerald();
        Type kJade();
        Type kObsidian();
        Type kSilver();
    }

    using RenderItemId = std::list<grRenderItem>::const_iterator;
    struct StructRenderItemId { StructRenderItemId(RenderItemId id) : Value(id) {} RenderItemId Value; };

    struct RenderItemDesc {
        RenderItemDesc(const std::string& name, const XMFLOAT4X4& transform, const std::string& material, PrimitiveTopology::Type primitiveTopology) :
            name(name), transform(transform), material(material), primitiveTopology(primitiveTopology) {}

        const std::string name;
        const XMFLOAT4X4 transform;
        const std::string material;
        const PrimitiveTopology::Type primitiveTopology;
    };
    struct RenderItemVerticesDesc : RenderItemDesc {
        RenderItemVerticesDesc(const std::string& name, const uint8* vertices, uint32 verticesCount, const XMFLOAT4X4& transform, const std::string& material, PrimitiveTopology::Type primitiveTopology) :
            RenderItemDesc(name, transform, material, primitiveTopology),
            vertices(vertices),
            verticesCount(verticesCount)
        {}

        const uint8* vertices;
        const uint32 verticesCount;
    };
    struct RenderItemTypeDesc : RenderItemDesc {
        RenderItemTypeDesc(const std::string& name, const PredefinedGeometryType type, const XMFLOAT4X4& transform, const std::string& material, PrimitiveTopology::Type primitiveTopology) :
            RenderItemDesc(name, transform, material, primitiveTopology),
            type(type) {}

        const PredefinedGeometryType type;
    };
    using DescsVertices = std::vector<RenderItemVerticesDesc>;
    using DescsTypes = std::vector<RenderItemTypeDesc>;
    struct RenderItemsDescriptions
    {
        DescsVertices Vertices;
        DescsTypes Types;
    };

    class Viewport : Pile::NonCopyable
    {
    public:
        Viewport(HWND hWnd);
        ~Viewport();

        HWND GetHwnd() const { return hwnd_; }
        uint32 GetWidth() const { return width_; }
        uint32 GetHeight() const { return height_; }

        void CreateMaterial(Material::Type material, const std::string& name);
        RenderItemId CreateRenderItemOpaque(const DescsVertices& viewportVerticesDescs, uint32 vertexSize);
        RenderItemId CreateRenderItemOpaque(const DescsTypes& viewportTypeDescs);
        RenderItemId CreateRenderItemTransparent(const DescsVertices& viewportVerticesDescs, uint32 vertexSize);
        RenderItemId CreateRenderItemTransparent(const DescsTypes& viewportTypeDescs);
        void DestroyRenderItemOpaque(const StructRenderItemId& id);
        void DestroyRenderItemTransparent(const StructRenderItemId& id);

        void BeforeDraw();
        void AfterDraw();
        void BeforeOpaque();
        void BeforeLine();
        void BeforeTransparent();

        void DrawRenderItemsOpaque();
        void DrawRenderItemsTransparent();

        void DrawReferenceFrame();
        void DrawGrid();

    private:
        void PrepareGeometry();
        void PreparePsos();
        XMVECTOR Convert2DTo3D(uint32 x, uint32 y) const;

        enum class PsoType {
            kOpaque,
            kTransparent,
            kLine,
            kSize
        };
        void SetPso(PsoType type);
        grRenderItem CreateRenderItemInternal(const DescsVertices& viewportVerticesDescs, uint32 vertexSize);
        grRenderItem CreateRenderItemInternal(const DescsTypes& viewportTypeDescs);

    private:
        HWND hwnd_;
        uint32 width_;
        uint32 height_;

        std::list<grRenderItem> renderItemsOpaque_;
        std::list<grRenderItem> renderItemsTransparent_;
        std::array<std::vector<VertexNormalTex>, (size_t)PredefinedGeometryType::kSize> geometries_;
        std::unordered_map<std::string, grMaterial> materials_;

        grRootSignature rootSignature_;
        PsoType currentPsoType_ = PsoType::kSize;
        std::unordered_map<PsoType, grPipelineStateObject> psos_;
        grRenderItem referenceFrame_;
        grRenderItem grid_;
    };
}
