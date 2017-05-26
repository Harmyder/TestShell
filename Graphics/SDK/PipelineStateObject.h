#pragma once
#include "stdafx.h"

namespace Graphics
{
    enum class VertexType {
        kNormalTex,
        kColor,
        kParticles,
    };

    enum class ShaderType {
        kLighting,
        kLightingWithInstances,
        kColor,
        kParticles,
    };

    class RootSignature;

    class PipelineStateObject
    {
    public:
        void SetRootSignature(RootSignature& rootSignature) { rootSignature_ = &rootSignature; }
        void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type) { primitiveTopologyType_ = type; }
        void SetFillMode(D3D12_FILL_MODE mode) { fillMode_ = mode; }
        void SetCullMode(D3D12_CULL_MODE mode) { cullMode_ = mode; }
        void SetBlendEnable(bool enable) { blendEnable_ = enable; }
        void SetDepthEnable(bool enable) { depthEnable_ = enable; }
        void SetVertexType(VertexType vertexType) { vertexType_ = vertexType; }
        void SetShaderType(ShaderType shaderType) { shaderType_ = shaderType; }

        void Finalize();

        uint32 GetVertexSize();
        ID3D12PipelineState* GetPipelineStateObject() { return pso_.Get(); }

    private:
        RootSignature *rootSignature_;
        VertexType vertexType_;
        ShaderType shaderType_;
        bool blendEnable_;
        bool depthEnable_;
        D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType_;
        D3D12_FILL_MODE fillMode_;
        D3D12_CULL_MODE cullMode_;

        Microsoft::WRL::ComPtr<ID3D12PipelineState> pso_;
    };
}
