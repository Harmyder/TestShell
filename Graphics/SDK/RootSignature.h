#pragma once

namespace Graphics
{
    class RootSignature
    {
    public:
        ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }

        void Finalize();

    private:
        ComPtr<ID3D12RootSignature> rootSignature_;
    };
}

