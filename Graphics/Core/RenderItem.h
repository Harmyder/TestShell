#pragma once
#include "stdafx.h"
#include "SDK\GpuBuffer.h"

namespace Graphics {
    class RenderItem
    {
    public:
        static void Create(const void* vertices, const uint_t vertexSize, const uint_t verticesCount,
            const void* indices, const uint_t indexSize, const uint_t indicesCount, DXGI_FORMAT ibFormat,
            CommandContext& commandContext_, RenderItem *&pri) {
            pri = new RenderItem();
            pri->indicesCount_ = (uint32)indicesCount;
         
            pri->vertexBuffer_.Create("", verticesCount, vertexSize, vertices, &commandContext_);
            pri->indexBuffer_.Create("", indicesCount, indexSize, indices, &commandContext_);

            pri->vbByteSize_ = (uint32)(verticesCount * vertexSize);
            pri->ibByteSize_ = (uint32)(indicesCount * indexSize);

            pri->vbStride_ = (uint32)vertexSize;
            pri->ibFormat_ = ibFormat;
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
