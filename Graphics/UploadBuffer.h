#pragma once

namespace Graphics
{
    class UploadBuffer
    {
        friend class GraphicsCore;
    public:
        UploadBuffer(uint32 elementCount, uint_t elementByteSize);

        ~UploadBuffer();

        void CopyData(int elementIndex, const void* data);

    private:
        ID3D12Resource* Resource() const { return uploadBuffer_.Get(); }

    public:
        bool isConstantBuffer_ = false;

    private:
        ComPtr<ID3D12Resource> uploadBuffer_;
        BYTE* mappedData_ = nullptr;

        uint_t elementByteSize_ = 0;
    };

    class ConstantBuffer : public UploadBuffer {
    public:
        ConstantBuffer(uint32 elementCount, uint_t elementByteSize) : 
            UploadBuffer(elementCount, elementByteSize) {
            isConstantBuffer_ = true;
        }
    };
}