#include "stdafx.h"
#include "SDK/RootSignature.h"

#include "Shaders/consts.shared.h"

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
        uint32 rPerObject, rPerPass, rParticlesMeta, rMaterialBuf, rDiffuseTexture, rInstanceBuf, kParamsCount;
        rPerObject = rPerPass = rParticlesMeta = rMaterialBuf = rDiffuseTexture = rInstanceBuf = kParamsCount = (uint32)-1;
        switch (type_) {
        case RootSignatureType::kLighting:
            rPerObject      = ParseRegisterIndex(STR(REGISTER_L_CB_PER_OBJECT));
            rPerPass        = ParseRegisterIndex(STR(REGISTER_L_CB_PER_PASS));
            rDiffuseTexture = ParseRegisterIndex(STR(REGISTER_L_TB_DIFFUSE_MAP));
            rMaterialBuf    = ParseRegisterIndex(STR(REGISTER_L_TB_MATERIAL_DATA));
            kParamsCount = 4;
            break;
        case RootSignatureType::kLightingWithInstances:
            rPerObject      = ParseRegisterIndex(STR(REGISTER_LI_CB_PER_OBJECT));
            rPerPass        = ParseRegisterIndex(STR(REGISTER_LI_CB_PER_PASS));
            rDiffuseTexture = ParseRegisterIndex(STR(REGISTER_LI_TB_DIFFUSE_MAP));
            rMaterialBuf    = ParseRegisterIndex(STR(REGISTER_LI_TB_MATERIAL_DATA));
            rInstanceBuf    = ParseRegisterIndex(STR(REGISTER_LI_TB_INSTANCE_DATA));
            kParamsCount = 5;
            break;
        case RootSignatureType::kColor:
            rPerPass        = ParseRegisterIndex(STR(REGISTER_C_CB_PER_PASS));
            rPerObject      = ParseRegisterIndex(STR(REGISTER_C_CB_PER_OBJECT));
            kParamsCount = 2;
            break;
        case RootSignatureType::kParticles:
            rParticlesMeta  = ParseRegisterIndex(STR(REGISTER_P_CB_PARTICLES_META));
            rPerPass        = ParseRegisterIndex(STR(REGISTER_P_CB_PER_PASS));
            rDiffuseTexture = ParseRegisterIndex(STR(REGISTER_P_TB_DIFFUSE_MAP));
            rMaterialBuf    = ParseRegisterIndex(STR(REGISTER_P_TB_MATERIAL_DATA));
            kParamsCount = 4;
            break;
        default:
            throw "Unknown root signature type";
        }

        CD3DX12_DESCRIPTOR_RANGE cbvTablePass(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, rPerPass);;
        CD3DX12_DESCRIPTOR_RANGE cbvTableObj(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, rPerObject);
        CD3DX12_DESCRIPTOR_RANGE cbvTableParticles(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, rParticlesMeta);

        vector<CD3DX12_ROOT_PARAMETER> slotRootParameters(kParamsCount);
        if (type_ == RootSignatureType::kParticles) {
            slotRootParameters[0].InitAsDescriptorTable(1, &cbvTableParticles, D3D12_SHADER_VISIBILITY_ALL);
        }
        else {
            slotRootParameters[0].InitAsDescriptorTable(1, &cbvTableObj, D3D12_SHADER_VISIBILITY_ALL);
        }
        slotRootParameters[1].InitAsDescriptorTable(1, &cbvTablePass, D3D12_SHADER_VISIBILITY_ALL);
        CD3DX12_DESCRIPTOR_RANGE texTable;
        if (type_ == RootSignatureType::kLighting ||
            type_ == RootSignatureType::kLightingWithInstances ||
            type_ == RootSignatureType::kParticles) {
            assert(rDiffuseTexture != (uint32)-1 && rMaterialBuf != (uint32)-1);
            texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, rDiffuseTexture);
            slotRootParameters[2].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
            slotRootParameters[3].InitAsShaderResourceView(rMaterialBuf, 0, D3D12_SHADER_VISIBILITY_PIXEL);
            if (type_ == RootSignatureType::kLightingWithInstances) {
                assert(rInstanceBuf != (uint32)-1);
                slotRootParameters[4].InitAsShaderResourceView(rInstanceBuf, 0, D3D12_SHADER_VISIBILITY_ALL);
            }
        }

        const CD3DX12_STATIC_SAMPLER_DESC sampler(0, D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);

        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(kParamsCount, slotRootParameters.data(), 1, &sampler,
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
