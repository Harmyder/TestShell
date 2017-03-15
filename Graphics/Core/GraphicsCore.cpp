#include "stdafx.h"
#include "Core\GraphicsCore.h"

#include <DirectXColors.h>

#include "SDK\CommandQueue.h"
#include "Core\FrameResource.h"
#include "SDK\UploadBuffer.h"
#include "SDK\CommandContext.h"
#include "SDK\RootSignature.h"
#include "Core\RenderItem.h"
#include "Core\RenderItemWithInstances.h"
#include "Core\Lighting.h"
#include "Utility\BufferStuff.h"

using namespace std;
using namespace DirectX;

namespace Graphics
{
    DEFINE_SINGLETON(GraphicsCore);

    ComPtr<ID3D12Device> g_device;
    DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT DEPTH_STENCIL_FORMAT = DXGI_FORMAT_D32_FLOAT;

    GraphicsCore::~GraphicsCore() {}

    void GraphicsCore::Initialize(HWND hwnd, const InitParams& ip)
    {
        screenViewport_ = make_unique<D3D12_VIEWPORT>();
        hwnd_ = hwnd;

        freePerObjBufferIndices_ = make_unique<Utility::FreeIndices>(ip.SceneObjectsCountLimit);

#if _DEBUG
        Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)))) {
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
        CommandContext::PreInitialize(commandQueue_.get());

        CreateSwapChain();
        CreateRtvDsvHeaps();
        Resize();

        frameResources_ = make_unique<FrameResources>(ip.FrameResourcesCount,
            ip.PassesCountLimit,
            ip.SceneObjectsCountLimit,
            ip.MaterialsCountLimit,
            ip.InstancesCountLimit);
        materialsBuffer_ = make_unique<MaterialsBuffer>(ip.MaterialsCountLimit);
        lightsHolder_ = make_unique<LightsHolder>();

        CreateCbvDescriptorHeap();
        CreateConstantBufferViews();
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

    void GraphicsCore::Shutdown() {
        CommandContext::DestroyAllInstances();

#if defined(_DEBUG)
        ID3D12DebugDevice* debugInterface;
        THROW_IF_FAILED(g_device->QueryInterface(&debugInterface));
        debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
        debugInterface->Release();
#endif
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

    void GraphicsCore::CreateCbvDescriptorHeap() {
        UINT numDescriptors = (frameResources_->ObjsCountLimit + frameResources_->PassesCountLimit) * frameResources_->Count();

        passCbvOffset_ = frameResources_->ObjsCountLimit * frameResources_->Count();

        D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
        cbvHeapDesc.NumDescriptors = numDescriptors;
        cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        cbvHeapDesc.NodeMask = 0;
        THROW_IF_FAILED(g_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(cbvHeap_.GetAddressOf())));
    }

    void GraphicsCore::CreateConstantBufferViews() {
        uint_t objCBByteSize = frameResources_->CalcObjCbSize();
        for (uint32 frameIndex = 0; frameIndex < frameResources_->Count(); ++frameIndex)
        {
            auto objCB = frameResources_->GetFrameResource(frameIndex).objCB->Resource();
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objCB->GetGPUVirtualAddress();
            for (UINT i = 0; i < frameResources_->ObjsCountLimit; ++i)
            {
                uint32 heapIndex = frameIndex * frameResources_->ObjsCountLimit + i;
                auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHeap_->GetCPUDescriptorHandleForHeapStart());
                handle.Offset(heapIndex, cbvSrvUavDescriptorSize_);

                D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
                cbvDesc.BufferLocation = cbAddress + i * objCBByteSize;
                cbvDesc.SizeInBytes = (uint32)objCBByteSize;

                g_device->CreateConstantBufferView(&cbvDesc, handle);
            }
        }

        uint_t passCBByteSize = frameResources_->CalcPassCbSize();
        for (uint32 frameIndex = 0; frameIndex < frameResources_->Count(); ++frameIndex)
        {
            auto passCB = frameResources_->GetFrameResource(frameIndex).passCB->Resource();
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

            // Offset to the pass cbv in the descriptor heap.
            int heapIndex = passCbvOffset_ + frameIndex;
            auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHeap_->GetCPUDescriptorHandleForHeapStart());
            handle.Offset(heapIndex, cbvSrvUavDescriptorSize_);

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = (uint32)passCBByteSize;

            g_device->CreateConstantBufferView(&cbvDesc, handle);
        }
    }

    void GraphicsCore::Resize() {
        auto commandContext = CommandContext::Start();
        auto commandList = commandContext->GetCommandList();

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

        screenViewport_->TopLeftX = 0;
        screenViewport_->TopLeftY = 0;
        screenViewport_->Width = static_cast<float>(width);
        screenViewport_->Height = static_cast<float>(height);
        screenViewport_->MinDepth = 0.0f;
        screenViewport_->MaxDepth = 1.0f;

        scissorRect_ = D3D12_RECT{ 0, 0, static_cast<long>(width), static_cast<long>(height) };
    
        commandList->RSSetViewports(1, screenViewport_.get());
        commandList->RSSetScissorRects(1, &scissorRect_);

        commandContext->Finish(true);
    }

    void GraphicsCore::Update() {
        auto& current = frameResources_->GetCurrentFrameResource();
        commandQueue_->WaitForFence(current.Fence);

        camera_.Update();
        UpdatePassesCBs();
        UpdateMaterials();
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

        auto currPassCB = frameResources_->GetCurrentFrameResource().passCB.get();
        currPassCB->CopyData(0, &pass);
    }

    void GraphicsCore::UpdateMaterials() {
        for (auto& mat : *materialsBuffer_) {
            if (mat.IsDirty()) {
                auto currMaterialBuffer = frameResources_->GetCurrentFrameResource().matBuffer.get();
                PerMatConsts matConstants;
                matConstants.Ambient = mat.GetAmbient();
                matConstants.Diffuse = mat.GetDiffuse();
                matConstants.Specular = mat.GetSpecular();
                matConstants.FresnelR0 = mat.GetFresnelR0();
                matConstants.Roughness = mat.GetRoughness();
                currMaterialBuffer->CopyData((int)mat.BufferIndex(), &matConstants);
                mat.DecreaseDirtyFramesCount();
            }
        }
    }

    void GraphicsCore::SetGraphicsRoot(RootSignatureType type) {
        auto commandList = commandContext_->GetCommandList();

        int passCbvIndex = passCbvOffset_ + frameResources_->GetCurrentIndex();
        auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvHeap_->GetGPUDescriptorHandleForHeapStart());
        passCbvHandle.Offset(passCbvIndex, cbvSrvUavDescriptorSize_);
        commandList->SetGraphicsRootDescriptorTable(1, passCbvHandle);

        if (type != RootSignatureType::kColor) {
            auto matBuffer = frameResources_->GetCurrentFrameResource().matBuffer->Resource();
            commandList->SetGraphicsRootShaderResourceView(2, matBuffer->GetGPUVirtualAddress());
        }
    }

    void GraphicsCore::BeginScene() {
        commandContext_ = CommandContext::Start();
        auto commandList = commandContext_->GetCommandList();

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

        commandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
        commandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

        commandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

        ID3D12DescriptorHeap* descriptorHeaps[] = { cbvHeap_.Get() };
        commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        commandList->RSSetViewports(1, screenViewport_.get());
        commandList->RSSetScissorRects(1, &scissorRect_);

        Update();
    }

    void GraphicsCore::EndScene() {
        auto commandList = commandContext_->GetCommandList();

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

        frameResources_->GetCurrentFrameResource().Fence = commandContext_->Finish(false);

        THROW_IF_FAILED(swapChain_->Present(0, 0));
        currentBackBuffer_ = (currentBackBuffer_ + 1) % SWAP_CHAIN_BUFFERS_COUNT;
        frameResources_->AdvanceFrame();
    }

    void GraphicsCore::DrawRenderSubItem(RenderItem& ri, const string& name) {
        auto& rsi = ri.FindSubItem(name);
        DrawRenderSubItemInternal(ri, rsi);
    }

    void GraphicsCore::DrawRenderSubItemInternal(const RenderItem& ri, RenderSubItem& rsi) {
        if (rsi.IsDirty()) {
            auto currObjectCB = frameResources_->GetCurrentFrameResource().objCB.get();
            PerObjConsts objConstants;
            objConstants.World = rsi.GetTransform();
            objConstants.MaterialIndex = rsi.GetMaterialIndex();
            currObjectCB->CopyData(rsi.BufferIndex(), &objConstants);
            rsi.DecreaseDirtyFramesCount();
        }

        auto commandList = commandContext_->GetCommandList();

        commandList->IASetVertexBuffers(0, 1, &ri.VertexBufferView());
        commandList->IASetPrimitiveTopology(rsi.GetPrimitiveTopology());

        auto cbHandleIndex = rsi.BufferIndex() + frameResources_->GetCurrentIndex() * frameResources_->ObjsCountLimit;
        auto cbvHandleObj = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvHeap_->GetGPUDescriptorHandleForHeapStart());
        cbvHandleObj.Offset(cbHandleIndex, cbvSrvUavDescriptorSize_);
        commandList->SetGraphicsRootDescriptorTable(0, cbvHandleObj);

        commandList->DrawInstanced(rsi.VerticesCount(), 1, rsi.BaseVertexLocation(), 0);
    }

    void GraphicsCore::DrawRenderItem(RenderItem& ri) {
        for (auto it = ri.GetSubItemsBegin(); it != ri.GetSubItemsEnd(); ++it) {
            auto& rsi = (*it).second;
            DrawRenderSubItemInternal(ri, rsi);
        }
    }

    void GraphicsCore::DrawRenderItemWithInstances(RenderItemWithInstances& riwi) {
        if (riwi.IsDirty()) {
            auto& fr = frameResources_->GetCurrentFrameResource();
            auto currObjectCB = fr.objCB.get();
            PerObjConsts objConstants;
            objConstants.World = riwi.GetTransform();
            objConstants.MaterialIndex = (uint32)-1;
            currObjectCB->CopyData(riwi.BufferIndex(), &objConstants);
            riwi.DecreaseDirtyFramesCount();

            auto currInstBuffer = fr.instBuffer.get();
            uint32 instancesCount = (uint32)riwi.GetInstances().size();
            for (uint32 i = 0; i < instancesCount; ++i) {
                currInstBuffer->CopyData(riwi.GetStartInstanceIndex() + i, &riwi.GetInstances()[i]);
            }
        }

        auto commandList = commandContext_->GetCommandList();

        commandList->IASetVertexBuffers(0, 1, &riwi.VertexBufferView());
        commandList->IASetPrimitiveTopology(riwi.GetPrimitiveTopology());

        auto cbHandleIndex = riwi.BufferIndex() + frameResources_->GetCurrentIndex() * frameResources_->ObjsCountLimit;
        auto cbvHandleObj = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvHeap_->GetGPUDescriptorHandleForHeapStart());
        cbvHandleObj.Offset(cbHandleIndex, cbvSrvUavDescriptorSize_);
        commandList->SetGraphicsRootDescriptorTable(0, cbvHandleObj);

        auto instBuffer = frameResources_->GetCurrentFrameResource().instBuffer->Resource();
        commandList->SetGraphicsRootShaderResourceView(3, instBuffer->GetGPUVirtualAddress());

        commandList->DrawInstanced(riwi.VerticesCount(), (UINT)riwi.GetInstances().size(), 0, 0);
    }
}