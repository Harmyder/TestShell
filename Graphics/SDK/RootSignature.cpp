#include "stdafx.h"
#include "SDK\RootSignature.h"

namespace Graphics
{
    extern ComPtr<ID3D12Device> g_device;

    void RootSignature::Finalize() {
        CD3DX12_DESCRIPTOR_RANGE cbvTable0;
        CD3DX12_DESCRIPTOR_RANGE cbvTable1;
        cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
        cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

        CD3DX12_ROOT_PARAMETER slotRootParameter[3];
        slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0, D3D12_SHADER_VISIBILITY_ALL);
        slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1, D3D12_SHADER_VISIBILITY_ALL);
        slotRootParameter[2].InitAsShaderResourceView(0, 0, D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter, 0, nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> serializedRootSig = nullptr;
        ComPtr<ID3DBlob> errorBlob = nullptr;
        HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
            serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

        if (errorBlob != nullptr) OutputDebugString((char*)errorBlob->GetBufferPointer());
        THROW_IF_FAILED(hr);

        THROW_IF_FAILED(g_device->CreateRootSignature(
            0,
            serializedRootSig->GetBufferPointer(),
            serializedRootSig->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature_)));
    }
}
