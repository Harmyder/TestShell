#pragma once

#include "Common/DefineNamespaceEnumType.h"
#include "Common/Attribute/NonCopyable.h"
#include "Graphics/Interface/GraphicsHandle.h"
#include "Graphics/Interface/GraphicsConsts.h"
#include "Vertex.h"
#include "Common/Container/Dynarray.h"
#include <array>
#include <unordered_map>

namespace Viewer
{
    class GameInput;
    class MaterialRaii;

    namespace PrimitiveTopology {
        DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
        Type kInvalid();
        Type kTriangleList();
        Type kLineList();
        Type kPointList();
    }

    enum class PredefinedGeometryType {
        kBox = 0,
        kSphere,
        kCylinder,
        kCone,
        kSize
    };

    namespace MaterialType {
        DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
        Type kInvalid();
        Type kRed();
        Type kGreen();
        Type kBlue();
        Type kWhite();
        Type kTurquesa();
        Type kEmerald();
        Type kJade();
        Type kObsidian();
        Type kSilver();
    }

    namespace ResourceFormat {
        DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
        Type kInvalid();
        Type kR8G8B8A8_UNORM();
    }

    struct Material {
        Material() : Value(grcMaterialNone) {}
    private:
        Material(grMaterial material) : Value(material) {}
        friend class Viewport;
        operator grMaterial() { return *(grMaterial*)this; }
        grMaterial Value;
    };

    struct Texture {
        Texture() : Value(grcTextureNone) {}
    private:
        Texture(grTexture texture) : Value(texture) {}
        friend class Viewport;
        operator grTexture() { return *(grTexture*)this; }
        grTexture Value;
    };

    using RenderItemId = std::list<grRenderItem>::const_iterator;
    struct StructRenderItemId { StructRenderItemId(RenderItemId id) : Value(id) {} RenderItemId Value; };
    using RenderItemParticlesId = std::list<grRenderItemParticles>::const_iterator;
    struct StructRenderItemParticlesId { StructRenderItemParticlesId(RenderItemParticlesId id) : Value(id) {} RenderItemParticlesId Value; };
    using RenderItemWithInstancesId = std::list<grRenderItemWithInstances>::const_iterator;
    struct StructRenderItemWithInstancesId { 
        StructRenderItemWithInstancesId(RenderItemWithInstancesId id) : Value(id) {} 
        RenderItemWithInstancesId Value;
    };

    struct RenderItemDesc {
        RenderItemDesc(const std::string& name, const XMFLOAT4X3& transform, const uint8* vertices, uint32 verticesCount, const uint8* indices, uint32 indicesCount, Texture texture) :
            name(name),
            transform(transform),
            vertices(vertices),
            verticesCount(verticesCount),
            indices(indices),
            indicesCount(indicesCount),
            texture(texture)
        {}

        const std::string name;
        const XMFLOAT4X3 transform;
        const uint8* vertices;
        const uint32 verticesCount;
        const uint8* indices;
        const uint32 indicesCount;
        Texture texture;
    };
    struct RenderItemInstanceDesc {
        RenderItemInstanceDesc() {}
        RenderItemInstanceDesc(XMFLOAT4X3& transform, Material material) : transform(transform), material(material) {}
        XMFLOAT4X3 transform;
        Material material;
    };
    struct RenderItemWithInstancesDesc : RenderItemDesc {
        RenderItemWithInstancesDesc(
            const std::string& name,
            const uint8* vertices,
            uint32 verticesCount,
            const uint8* indices,
            uint32 indicesCount,
            Texture texture,
            const XMFLOAT4X3& transform,
            PrimitiveTopology::Type primitiveTopology,
            std::unique_ptr<RenderItemInstanceDesc[]> instances,
            uint32 instancesCount
            ) :
            RenderItemDesc(name, transform, vertices, verticesCount, indices, indicesCount, texture),
            primitiveTopology(primitiveTopology), instances(move(instances)), instancesCount(instancesCount)
        {}
        const PrimitiveTopology::Type primitiveTopology;
        std::unique_ptr<RenderItemInstanceDesc[]> instances;
        const uint32 instancesCount;
    };
    struct RenderItemVerticesDesc : RenderItemDesc {
        RenderItemVerticesDesc(const std::string& name, const uint8* vertices, uint32 verticesCount, const uint8* indices, uint32 indicesCount, const XMFLOAT4X3& transform, Material material, Texture texture, PrimitiveTopology::Type primitiveTopology) :
            RenderItemDesc(name, transform, vertices, verticesCount, indices, indicesCount, texture),
            material(material), primitiveTopology(primitiveTopology)
        {}

        Material material;
        const PrimitiveTopology::Type primitiveTopology;
    };
    struct RenderItemTypeDesc {
        RenderItemTypeDesc(const std::string& name, const PredefinedGeometryType type, const XMFLOAT4X3& transform, Material material, PrimitiveTopology::Type primitiveTopology) :
            name(name),
            transform(transform),
            primitiveTopology(primitiveTopology),
            material(material),
            type(type)
        {}

