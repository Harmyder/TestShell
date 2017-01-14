#pragma once

namespace Graphics
{
    enum class RootSignatureType {
        kLighting = 1,
        kLightingWithInstances = 2,
        kColor = 4,
    };

    class RootSignature
    {
    public:
        ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }

        void SetType(RootSignatureType type) { type_ = type; }
        RootSignatureType GetType() { return type_; }

        void Finalize();

    private:
        RootSignatureType type_;

        ComPtr<ID3D12RootSignature> rootSignature_;
    };
}

