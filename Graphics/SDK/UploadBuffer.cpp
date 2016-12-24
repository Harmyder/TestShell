#include "stdafx.h"
#include "UploadBuffer.h"

namespace Graphics
{
    extern ComPtr<ID3D12Device> g_device;

    UploadBuffer::UploadBuffer(std::wstring name, uint32 elementCount, uint_t elementByteSize, bool isConstantBuffer) : isConstantBuffer_(isConstantBuffer) {
        elementByteSize_ = elementByteSize;

        const uint_t bufferByteSize = isConstantBuffer_ ? Utility::CalcConstBufSize(elementByteSize_) : elementByteSize_;

        THROW_IF_FAILED(g_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(bufferByteSize * elementCount),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(uploadBuffer_.GetAddressOf())));
        uploadBuffer_->SetName(name.c_str());

        THROW_IF_FAILED(uploadBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_)));
    }

    UploadBuffer::~UploadBuffer() {
        if (uploadBuffer_ != nullptr)
            uploadBuffer_->Unmap(0, nullptr);

        mappedData_ = nullptr;
    }

    void UploadBuffer::CopyData(int elementIndex, const void* data) {
        const uint_t bufferByteSize = isConstantBuffer_ ? Utility::CalcConstBufSize(elementByteSize_) : elementByteSize_;

        memcpy(&mappedData_[elementIndex * bufferByteSize], data, elementByteSize_);
    }
}
