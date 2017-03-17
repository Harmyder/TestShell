#pragma once
#include "stdafx.h"
#include "SDK\GpuBuffer.h"
#include "Utility\BufferStuff.h"

namespace Graphics {

    class Material;

    struct RenderVerticesDesc
    {
        const uint8 *vertices;
        uint32 verticesCount;
        const uint8 *indices;
        uint32 indicesCount;
    };
    struct RenderItemDesc
    {
        const std::string& name;
        const XMFLOAT4X3& transform;
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology;
        const Material *material;
    };

    class RenderItem;
    
    class RenderSubItem : public Utility::BufferEntryDirty
    {
    public:
        RenderSubItem(uint32 baseVertexLocation,
            uint32 baseIndexLocation,
            int verticesCount,
            int indicesCount,
            const XMFLOAT4X3& transform,
            uint32 objBufferIndex,
            const Material* material,
            D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
            const RenderItem& container);

        RenderSubItem(const RenderSubItem& other) = delete;

        uint32 BaseVertexLocation() const { return baseVertexLocation_; }
        uint32 VerticesCount() const { return verticesCount_; }
        
        bool HasIndices() const { return indicesCount_ != 0; }
        uint32 BaseIndexLocation() const { return baseIndexLocation_; }
        uint32 IndicesCount() const { return indicesCount_; }

        const auto& GetTransform() const { return transform_; }
        void SetTransform(const XMFLOAT4X3& transform);

        uint32 GetMaterialIndex() const { return materialIndex_; }
        D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return primitiveTopology_; }

    private:
        uint32 baseVertexLocation_;
        uint32 verticesCount_;
        uint32 baseIndexLocation_;
        uint32 indicesCount_;
        XMFLOAT4X3 transform_;
        uint32 materialIndex_;
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_;

        const RenderItem& container_;
    };

    class RenderItem
    {
    public:
        enum { kIndexSize = 2 }; // Keep in sync with kIndexFormat

        static void Create(
            const RenderItemDesc* itemsDescs, uint32 itemsDescsCount,
            const RenderVerticesDesc* verticesDescs, uint32 verticesDescsCount,
            const uint32* itemsToVertices,
            uint32 vertexSize,
            std::unique_ptr<RenderItem>& ri);

        D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
        bool HasIndexBuffer() const { return ibByteSize_ != 0; }
        D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;

        using SubItems = std::unordered_map<std::string, RenderSubItem>;
        const SubItems::iterator GetSubItemsBegin() { return begin(subItems_); }
        const SubItems::iterator GetSubItemsEnd() { return end(subItems_); }
        RenderSubItem& FindSubItem(const std::string& name) { return subItems_.find(name)->second; }

    private:
        RenderItem(uint32 vertexSize, uint32 verticesCount, uint32 indicesCount) :
            vertexSize_(vertexSize),
            vbByteSize_(vertexSize * verticesCount),
            ibByteSize_(kIndexSize * indicesCount)
        {}

        enum { kIndexFormat = DXGI_FORMAT_R16_UINT };

    private:
        SubItems subItems_;
        GpuBuffer vertexBuffer_;
        const uint32 vertexSize_;
        const uint32 vbByteSize_;
        GpuBuffer indexBuffer_;
        const uint32 ibByteSize_;
    };
}
