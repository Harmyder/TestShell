#pragma once
#include "stdafx.h"
#include "SDK\GpuBuffer.h"
#include "Utility\BufferStuff.h"

namespace Graphics {

    class Material;
    class Texture;

    struct RenderVerticesDesc {
        const uint8 *vertices;
        uint32 verticesCount;
        const uint8 *indices;
        uint32 indicesCount;
    };
    struct RenderItemDescBase {
        const std::string& name;
        const XMFLOAT4X3& transform;
        const Texture *texture;
        const Material *material;
    };
    // Keep in sync with grtRenderSubItemDesc
    struct RenderItemDesc : RenderItemDescBase {
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology;
    };
    // Keep in sync with grtRenderSubItemParticlesDesc
    struct RenderItemParticlesDesc : RenderItemDescBase {
        float particleSize;
    };

    class RenderSubItemBase : public Utility::BufferEntryDirty
    {
    public:
        struct ConstructionData {
            uint32 baseVertexLocation;
            uint32 baseIndexLocation;
            uint32 verticesCount;
            uint32 indicesCount;
            const XMFLOAT4X3& transform;
            uint32 objBufferIndex;
            const Material* material;
            const Texture* const texture;
        };

        RenderSubItemBase(const ConstructionData& data);
        RenderSubItemBase(const RenderSubItemBase& other) = delete;

        uint32 BaseVertexLocation() const { return baseVertexLocation_; }
        uint32 VerticesCount() const { return verticesCount_; }
        
        bool HasIndices() const { return indicesCount_ != 0; }
        uint32 BaseIndexLocation() const { return baseIndexLocation_; }
        uint32 IndicesCount() const { return indicesCount_; }

        const auto& GetTransform() const { return transform_; }
        void SetTransform(const XMFLOAT4X3& transform);

        uint32 GetMaterialIndex() const { return materialIndex_; }
        const Texture* GetTexture() const { return texture_; }

    private:
        uint32 baseVertexLocation_;
        uint32 verticesCount_;
        uint32 baseIndexLocation_;
        uint32 indicesCount_;
        XMFLOAT4X3 transform_;
        uint32 materialIndex_;
        const Texture* texture_;
    };

    class RenderItem;
    class RenderItemParticles;

    class RenderSubItemParticles : public RenderSubItemBase
    {
    public:
        using Desc = RenderItemParticlesDesc;
        RenderSubItemParticles(const ConstructionData& data, float particleSize, const RenderItemParticles& container) :
            RenderSubItemBase(data),
            particleSize_(particleSize),
            container_(container)
        {}

        D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return D3D_PRIMITIVE_TOPOLOGY_POINTLIST; }
        float GetParticleSize() const { return particleSize_; }

    private:
        float particleSize_;
        const RenderItemParticles& container_;
    };

    class RenderSubItem : public RenderSubItemBase
    {
    public:
        using Desc = RenderItemDesc;
        RenderSubItem(const ConstructionData& data, D3D_PRIMITIVE_TOPOLOGY primitiveTopology, const RenderItem& container) :
            RenderSubItemBase(data),
            primitiveTopology_(primitiveTopology),
            container_(container)
        {}

        D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return primitiveTopology_; }

    private:
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_;
        const RenderItem& container_;
    };

    class RenderItemBase : Common::NonCopyable
    {
    public:
        enum { kIndexSize = 2 }; // Keep in sync with kIndexFormat

        D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
        bool HasIndexBuffer() const { return ibByteSize_ != 0; }
        D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;

    protected:
        RenderItemBase(uint32 vertexSize, uint32 verticesCount, uint32 indicesCount) :
            vertexSize_(vertexSize),
            vbByteSize_(vertexSize * verticesCount),
            ibByteSize_(kIndexSize * indicesCount)
        {}
        ~RenderItemBase() {}
        GpuBuffer& VertexBuffer() { return vertexBuffer_; }
        GpuBuffer& IndexBuffer() { return indexBuffer_; }

        void SetVertexData(const uint8* data, uint32 vertexOffset, uint32 verticesCount);

    protected:
        void CreateVertexBuffer(const RenderVerticesDesc* verticesDescs, uint32 verticesDescsCount, uint32 totalVerticesCount, uint32 vertexSize);

    private:
        enum { kIndexFormat = DXGI_FORMAT_R16_UINT };

        GpuBuffer vertexBuffer_;
        const uint32 vertexSize_;
        const uint32 vbByteSize_;
        GpuBuffer indexBuffer_;
        const uint32 ibByteSize_;
    };

    template <class RSI>
    class RenderItemBase2 : public RenderItemBase
    {
    public:
        using RenderItemBase::RenderItemBase;

        using SubItems = std::unordered_map<std::string, RSI>;
        const typename SubItems::iterator GetSubItemsBegin() { return begin(subItems_); }
        const typename SubItems::iterator GetSubItemsEnd() { return end(subItems_); }
        RSI& FindSubItem(const std::string& name) { return subItems_.find(name)->second; }

        void SetSubItemVertexData(const std::string& name, const uint8* data) {
            const RSI& rsi = subItems_.at(name);
            SetVertexData(data, rsi.BaseVertexLocation(), rsi.VerticesCount());
        }

    protected:
        template<class IndicesOffsets>
        void AddSubItems(
            const typename RSI::Desc* itemsDescs,
            uint32 itemsDescsCount,
            const RenderVerticesDesc* verticesDescs,
            const uint32* itemsToData,
            const std::vector<uint32>& verticesOffsets,
            const IndicesOffsets& indicesOffsets,
            Utility::FreeIndices& freeIndices);

    private:
        bool AddSubItem(const RenderSubItemBase::ConstructionData& data, const typename RSI::Desc& itemDesc);

    private:
        SubItems subItems_;
    };

    class RenderItem : public RenderItemBase2<RenderSubItem>
    {
    public:
        static void Create(
            const RenderItemDesc* itemsDescs, uint32 itemsDescsCount,
            const RenderVerticesDesc* verticesDescs, uint32 verticesDescsCount,
            const uint32* itemsToVertices,
            uint32 vertexSize,
            std::unique_ptr<RenderItem>& ri);

    private:
        RenderItem(uint32 vertexSize, uint32 verticesCount, uint32 indicesCount) :
            RenderItemBase2(vertexSize, verticesCount, indicesCount)
        {}
    };

    class RenderItemParticles : public RenderItemBase2<RenderSubItemParticles>
    {
    public:
        static void Create(
            const RenderItemParticlesDesc* itemsDescs, uint32 itemsDescsCount,
            const RenderVerticesDesc* verticesDescs, uint32 verticesDescsCount,
            const uint32* itemsToVertices,
            uint32 vertexSize,
            std::unique_ptr<RenderItemParticles>& ri);

    private:
        RenderItemParticles(uint32 vertexSize, uint32 verticesCount) :
            RenderItemBase2(vertexSize, verticesCount, 0)
        {}
    };
}
