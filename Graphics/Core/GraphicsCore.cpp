#include "stdafx.h"
#include "Core\GraphicsCore.h"

#include <DirectXColors.h>

#include "SDK\CommandQueue.h"
#include "Core\FrameResource.h"
#include "SDK\UploadBuffer.h"
#include "SDK\CommandContext.h"
#include "Core\RenderItem.h"
#include "Core\Lighting.h"

#include "Shaders\Compiled\pixel.h"
#include "Shaders\Compiled\vertex.h"

using namespace std;
using namespace DirectX;

namespace Graphics
{
    DEFINE_SINGLETON(GraphicsCore);

    ComPtr<ID3D12Device> g_device;

    const uint32 kGpuAllocatorPageSize = 0x10000;	// 64K. Resource heap must be a multiple of 64K

    static DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
    static DXGI_FORMAT DEPTH_STENCIL_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;

    GraphicsCore::~GraphicsCore() {}

    void GraphicsCore::Initialize(HWND hwnd, const InitParams& ip)
    {
        screenViewport_ = make_unique<D3D12_VIEWPORT>();
        hwnd_ = hwnd;

        freePerObjCbIndices_ = make_unique<FreeIndices>(ip.SceneObjectsCountLimit);
        freeMaterialCbIndices_ = make_unique<FreeIndices>(ip.MaterialsCountLimit);

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

        frameResources_ = make_unique<FrameResources>(ip.FrameResourcesCount,
            ip.PassesCountLimit,
            ip.SceneObjectsCountLimit,
            ip.MaterialsCountLimit);
        materialsUpdated_.resize(frameResources_->MatsCountLimit, kNotUpdated);
        lightsHolder_ = make_unique<LightsHolder>();

        CreateRootSignature();
        CreateInputLayout();
        CreateDescriptorHeaps();
        CreateConstantBufferViews();
        CreatePSO();

        commandContext_->Flush(true);
    }

