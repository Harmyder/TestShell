#include "stdafx.h"
#include "Core\GraphicsCore.h"

#include <DirectXColors.h>

#include "SDK\CommandQueue.h"
#include "Core\FrameResource.h"
#include "SDK\UploadBuffer.h"
#include "Consts.h"
#include "SDK\CommandContext.h"
#include "Core\RenderItem.h"

#include "Shaders\Compiled\pixel.h"
#include "Shaders\Compiled\vertex.h"

using namespace std;

namespace Graphics
{
    DEFINE_SINGLETON(GraphicsCore);

    ComPtr<ID3D12Device> g_device;
    const uint32 g_frameResourcesCount = 3;

    const uint32 kGpuAllocatorPageSize = 0x10000;	// 64K. Resource heap must be a multiple of 64K

    static DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
    static DXGI_FORMAT DEPTH_STENCIL_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;

    GraphicsCore::~GraphicsCore() {}

    void GraphicsCore::Initialize(HWND hwnd)
    {
        screenViewport_ = make_unique<D3D12_VIEWPORT>();
        hwnd_ = hwnd;

#if _DEBUG
        Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)))) {
            //Direct3D will enable extra debugging and send debug messages to the VC++ output window
            debugInterface->EnableDebugLayer();
        }
        else {
            OutputDebugString("WARNING:  Unable to enable D3D12 debug validation layer.\n");
        }
