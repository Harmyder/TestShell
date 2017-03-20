#pragma once

#include "Common\DefineNamespaceEnumType.h"
#include "Common\Attribute\NonCopyable.h"
#include "Graphics\Interface\GraphicsHandle.h"
#include "Graphics\Interface\GraphicsConsts.h"
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
    using RenderItemWithInstancesId = std::list<grRenderItemWithInstances>::const_iterator;
    struct StructRenderItemWithInstancesId { 
        StructRenderItemWithInstancesId(RenderItemWithInstancesId id) : Value(id) {} 
        RenderItemWithInstancesId Value;
    };

    struct RenderItemDesc {
        RenderItemDesc(const std::string& name, const XMFLOAT4X3& transform, PrimitiveTopology::Type primitiveTopology) :
            name(name), transform(transform), primitiveTopology(primitiveTopology) {}

        const std::string name;
        const XMFLOAT4X3 transform;
        const PrimitiveTopology::Type primitiveTopology;
    };
    struct RenderItemInstanceDesc {
        RenderItemInstanceDesc(XMFLOAT4X3& transform, std::string& material) : transform(transform), material(material) {}
        XMFLOAT4X3 transform;
        std::string& material;
    };
    struct RenderItemWithInstancesDesc : RenderItemDesc {
        RenderItemWithInstancesDesc(
            const std::string& name,
            const uint8* vertices,
            uint32 verticesCount,
            const uint8* indices,
            uint32 indicesCount,
            const XMFLOAT4X3& transform,
            PrimitiveTopology::Type primitiveTopology,
            const RenderItemInstanceDesc* instances,
            uint32 instancesCount
            ) :
            RenderItemDesc(name, transform, primitiveTopology),
            vertices(vertices),
            verticesCount(verticesCount),
            indices(indices),
            indicesCount(indicesCount),
            instances(instances),
            instancesCount(instancesCount)
        {}
        const uint8* vertices;
        const uint32 verticesCount;
        const uint8* indices;
        const uint32 indicesCount;
        const RenderItemInstanceDesc* instances;
        const uint32 instancesCount;
    };
    struct RenderItemVerticesDesc : RenderItemDesc {
        RenderItemVerticesDesc(const std::string& name, const uint8* vertices, uint32 verticesCount, uint8* indices, uint32 indicesCount, const XMFLOAT4X3& transform, const std::string& material, PrimitiveTopology::Type primitiveTopology) :
            RenderItemDesc(name, transform, primitiveTopology),
            material(material),
            vertices(vertices),
            verticesCount(verticesCount),
            indices(indices),
            indicesCount(indicesCount)
        {}

        const std::string material;
        const uint8* vertices;
        const uint32 verticesCount;
        const uint8* indices;
        const uint32 indicesCount;
    };
    struct RenderItemTypeDesc : RenderItemDesc {
        RenderItemTypeDesc(const std::string& name, const PredefinedGeometryType type, const XMFLOAT4X3& transform, const std::string& material, PrimitiveTopology::Type primitiveTopology) :
            RenderItemDesc(name, transform, primitiveTopology),
            material(material),
            type(type) {}

        const std::string material;
        const PredefinedGeometryType type;
    };
    using DescsVertices = std::vector<RenderItemVerticesDesc>;
    using DescsTypes = std::vector<RenderItemTypeDesc>;
    struct RenderItemsDescriptions
    {
        DescsVertices Vertices;
        DescsTypes Types;
    };

    class Viewport : Common::NonCopyable
    {
    public:
        Viewport(HWND hWnd);
        ~Viewport();

        void UpdateCamera(const XMFLOAT3X3& transform, const XMFLOAT3& frameTranslation);

        HWND GetHwnd() const { return hwnd_; }
        uint32 GetWidth() const { return width_; }
        uint32 GetHeight() const { return height_; }

        void CreateMaterial(Material::Type material, const std::string& name);
        void DestroyMaterial(const std::string& name);

        RenderItemId CreateRenderItemOpaque(const DescsVertices& viewportVerticesDescs, uint32 vertexSize);
        RenderItemId CreateRenderItemOpaque(const DescsTypes& viewportTypeDescs);
        RenderItemId CreateRenderItemTransparent(const DescsVertices& viewportVerticesDescs, uint32 vertexSize);
        RenderItemId CreateRenderItemTransparent(const DescsTypes& viewportTypeDescs);
        RenderItemWithInstancesId CreateRenderItemOpaqueWithInstances(const RenderItemWithInstancesDesc& viewportWithInstancesDesc, uint32 vertexSize);
        void DestroyRenderItemOpaque(const StructRenderItemId& id);
        void DestroyRenderItemTransparent(const StructRenderItemId& id);
        void DestroyRenderItemOpaqueWithInstances(const StructRenderItemWithInstancesId& id);

        void UpdateRenderSubitemTransform(const StructRenderItemId& id, const std::string& name, const XMFLOAT4X3& transform);

        void BeforeDraw();
        void AfterDraw();
        void BeforeOpaque();
        void BeforeOpaqueWithInstances();
        void BeforeLine();
        void BeforeTransparent();

        void DrawRenderItemsOpaque();
        void DrawRenderItemsOpaqueWithInstances();
        void DrawRenderItemsTransparent();

        void DrawReferenceFrame();
        void DrawGrating();

    private:
        void PrepareGeometry();
        void PrepareRootSignatures();
        void PreparePsos();
        XMVECTOR Convert2DTo3D(uint32 x, uint32 y) const;

        enum class RootSignatureType { kLighting, kLightingWithInstances, kColor, kSize };
        void SetRootSignature(RootSignatureType type);

        enum class PsoType { kOpaque, kOpaqueWithInstances, kTransparent, kLine, kSize };
        void SetPso(PsoType type);

        grRenderItem CreateRenderItemInternal(const DescsVertices& descs, uint32 vertexSize);
        grRenderItem CreateRenderItemInternal(const DescsTypes& descs);
        grRenderItemWithInstances CreateRenderItemInternal(const RenderItemWithInstancesDesc& descs, uint32 vertexSize);

    private:
        HWND hwnd_;
        uint32 width_;
        uint32 height_;

        std::list<grRenderItem> renderItemsOpaque_;
        std::list<grRenderItem> renderItemsTransparent_;
        std::list<grRenderItemWithInstances> renderItemsWithInstances_;
        std::array<std::pair<std::vector<VertexNormalTex>, std::vector<uint16>>, (size_t)PredefinedGeometryType::kSize> geometries_;
        std::unordered_map<std::string, grMaterial> materials_;

        RootSignatureType currentRootSignatureType_;
        std::unordered_map<RootSignatureType, grRootSignature> rootSignatures_;
        PsoType currentPsoType_ = PsoType::kSize;
        std::unordered_map<PsoType, grPipelineStateObject> psos_;
        std::unordered_map<PsoType, RootSignatureType> pso2rs_;
        grRenderItem referenceFrame_;
        grRenderItem grating_;

        grDirectionalLight lightKey_;
        grDirectionalLight lightFill_;
        grDirectionalLight lightBack_;
        grPointLight lightPoint_;
        grSpotLight lightSpot_;
    };
}
