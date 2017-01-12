#include "stdafx.h"
#include "UploadBuffer.h"

using namespace std;

namespace Graphics
{
    extern ComPtr<ID3D12Device> g_device;

    void CreateUploadBuffer(ID3D12Resource** ub, wstring name, uint32 size, BYTE** data) {
        THROW_IF_FAILED(g_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(size),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(ub)));
        (*ub)->SetName(name.c_str());

        THROW_IF_FAILED((*ub)->Map(0, nullptr, reinterpret_cast<void**>(data)));
    }

}