#endif

        THROW_IF_FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&g_device)));
        THROW_IF_FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgiFactory_)));

        rtvDescriptorSize_ = g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        dsvDescriptorSize_ = g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        cbvSrvUavDescriptorSize_ = g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        commandQueue_ = make_unique<CommandQueue>();
        commandQueue_->Create(g_device.Get());
        commandContext_ = make_unique<CommandContext>(commandQueue_.get());
        commandContext_->Initialize();

        CreateSwapChain();
        CreateRtvDsvHeaps();
        Resize();

        CreateRootSignature();
        CreateInputLayout();
        CreateFrameResources();
        CreateDescriptorHeaps();
        CreateConstantBufferViews();
        CreatePSO();

        commandContext_->Flush(true);
    }

    void GraphicsCore::CreateSwapChain() {
        RECT rc;
        GetClientRect(hwnd_, &rc);

        DXGI_SWAP_CHAIN_DESC sd;
        sd.BufferDesc.Width = rc.right - rc.left;
        sd.BufferDesc.Height = rc.bottom - rc.top;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format = BACK_BUFFER_FORMAT;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = SWAP_CHAIN_BUFFERS_COUNT;
        sd.OutputWindow = hwnd_;
        sd.Windowed = true;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        THROW_IF_FAILED(dxgiFactory_->CreateSwapChain(
            commandQueue_->GetCommandQueue(),
            &sd,
            swapChain_.GetAddressOf()));
    }

    void GraphicsCore::CreateRtvDsvHeaps() {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
        rtvHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFERS_COUNT;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NodeMask = 0;
        THROW_IF_FAILED(g_device->CreateDescriptorHeap(
            &rtvHeapDesc, IID_PPV_ARGS(rtvHeap_.GetAddressOf())));


        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        dsvHeapDesc.NodeMask = 0;
        THROW_IF_FAILED(g_device->CreateDescriptorHeap(
            &dsvHeapDesc, IID_PPV_ARGS(dsvHeap_.GetAddressOf())));
    }

    void GraphicsCore::CreateRenderTargetViews() {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap_->GetCPUDescriptorHandleForHeapStart());
        for (UINT i = 0; i < SWAP_CHAIN_BUFFERS_COUNT; i++)
        {
            THROW_IF_FAILED(swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainBuffer_[i])));
            g_device->CreateRenderTargetView(swapChainBuffer_[i].Get(), nullptr, rtvHeapHandle);
            rtvHeapHandle.Offset(1, rtvDescriptorSize_);
        }
    }

    void GraphicsCore::CreateDepthStencil(uint32 width, uint32 height) {
        D3D12_RESOURCE_DESC depthStencilDesc;
        depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthStencilDesc.Alignment = 0;
        depthStencilDesc.Width = width;
        depthStencilDesc.Height = height;
        depthStencilDesc.DepthOrArraySize = 1;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.Format = DEPTH_STENCIL_FORMAT;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
        depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE optClear;
        optClear.Format = DEPTH_STENCIL_FORMAT;
        optClear.DepthStencil.Depth = 1.0f;
        optClear.DepthStencil.Stencil = 0;
        THROW_IF_FAILED(g_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &depthStencilDesc,
            D3D12_RESOURCE_STATE_COMMON,
            &optClear,
            IID_PPV_ARGS(depthStencilBuffer_.GetAddressOf())));

        g_device->CreateDepthStencilView(depthStencilBuffer_.Get(), nullptr, DepthStencilView());
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GraphicsCore::DepthStencilView() { return dsvHeap_->GetCPUDescriptorHandleForHeapStart(); }

    ID3D12Resource* GraphicsCore::CurrentBackBuffer() { return swapChainBuffer_[currentBackBuffer_].Get(); }

    CD3DX12_CPU_DESCRIPTOR_HANDLE GraphicsCore::CurrentBackBufferView() {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(
            rtvHeap_->GetCPUDescriptorHandleForHeapStart(),
            currentBackBuffer_,
            rtvDescriptorSize_);
    }

    void GraphicsCore::CreateRootSignature() {
        CD3DX12_DESCRIPTOR_RANGE cbvTable0;
        cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
        CD3DX12_DESCRIPTOR_RANGE cbvTable1;
        cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

        CD3DX12_ROOT_PARAMETER slotRootParameter[2];
        slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
        slotRootParameter[1].InitAsDescriptorTable(2, &cbvTable1);

        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
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

    void GraphicsCore::CreateInputLayout() {
        inputLayout_ =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };
    }

    void GraphicsCore::CreatePSO() {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

        // PSO for opaque objects.
        ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
        opaquePsoDesc.InputLayout = { inputLayout_.data(), (UINT)inputLayout_.size() };
        opaquePsoDesc.pRootSignature = rootSignature_.Get();
        opaquePsoDesc.VS = { g_shvertex, sizeof(g_shvertex) };
        opaquePsoDesc.PS = { g_shpixel, sizeof(g_shpixel) };
        opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
        opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        opaquePsoDesc.SampleMask = UINT_MAX;
        opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        opaquePsoDesc.NumRenderTargets = 1;
        opaquePsoDesc.RTVFormats[0] = BACK_BUFFER_FORMAT;
        opaquePsoDesc.SampleDesc.Count = 1;
        opaquePsoDesc.SampleDesc.Quality = 0;
        opaquePsoDesc.DSVFormat = DEPTH_STENCIL_FORMAT;
        THROW_IF_FAILED(g_device->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&psos_["opaque"])));

        // PSO for opaque wireframe objects.
        D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = opaquePsoDesc;
        opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
        THROW_IF_FAILED(g_device->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&psos_["opaque_wireframe"])));
    }

    void GraphicsCore::CreateFrameResources() {
        for (int i = 0; i < g_frameResourcesCount; ++i) {
            frameResources_.push_back(make_unique<FrameResource>(kPassesCountAllowed, kSceneObjectsCountAllowed));
        }
    }

    void GraphicsCore::CreateDescriptorHeaps() {
        UINT numDescriptors = (kSceneObjectsCountAllowed + kPassesCountAllowed) * g_frameResourcesCount;

        passCbvOffset_ = kSceneObjectsCountAllowed * g_frameResourcesCount;

        D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
        cbvHeapDesc.NumDescriptors = numDescriptors;
        cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        cbvHeapDesc.NodeMask = 0;
        THROW_IF_FAILED(g_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(cbvHeap_.GetAddressOf())));
    }

    void GraphicsCore::CreateConstantBufferViews()
    {
        UINT objCBByteSize = Utility::CalcConstBufSize(sizeof(PerObjConsts));

        for (int frameIndex = 0; frameIndex < g_frameResourcesCount; ++frameIndex)
        {
            auto objCB = frameResources_[frameIndex]->objCB->Resource();
            for (UINT i = 0; i < kSceneObjectsCountAllowed; ++i)
            {
                D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objCB->GetGPUVirtualAddress();

                cbAddress += i * objCBByteSize;

                int heapIndex = frameIndex * kSceneObjectsCountAllowed + i;
                auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHeap_->GetCPUDescriptorHandleForHeapStart());
                handle.Offset(heapIndex, cbvSrvUavDescriptorSize_);

                D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
                cbvDesc.BufferLocation = cbAddress;
                cbvDesc.SizeInBytes = objCBByteSize;

                g_device->CreateConstantBufferView(&cbvDesc, handle);
            }
        }

        UINT passCBByteSize = Utility::CalcConstBufSize(sizeof(PerPassConsts));

        for (int frameIndex = 0; frameIndex < g_frameResourcesCount; ++frameIndex)
        {
            auto passCB = frameResources_[frameIndex]->passCB->Resource();
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

            // Offset to the pass cbv in the descriptor heap.
            int heapIndex = passCbvOffset_ + frameIndex;
            auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHeap_->GetCPUDescriptorHandleForHeapStart());
            handle.Offset(heapIndex, cbvSrvUavDescriptorSize_);

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = passCBByteSize;

            g_device->CreateConstantBufferView(&cbvDesc, handle);
        }
    }

    void GraphicsCore::Resize() {
        commandContext_->Flush(true);
        commandContext_->Reset();
        auto commandList = commandContext_->GetCommandList();

        RECT rc;
        GetClientRect(hwnd_, &rc);
        uint32 width = rc.right - rc.left;
        uint32 height = rc.bottom - rc.top;

        for (int i = 0; i < SWAP_CHAIN_BUFFERS_COUNT; ++i)
            swapChainBuffer_[i].Reset();
        depthStencilBuffer_.Reset();

        THROW_IF_FAILED(swapChain_->ResizeBuffers(
            SWAP_CHAIN_BUFFERS_COUNT,
            width, height,
            BACK_BUFFER_FORMAT,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

        currentBackBuffer_ = 0;

        CreateRenderTargetViews();
        CreateDepthStencil(width, height);

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(depthStencilBuffer_.Get(),
            D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

        commandContext_->Flush(true);

        screenViewport_->TopLeftX = 0;
        screenViewport_->TopLeftY = 0;
        screenViewport_->Width = static_cast<float>(width);
        screenViewport_->Height = static_cast<float>(height);
        screenViewport_->MinDepth = 0.0f;
        screenViewport_->MaxDepth = 1.0f;

        scissorRect_ = D3D12_RECT{ 0, 0, static_cast<long>(width), static_cast<long>(height) };
    }

    void GraphicsCore::BeginScene() {
        commandContext_->Reset();
        auto commandList = commandContext_->GetCommandList();

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

        commandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
        commandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

        commandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

        ID3D12DescriptorHeap* descriptorHeaps[] = { cbvHeap_.Get() };
        commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        commandList->SetGraphicsRootSignature(rootSignature_.Get());
    }

    void GraphicsCore::EndScene() {
        auto commandList = commandContext_->GetCommandList();

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

        commandContext_->Flush(false);

        THROW_IF_FAILED(swapChain_->Present(0, 0));
        currentBackBuffer_ = (currentBackBuffer_ + 1) % SWAP_CHAIN_BUFFERS_COUNT;
    }

    void GraphicsCore::DrawRenderItem(const RenderItem& ri) {
        auto commandList = commandContext_->GetCommandList();

        commandList->IASetVertexBuffers(0, 1, &ri.VertexBufferView());
        commandList->IASetIndexBuffer(&ri.IndexBufferView());
        commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        commandList->SetGraphicsRootDescriptorTable(0, cbvHeap_->GetGPUDescriptorHandleForHeapStart());

        // I believe this must be done before calling SetDescriptorHeaps
        commandList->DrawIndexedInstanced(ri.IndicesCount(), 1, 0, 0, 0);
    }
}