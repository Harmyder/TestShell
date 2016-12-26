#include "stdafx.h"
#include "RenderItem.h"

using namespace std;

namespace Graphics
{
    D3D12_VERTEX_BUFFER_VIEW RenderItem::VertexBufferView() const {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
        vbv.StrideInBytes = vertexByteStride_;
        vbv.SizeInBytes = vbByteSize_;
        return vbv;
    }
}