#include "stdafx.h"
#include "GpuBuffer.h"

#include "SDK\CommandContext.h"

namespace Graphics
{
    extern ComPtr<ID3D12Device> g_device;

    GpuBuffer::~GpuBuffer() {}

    D3D12_GPU_VIRTUAL_ADDRESS GpuBuffer::GetGPUVirtualAddress() const { return buffer_->GetGPUVirtualAddress(); }

    void GpuBuffer::Create(
        const std::string& name,
        const uint_t elementsCount,
        const uint_t elementSize,
        const void* data,
        CommandContext* commandContext) 
    {
        name;
        elementsCount_ = elementsCount;
        elementSize_ = elementSize;
        uint_t byteSize = elementsCount * elementSize;

        THROW_IF_FAILED(g_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(buffer_.GetAddressOf())));

        if (data) {
            THROW_IF_FAILED(g_device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(uploadBuffer_.GetAddressOf())));


            D3D12_SUBRESOURCE_DATA subResourceData = {};
            subResourceData.pData = data;
            subResourceData.RowPitch = byteSize;
            subResourceData.SlicePitch = subResourceData.RowPitch;

            auto commandList = commandContext->GetCommandList();
            commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer_.Get(),
                D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
            UpdateSubresources<1>(commandList, buffer_.Get(), uploadBuffer_.Get(), 0, 0, 1, &subResourceData);
            commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer_.Get(),
                D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

            // Keep uploadBuffer_ alive till copy has been executed.
        }
    }

    void GpuBuffer::ReleaseUpload() {
        uploadBuffer_ = nullptr;
    }

}