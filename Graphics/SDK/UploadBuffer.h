#pragma once

namespace Graphics
{
    class UploadBuffer
    {
        friend class GraphicsCore;
    public:
        UploadBuffer(std::wstring name, uint32 elementCount, uint_t elementByteSize, bool isConstantBuffer);

        ~UploadBuffer();

        void CopyData(int elementIndex, const void* data);
        uint_t ElementByteSize() { return elementByteSize_; }

    private:
        ID3D12Resource* Resource() const { return uploadBuffer_.Get(); }

    protected:
        bool isConstantBuffer_ = false;

    private:
        ComPtr<ID3D12Resource> uploadBuffer_;
        BYTE* mappedData_ = nullptr;

        uint_t elementByteSize_ = 0;
    };

    class ConstantBuffer : public UploadBuffer {
    public:
        ConstantBuffer(std::wstring name, uint32 elementCount, uint_t elementByteSize) :
            UploadBuffer(name, elementCount, elementByteSize, true) {}
    };
}