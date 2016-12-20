#include "stdafx.h"
#include "RenderItem.h"

namespace Graphics
{
    D3D12_VERTEX_BUFFER_VIEW RenderItem::VertexBufferView() const {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
        vbv.StrideInBytes = vbStride_;
        vbv.SizeInBytes = vbByteSize_;
        return vbv;
    }

    D3D12_INDEX_BUFFER_VIEW RenderItem::IndexBufferView() const {
        D3D12_INDEX_BUFFER_VIEW ibv;
        ibv.BufferLocation = indexBuffer_.GetGPUVirtualAddress();
        ibv.Format = (DXGI_FORMAT)ibFormat_;
        ibv.SizeInBytes = ibByteSize_;
        return ibv;
    }
}