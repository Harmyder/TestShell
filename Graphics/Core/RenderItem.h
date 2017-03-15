#pragma once
#include "stdafx.h"
#include "SDK\GpuBuffer.h"
#include "Utility\BufferStuff.h"

namespace Graphics {

    class Material;

    struct RenderVerticesDesc
    {
        const uint8 *data;
        uint32 verticesCount;
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
            uint32 verticesCount,
            const XMFLOAT4X3& transform,
            uint32 objBufferIndex,
            const Material* material,
            D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
            const RenderItem& container);

        RenderSubItem(const RenderSubItem& other) = delete;

        uint32 BaseVertexLocation() const { return baseVertexLocation_; }
        uint32 VerticesCount() const { return verticesCount_; }
        
        const auto& GetTransform() const { return transform_; }
        void SetTransform(const XMFLOAT4X3& transform);

        uint32 GetMaterialIndex() const { return materialIndex_; }
        D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return primitiveTopology_; }

    private:
        uint32 baseVertexLocation_;
        uint32 verticesCount_;
        XMFLOAT4X3 transform_;
        uint32 materialIndex_;
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_;

        const RenderItem& container_;
    };

    class RenderItem
    {
    public:
        static void Create(
            const RenderItemDesc* itemsDescs, uint32 itemsDescsCount,
            const RenderVerticesDesc* verticesDescs, uint32 verticesDescsCount,
            const uint32* itemsToVertices,
            uint32 vertexSize,
            std::unique_ptr<RenderItem>& ri);

        D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
        uint32 VertexByteStride() const { return vertexSize_; }
        uint32 VertexBufferByteSize() const { return vbByteSize_; }

        using SubItems = std::unordered_map<std::string, RenderSubItem>;
        const SubItems::iterator GetSubItemsBegin() { return begin(subItems_); }
        const SubItems::iterator GetSubItemsEnd() { return end(subItems_); }
        RenderSubItem& FindSubItem(const std::string& name) { return subItems_.find(name)->second; }

    private:
        RenderItem(uint32 vertexSize, uint32 verticesCount) :
            vertexSize_(vertexSize),
            vbByteSize_(vertexSize * verticesCount)
        {}

    private:
        SubItems subItems_;
        GpuBuffer vertexBuffer_;
        const uint32 vertexSize_;
        const uint32 vbByteSize_;
    };
}
