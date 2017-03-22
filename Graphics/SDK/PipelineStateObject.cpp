#include "stdafx.h"
#include "SDK\PipelineStateObject.h"

#include "SDK\RootSignature.h"

#include "Shaders\Compiled\lighting_Pixel.h"
#include "Shaders\Compiled\lighting_Vertex.h"
#include "Shaders\Compiled\lightingInstanced_Pixel.h"
#include "Shaders\Compiled\lightingInstanced_Vertex.h"
#include "Shaders\Compiled\color_Pixel.h"
#include "Shaders\Compiled\color_Vertex.h"

using namespace std;
using namespace DirectX;

namespace Graphics
{
    extern ComPtr<ID3D12Device> g_device;

    extern DXGI_FORMAT BACK_BUFFER_FORMAT;
    extern DXGI_FORMAT DEPTH_STENCIL_FORMAT;

    class VertexTraits
    {
    public:
        explicit VertexTraits(VertexType type) : type_(type) {}

        uint32 VertexSize() const {
            switch (type_) {
            case VertexType::kNormalTex:
                return 32;
            case VertexType::kColor:
                return 28;
            default:
                throw "Unknown vertex type";
            }
        }

        vector<D3D12_INPUT_ELEMENT_DESC> CreateInputLayout() const;

    private:
        VertexType type_;
    };

    vector<D3D12_INPUT_ELEMENT_DESC> VertexTraits::CreateInputLayout() const {
        vector<D3D12_INPUT_ELEMENT_DESC> il;
        switch (type_) {
        case VertexType::kNormalTex:
            il.reserve(3);
            il.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
            il.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
            il.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
            break;
        case VertexType::kColor:
            il.reserve(2);
            il.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
            il.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
            break;
        default:
            throw "Unknown vertex type";
        }
        return il;
    }

    void PipelineStateObject::Finalize() {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;

        VertexTraits vt(vertexType_);
        auto inputLayout_ = vt.CreateInputLayout();

        ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
        psoDesc.InputLayout = { inputLayout_.data(), (UINT)inputLayout_.size() };
        psoDesc.pRootSignature = rootSignature_->GetRootSignature();
        switch (shaderType_) {
        case ShaderType::kColor:
            psoDesc.VS = { gsh_color_Vertex, sizeof(gsh_color_Vertex) };
            psoDesc.PS = { gsh_color_Pixel, sizeof(gsh_color_Pixel) };
            break;
        case ShaderType::kLighting:
            psoDesc.VS = { gsh_lighting_Vertex, sizeof(gsh_lighting_Vertex) };
            psoDesc.PS = { gsh_lighting_Pixel, sizeof(gsh_lighting_Pixel) };
            break;
        case ShaderType::kLightingWithInstances:
            psoDesc.VS = { gsh_lightingInstanced_Vertex, sizeof(gsh_lightingInstanced_Vertex) };
            psoDesc.PS = { gsh_lightingInstanced_Pixel, sizeof(gsh_lightingInstanced_Pixel) };
            break;
        default:
            throw "Unknown vertex type";
        }
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.RasterizerState.FillMode = fillMode_;
        psoDesc.RasterizerState.FrontCounterClockwise = true;
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        if (blendEnable_) {
            psoDesc.DepthStencilState.DepthEnable = false;
            psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
            psoDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
            psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
            psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
            psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
            psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
            psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        }
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = depthEnable_;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = primitiveTopologyType_;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = BACK_BUFFER_FORMAT;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleDesc.Quality = 0;
        psoDesc.DSVFormat = DEPTH_STENCIL_FORMAT;
        THROW_IF_FAILED(g_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso_)));
    }

    uint32 PipelineStateObject::GetVertexSize() {
        return VertexTraits(vertexType_).VertexSize();
    }
}
