#include "stdafx.h"
#include "SDK\RootSignature.h"

#include "Shaders\consts.shared.h"

using namespace std;

namespace Graphics
{
#define QUOTE(x) #x
#define STR(x) QUOTE(x)

    int ParseRegisterIndex(const string& reg) {
        return atoi(reg.substr(1).c_str());
    }

    extern ComPtr<ID3D12Device> g_device;

    void RootSignature::Finalize() {
        uint32 rPerObject, rPerPass, rMaterialBuf, rInstanceBuf, kParamsCount;
        rPerObject = rPerPass = rMaterialBuf = rInstanceBuf = kParamsCount = (uint32)-1;
        switch (type_) {
        case RootSignatureType::kLighting:
            rPerObject   = ParseRegisterIndex(STR(REGISTER_L_CB_PER_OBJECT));
            rPerPass     = ParseRegisterIndex(STR(REGISTER_L_CB_PER_PASS));
            rMaterialBuf = ParseRegisterIndex(STR(REGISTER_L_SB_MATERIAL_DATA));
            kParamsCount = 3;
            break;
        case RootSignatureType::kLightingWithInstances:
            rPerObject   = ParseRegisterIndex(STR(REGISTER_LI_CB_PER_OBJECT));
            rPerPass     = ParseRegisterIndex(STR(REGISTER_LI_CB_PER_PASS));
            rMaterialBuf = ParseRegisterIndex(STR(REGISTER_LI_SB_MATERIAL_DATA));
            rInstanceBuf = ParseRegisterIndex(STR(REGISTER_LI_SB_INSTANCE_DATA));
            kParamsCount = 4;
            break;
        case RootSignatureType::kColor:
            rPerObject   = ParseRegisterIndex(STR(REGISTER_C_CB_PER_OBJECT));
            rPerPass     = ParseRegisterIndex(STR(REGISTER_C_CB_PER_PASS));
            kParamsCount = 2;
            break;
        default:
            throw "Unknown root signature type";
        }

        CD3DX12_DESCRIPTOR_RANGE cbvTable0;
        CD3DX12_DESCRIPTOR_RANGE cbvTable1;
        cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, rPerObject);
        cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, rPerPass);

        vector<CD3DX12_ROOT_PARAMETER> slotRootParameters(kParamsCount);
        slotRootParameters[0].InitAsDescriptorTable(1, &cbvTable0, D3D12_SHADER_VISIBILITY_ALL);
        slotRootParameters[1].InitAsDescriptorTable(1, &cbvTable1, D3D12_SHADER_VISIBILITY_ALL);
        if (type_ == RootSignatureType::kLighting || type_ == RootSignatureType::kLightingWithInstances) {
            slotRootParameters[2].InitAsShaderResourceView(rMaterialBuf, 0, D3D12_SHADER_VISIBILITY_PIXEL);
            if (type_ == RootSignatureType::kLightingWithInstances) {
                slotRootParameters[3].InitAsShaderResourceView(rInstanceBuf, 0, D3D12_SHADER_VISIBILITY_ALL);
            }
        }

        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(kParamsCount, slotRootParameters.data(), 0, nullptr,
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