        const std::string name;
        const XMFLOAT4X3 transform;
        const PrimitiveTopology::Type primitiveTopology;
        const Material material;
        const Texture texture;
        const PredefinedGeometryType type;
    }; 
    struct RenderItemParticlesDesc : RenderItemDesc {
        RenderItemParticlesDesc(const std::string& name, const uint8* vertices, uint32 verticesCount, const XMFLOAT4X3& transform, Material material, Texture texture, float particleSize) :
            RenderItemDesc(name, transform, vertices, verticesCount, nullptr, 0, texture),
            material(material),
            particleSize(particleSize)
        {}

        const Material material;
        const float particleSize;
    };
    using DescsInstanced = std::vector<RenderItemWithInstancesDesc>;
    using DescsVertices = std::vector<RenderItemVerticesDesc>;
    using DescsTypes = std::vector<RenderItemTypeDesc>;
    using DescsParticles = std::vector<RenderItemParticlesDesc>;

    class Viewport : Common::NonCopyable
    {
    public:
        Viewport(HWND hWnd);
        ~Viewport();

        void UpdateCamera(const XMFLOAT4X3& transform);

        HWND GetHwnd() const { return hwnd_; }
        uint32 GetWidth() const { return width_; }
        uint32 GetHeight() const { return height_; }

        Material CreateMaterial(MaterialType::Type material, const std::string& name);
        void DestroyMaterial(Material material);

        void SetTextureRootDirectory(const std::wstring& rootDirectory);
        Texture CreateTextureFromFile(const std::wstring& fileTitle);
        Texture CreateTextureFromMemory(const std::wstring& title, const Common::Dynarray<uint8>& data);
        Texture CreateTextureFromHandmadeData(const std::wstring& title, uint32 width, uint32 height, ResourceFormat::Type format, const void* data, bool forceRecreation);
        void DestroyTexture(Texture texture);

        RenderItemId CreateRenderItemOpaque(const DescsVertices& desc, uint32 vertexSize);
        RenderItemId CreateRenderItemOpaque(const DescsTypes& desc);
        RenderItemId CreateRenderItemTransparent(const DescsVertices& desc, uint32 vertexSize);
        RenderItemId CreateRenderItemTransparent(const DescsTypes& desc);
        RenderItemParticlesId CreateRenderItemParticles(const DescsParticles& desc, uint32 vertexSize);
        RenderItemWithInstancesId CreateRenderItemOpaqueWithInstances(const RenderItemWithInstancesDesc& viewportWithInstancesDesc, uint32 vertexSize);
        void DestroyRenderItemOpaque(const StructRenderItemId& id);
        void DestroyRenderItemTransparent(const StructRenderItemId& id);
        void DestroyRenderItem(const StructRenderItemParticlesId& id);
        void DestroyRenderItemOpaqueWithInstances(const StructRenderItemWithInstancesId& id);

        void UpdateRenderSubItemTransform(const StructRenderItemId& id, const std::string& name, const XMFLOAT4X3& transform);
        void UpdateRenderSubItemTransform(const StructRenderItemParticlesId& id, const std::string& name, const XMFLOAT4X3& transform);
        void UpdateRenderWithInstancesTransforms(const StructRenderItemWithInstancesId& id, const XMFLOAT4X3& transform, const XMFLOAT4X3* instancesTransforms);
        void UpdateRenderSubItemVertexData(const StructRenderItemId& id, const std::string& name, const uint8* data);
        void UpdateRenderSubItemVertexData(const StructRenderItemParticlesId& id, const std::string& name, const uint8* data);

        void BeforeDraw();
        void AfterDraw();
        void BeforeOpaque();
        void BeforeOpaqueWithInstances();
        void BeforeLine();
        void BeforeTransparent();
        void BeforeParticles();

        void DrawRenderItemsOpaque();
        void DrawRenderItemsOpaqueWithInstances();
        void DrawRenderItemsTransparent();
        void DrawRenderItemsParticles();

        void DrawReferenceFrame();
        void DrawGrating();

    private:
        void PrepareGeometry();
        void PrepareRootSignatures();
        void PreparePsos();
        XMVECTOR Convert2DTo3D(uint32 x, uint32 y) const;

        enum class RootSignatureType { kLighting, kLightingWithInstances, kColor, kParticles, kSize };
        void SetRootSignature(RootSignatureType type);

        enum class PsoType { kOpaque, kOpaqueWithInstances, kTransparent, kLine, kParticles, kSize };
        void SetPso(PsoType type);

        template <class Descs>
        auto CreateRenderItemInternal(const Descs& descs, uint32 vertexSize);
        grRenderItem CreateRenderItemInternal(const DescsTypes& descs);
        grRenderItemWithInstances CreateRenderItemInternal(const RenderItemWithInstancesDesc& descs, uint32 vertexSize);

    private:
        HWND hwnd_;
        uint32 width_;
        uint32 height_;

        std::list<grRenderItem> renderItemsOpaque_;
        std::list<grRenderItem> renderItemsTransparent_;
        std::list<grRenderItemParticles> renderItemsParticles_;
        std::list<grRenderItemWithInstances> renderItemsWithInstances_;
        std::array<std::pair<std::vector<VertexNormalTex>, std::vector<uint16>>, (size_t)PredefinedGeometryType::kSize> geometries_;

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

        std::unique_ptr<MaterialRaii> matRed_;
        std::unique_ptr<MaterialRaii> matGreen_;
        std::unique_ptr<MaterialRaii> matBlue_;
        std::unique_ptr<MaterialRaii> matDummy_;
    };
}