    uint_t GraphicsCore::GetFrameResourcesCount() const { return frameResources_->Count(); }

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
        CD3DX12_DESCRIPTOR_RANGE cbvTable1;
        CD3DX12_DESCRIPTOR_RANGE cbvTable2;
        cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
        cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
        cbvTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);

        CD3DX12_ROOT_PARAMETER slotRootParameter[3];
        slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0, D3D12_SHADER_VISIBILITY_ALL);
        slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1, D3D12_SHADER_VISIBILITY_ALL);
        slotRootParameter[2].InitAsDescriptorTable(1, &cbvTable2, D3D12_SHADER_VISIBILITY_ALL);

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

    void GraphicsCore::CreateInputLayout() {
        inputLayout_ =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
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

        // PSO for opaque HUD objects.
        D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueHudPsoDesc = opaquePsoDesc;
        opaqueHudPsoDesc.DepthStencilState.DepthEnable = false;
        THROW_IF_FAILED(g_device->CreateGraphicsPipelineState(&opaqueHudPsoDesc, IID_PPV_ARGS(&psos_["opaque_HUD"])));
    }

    void GraphicsCore::CreateDescriptorHeaps() {
        UINT numDescriptors = (frameResources_->ObjsCountLimit + frameResources_->PassesCountLimit + frameResources_->MatsCountLimit) * (uint32)frameResources_->Count();

        passCbvOffset_ = frameResources_->ObjsCountLimit * (uint32)frameResources_->Count();
        matCbvOffset_ = (frameResources_->ObjsCountLimit + frameResources_->PassesCountLimit) * (uint32)frameResources_->Count();

        D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
        cbvHeapDesc.NumDescriptors = numDescriptors;
        cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        cbvHeapDesc.NodeMask = 0;
        THROW_IF_FAILED(g_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(cbvHeap_.GetAddressOf())));
    }

    void GraphicsCore::CreateConstantBufferViews() {
        uint_t objCBByteSize = frameResources_->CalcObjCbSize();
        for (uint_t frameIndex = 0; frameIndex < frameResources_->Count(); ++frameIndex)
        {
            auto objCB = frameResources_->GetFrameResource(frameIndex).objCB->Resource();
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objCB->GetGPUVirtualAddress();
            for (UINT i = 0; i < frameResources_->ObjsCountLimit; ++i)
            {
                int heapIndex = (int)frameIndex * frameResources_->ObjsCountLimit + i;
                auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHeap_->GetCPUDescriptorHandleForHeapStart());
                handle.Offset(heapIndex, cbvSrvUavDescriptorSize_);

                D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
                cbvDesc.BufferLocation = cbAddress + i * objCBByteSize;
                cbvDesc.SizeInBytes = (uint32)objCBByteSize;

                g_device->CreateConstantBufferView(&cbvDesc, handle);
            }
        }

        uint_t passCBByteSize = frameResources_->CalcPassCbSize();
        for (uint_t frameIndex = 0; frameIndex < frameResources_->Count(); ++frameIndex)
        {
            auto passCB = frameResources_->GetFrameResource(frameIndex).passCB->Resource();
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

            // Offset to the pass cbv in the descriptor heap.
            int heapIndex = passCbvOffset_ + (uint32)frameIndex;
            auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHeap_->GetCPUDescriptorHandleForHeapStart());
            handle.Offset(heapIndex, cbvSrvUavDescriptorSize_);

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = (uint32)passCBByteSize;

            g_device->CreateConstantBufferView(&cbvDesc, handle);
        }

        uint_t matCBByteSize = frameResources_->CalcMatCbSize();
        for (uint_t frameIndex = 0; frameIndex < frameResources_->Count(); ++frameIndex)
        {
            auto matCB = frameResources_->GetFrameResource(frameIndex).matCB->Resource();
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = matCB->GetGPUVirtualAddress();
            for (UINT i = 0; i < frameResources_->MatsCountLimit; ++i)
            {
                int heapIndex = matCbvOffset_ + (uint32)frameIndex * frameResources_->MatsCountLimit + i;
                auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHeap_->GetCPUDescriptorHandleForHeapStart());
                handle.Offset(heapIndex, cbvSrvUavDescriptorSize_);

                D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
                cbvDesc.BufferLocation = cbAddress + i * matCBByteSize;
                cbvDesc.SizeInBytes = (uint32)matCBByteSize;

                g_device->CreateConstantBufferView(&cbvDesc, handle);
            }
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
        commandContext_->Reset();

        screenViewport_->TopLeftX = 0;
        screenViewport_->TopLeftY = 0;
        screenViewport_->Width = static_cast<float>(width);
        screenViewport_->Height = static_cast<float>(height);
        screenViewport_->MinDepth = 0.0f;
        screenViewport_->MaxDepth = 1.0f;

        scissorRect_ = D3D12_RECT{ 0, 0, static_cast<long>(width), static_cast<long>(height) };
    
        commandList->RSSetViewports(1, screenViewport_.get());
        commandList->RSSetScissorRects(1, &scissorRect_);
    }

    void GraphicsCore::Update() {
        camera_.Update();
        UpdatePassesCBs();
    }

    void GraphicsCore::UpdatePassesCBs() {
        const XMMATRIX& view = camera_.GetViewMatrix();
        const XMMATRIX& proj = camera_.GetProjMatrix();

        PerPassConsts pass;
        XMStoreFloat4x4(&pass.ViewProj, XMMatrixMultiply(proj, view));
        XMStoreFloat4x4(&pass.View, view);
        XMStoreFloat4x4(&pass.Proj, proj);
        XMStoreFloat3(&pass.EyePosW, camera_.GetEyePos());

        FillPerPassLights(*lightsHolder_, pass);

        auto currPassCB = frameResources_->GetFrameResource(currFrameResource_).passCB.get();
        currPassCB->CopyData(0, &pass);
    }

    void GraphicsCore::BeginScene() {
        commandContext_->Reset();
        auto commandList = commandContext_->GetCommandList();
        commandList->SetPipelineState(psos_["opaque"].Get());

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

        commandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
        commandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

        commandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

        ID3D12DescriptorHeap* descriptorHeaps[] = { cbvHeap_.Get() };
        commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        commandList->SetGraphicsRootSignature(rootSignature_.Get());
        commandList->RSSetViewports(1, screenViewport_.get());
        commandList->RSSetScissorRects(1, &scissorRect_);

        Update();
        int passCbvIndex = passCbvOffset_ + currFrameResource_;
        auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvHeap_->GetGPUDescriptorHandleForHeapStart());
        passCbvHandle.Offset(passCbvIndex, cbvSrvUavDescriptorSize_);
        commandList->SetGraphicsRootDescriptorTable(2, passCbvHandle);
    }

    void GraphicsCore::BeginHud() {
        auto commandList = commandContext_->GetCommandList();
        commandList->SetPipelineState(psos_["opaque_HUD"].Get());
    }

    void GraphicsCore::EndScene() {
        auto commandList = commandContext_->GetCommandList();

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

        commandContext_->Flush(true);

        THROW_IF_FAILED(swapChain_->Present(0, 0));
        currentBackBuffer_ = (currentBackBuffer_ + 1) % SWAP_CHAIN_BUFFERS_COUNT;

        fill(begin(materialsUpdated_), end(materialsUpdated_), kNotUpdated);
    }

    void GraphicsCore::DrawRenderSubItem(RenderItem& ri, const string& name) {
        auto& rsi = ri.FindSubItem(name);
        DrawRenderSubItemInternal(ri, rsi);
    }

    void GraphicsCore::DrawRenderSubItemInternal(const RenderItem& ri, RenderSubItem& rsi) {
        auto cbObjIndex = rsi.CbIndex() + currFrameResource_ * frameResources_->ObjsCountLimit;
        auto cbMatIndexBase = matCbvOffset_ + currFrameResource_ * frameResources_->MatsCountLimit;
        auto cbMatIndex = rsi.GetMaterial().CbIndex() + cbMatIndexBase;

        if (rsi.IsDirty()) {
            auto currObjectCB = frameResources_->GetFrameResource(currFrameResource_).objCB.get();
            PerObjConsts objConstants;
            objConstants.World = rsi.GetTransform();
            currObjectCB->CopyData(cbObjIndex, &objConstants);
            rsi.DecreaseDirtyFramesCount();
        }
        if (rsi.GetMaterial().IsDirty() && materialsUpdated_[cbMatIndex - cbMatIndexBase] == kNotUpdated) {
            materialsUpdated_[cbMatIndex - cbMatIndexBase] = kUpdated;
            auto currMaterialCB = frameResources_->GetFrameResource(currFrameResource_).matCB.get();
            PerMatConsts matConstants;
            auto& mat = rsi.GetMaterial();
            matConstants.Ambient = mat.GetAmbient();
            matConstants.Diffuse = mat.GetDiffuse();
            matConstants.Specular = mat.GetSpecular();
            matConstants.FresnelR0 = mat.GetFresnelR0();
            matConstants.Roughness = mat.GetRoughness();
            currMaterialCB->CopyData(cbMatIndex - cbMatIndexBase, &matConstants);
            mat.DecreaseDirtyFramesCount();
        }

        auto commandList = commandContext_->GetCommandList();

        commandList->IASetVertexBuffers(0, 1, &ri.VertexBufferView());
        commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        auto cbvHandleObj = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvHeap_->GetGPUDescriptorHandleForHeapStart());
        cbvHandleObj.Offset(cbObjIndex, cbvSrvUavDescriptorSize_);
        commandList->SetGraphicsRootDescriptorTable(0, cbvHandleObj);

        auto cbvHandleMat = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvHeap_->GetGPUDescriptorHandleForHeapStart());
        cbvHandleMat.Offset(cbMatIndex, cbvSrvUavDescriptorSize_);
        commandList->SetGraphicsRootDescriptorTable(1, cbvHandleMat);

        commandList->DrawInstanced(rsi.VerticesCount(), 1, rsi.BaseVertexLocation(), 0);
    }

    void GraphicsCore::DrawRenderItem(RenderItem& ri) {
        for (auto it = ri.GetSubItemsBegin(); it != ri.GetSubItemsEnd(); ++it) {
            auto& rsi = (*it).second;
            DrawRenderSubItemInternal(ri, rsi);
        }
    }
}