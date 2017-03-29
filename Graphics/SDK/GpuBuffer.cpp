#include "stdafx.h"
#include "GpuBuffer.h"

#include "SDK\CommandContext.h"

namespace Graphics
{
    extern ComPtr<ID3D12Device> g_device;

    GpuBuffer::~GpuBuffer() {}

    void GpuBuffer::Create(
        const std::wstring& name,
        const uint_t elementsCount,
        const uint_t elementSize,
        const void* data) 
    {
        elementsCount_ = elementsCount;
        elementSize_ = elementSize;
        uint_t byteSize = elementsCount * elementSize;

        currentState_ = D3D12_RESOURCE_STATE_COPY_DEST;
        THROW_IF_FAILED(g_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
            currentState_,
            nullptr,
            IID_PPV_ARGS(resource_.GetAddressOf())));
        resource_->SetName(name.c_str());

        if (data) {
            ComPtr<ID3D12Resource> uploadBuffer;
            THROW_IF_FAILED(g_device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(uploadBuffer.GetAddressOf())));
            uploadBuffer->SetName((name + L" (upload)").c_str());

            D3D12_SUBRESOURCE_DATA subResourceData = {};
            subResourceData.pData = data;
            subResourceData.RowPitch = byteSize;
            subResourceData.SlicePitch = subResourceData.RowPitch;

            auto cc = CommandContext::Start(D3D12_COMMAND_LIST_TYPE_COPY);
            auto commandList = cc->GetCommandList();
            UpdateSubresources<1>(commandList, resource_.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
            cc->Finish(true);

            auto ccTransition = CommandContext::Start(D3D12_COMMAND_LIST_TYPE_DIRECT);
            ccTransition->TransitionResourceBegin(*this, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
            ccTransition->FlushResourceBarriers();
            ccTransition->Finish(false);
        }
    }

}