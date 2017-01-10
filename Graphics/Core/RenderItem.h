#pragma once
#include "stdafx.h"
#include "SDK\GpuBuffer.h"
#include "Core\GraphicsCore.h"

namespace Graphics {

    class Material;

    struct RenderVerticesDesc // keep in sync with grtRenderVertices
    {
        uint8 *data;
        uint32 verticesCount;
    };
    struct RenderItemDesc // keep in sync with grtRenderSubItemDesc
    {
        std::string& name;
        XMFLOAT4X4& transform;
        Material *material;
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology;
    };

    class RenderItem;
    
    class RenderSubItem {
    public:
        RenderSubItem(uint32 baseVertexLocation,
            uint32 verticesCount,
            const XMFLOAT4X4& transform,
            uint32 objBufferIndex,
            Material* material,
            D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
            const RenderItem& container);

        RenderSubItem(const RenderSubItem& other) = delete;

        uint32 BaseVertexLocation() const { return baseVertexLocation_; }
        uint32 VerticesCount() const { return verticesCount_; }
        
        uint32 BufferIndex() const { return objBufferIndex_; }
        const bool IsDirty() const { return dirtyFramesCount_ != 0; }
        const void DecreaseDirtyFramesCount() { --dirtyFramesCount_; }

        const XMFLOAT4X4& GetTransform() const { return transform_; }
        void SetTransform(const XMFLOAT4X4& transform) {
            transform_ = transform;
            dirtyFramesCount_ = (uint32)GraphicsCore::GetInstance().GetFrameResourcesCount();
        }

        uint32 GetMaterialIndex() const { return materialIndex_; }
        D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return primitiveTopology_; }

        const RenderItem& Container() const { return container_; }

    private:
        uint32 baseVertexLocation_;
        uint32 verticesCount_;
        uint32 dirtyFramesCount_;
        uint32 objBufferIndex_;
        XMFLOAT4X4 transform_;
        uint32 materialIndex_;
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_;

        const RenderItem& container_;
    };

    class RenderItem
    {
    public:
        static void Create(
            const std::vector<RenderItemDesc>& itemsDescs,
            const std::vector<RenderVerticesDesc>& verticesDescs,
            const std::vector<uint32> itemsToVertices,
            uint32 vertexSize,
            CommandContext& commandContext_,
            RenderItem *&pri);

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
