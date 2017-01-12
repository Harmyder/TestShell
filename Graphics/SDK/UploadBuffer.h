#pragma once

namespace Graphics
{
    template<class E>
    class UploadBuffer
    {
        friend class GraphicsCore;
    public:
        UploadBuffer(std::wstring name, uint32 elementCount, bool isConstantBuffer);

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
    };

    template<class E>
    class ConstantBuffer : public UploadBuffer<E> {
    public:
        ConstantBuffer(std::wstring name, uint32 elementCount) :
            UploadBuffer(name, elementCount, true) {}
    };

    void CreateUploadBuffer(ID3D12Resource** ub, std::wstring name, uint32 size, BYTE** data);

    template<class E>
    UploadBuffer<E>::UploadBuffer(std::wstring name, uint32 elementCount, bool isConstantBuffer) : isConstantBuffer_(isConstantBuffer) {
        const uint32 bufferByteSize = (uint32)(isConstantBuffer_ ? Utility::CalcConstBufSize(sizeof(E)) : sizeof(E));

        CreateUploadBuffer(uploadBuffer_.GetAddressOf(), name, bufferByteSize * elementCount, &mappedData_);
    }

    template<class E>
    UploadBuffer<E>::~UploadBuffer() {
        if (uploadBuffer_ != nullptr)
            uploadBuffer_->Unmap(0, nullptr);

        mappedData_ = nullptr;
    }

    template<class E>
    void UploadBuffer<E>::CopyData(int elementIndex, const void* data) {
        const uint_t bufferByteSize = isConstantBuffer_ ? Utility::CalcConstBufSize(sizeof(E)) : sizeof(E);

        memcpy(&mappedData_[elementIndex * bufferByteSize], data, sizeof(E));
    }
}