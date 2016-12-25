#pragma once
#include "stdafx.h"
#include "SDK\GpuBuffer.h"

namespace Graphics {
    class RenderItem
    {
    public:
        static void Create(const void* vertices, const uint_t vertexSize, const uint_t verticesCount, const XMFLOAT4X4& transform,
            CommandContext& commandContext_, RenderItem *&pri)
        {
            pri = new RenderItem();

            pri->vertexBuffer_.Create(L"ri_vertex", verticesCount, vertexSize, vertices, &commandContext_);
            pri->vbByteSize_ = (uint32)(verticesCount * vertexSize);
            pri->vbStride_ = (uint32)vertexSize;
            pri->verticesCount_ = (uint32)verticesCount;

            pri->SetTransform(transform);
        }

        uint32 VerticesCount() const { return verticesCount_; }
        D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;

        const XMFLOAT4X4& GetTransform() const { return transform_; }
        void SetTransform(const XMFLOAT4X4& transform);

        const bool IsDirty() const { return dirtyFramesCount_ != 0; }
        const void DecreaseDirtyFramesCount() { --dirtyFramesCount_; }

    private:
        XMFLOAT4X4 transform_;

        GpuBuffer vertexBuffer_;

        uint32 verticesCount_;
        uint32 vbByteSize_;
        uint32 vbStride_;

        uint32 dirtyFramesCount_;
    };

    class RenderIndexedItem : public RenderItem
    {
    public:
        static void Create(const void* vertices, const uint_t vertexSize, const uint_t verticesCount,
            const void* indices, const uint_t indexSize, const uint_t indicesCount, DXGI_FORMAT ibFormat,
            XMFLOAT4X4 transform, CommandContext& commandContext_, RenderIndexedItem *&prii)
        {
            prii = new RenderIndexedItem();

            RenderItem* pri = static_cast<RenderItem*>(prii);
            RenderItem::Create(vertices, vertexSize, verticesCount, transform, commandContext_, pri);

            prii->indicesCount_ = (uint32)indicesCount;
            prii->indexBuffer_.Create(L"ri_index", indicesCount, indexSize, indices, &commandContext_);
            prii->ibByteSize_ = (uint32)(indicesCount * indexSize);
            prii->ibFormat_ = ibFormat;
        }

        uint32 IndicesCount() const { return indicesCount_; }
        D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
        D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;

    private:
        GpuBuffer vertexBuffer_;
        GpuBuffer indexBuffer_;

        uint32 vbByteSize_;
        uint32 vbStride_;

        uint32 ibByteSize_;
        DXGI_FORMAT ibFormat_;

        uint32 indicesCount_;
    };
}
