#pragma once

#include "Pile\Attribute\NonCopyable.h"
#include "Pile\Pattern\Singleton.h"
#include "Pile\DirectXInclude.h"
#include "Core\Camera.h"
#include <wrl.h>

struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct CD3DX12_CPU_DESCRIPTOR_HANDLE;
struct D3D12_INPUT_ELEMENT_DESC;
struct ID3D12Resource;
struct IDXGIFactory4;
struct IDXGISwapChain;
struct ID3D12DescriptorHeap;
struct ID3D12RootSignature;
struct ID3D12PipelineState;
struct D3D12_VIEWPORT;
typedef RECT D3D12_RECT;

namespace Graphics
{
    class CommandQueue;
    class FrameResource;
    class CommandContext;
    class RenderItem;
    class RenderIndexedItem;

    class GraphicsCore : Pile::NonCopyable
    {
    public:
        DECLARE_CLASS_SINGLETON(GraphicsCore);

    public:
        ~GraphicsCore();

        void Initialize(HWND hwnd);

        void Resize();

        void Update();
        void BeginScene();
        void EndScene();
        void DrawRenderIndexedItem(const RenderIndexedItem& ri);
        void DrawRenderItem(const RenderItem& ri);

        CommandContext* GetCommandContext() { return commandContext_.get(); }
        Camera& GetCamera() { return camera_; }

    private:
        void CreateSwapChain();
        void CreateRtvDsvHeaps();
        void CreateRenderTargetViews();
        void CreateDepthStencil(uint32 width, uint32 height);
        void CreateRootSignature();
        void CreateInputLayout();
        void CreateFrameResources();
        void CreateDescriptorHeaps();
        void CreateConstantBufferViews();
        void CreatePSO();

        D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView();
        ID3D12Resource* CurrentBackBuffer();
        CD3DX12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView();

        void UpdatePassesCBs();
        void UpdateObjectsCBs();

    private:
        HWND hwnd_;
        Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory_;
        Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;

        std::unique_ptr<CommandQueue> commandQueue_;
        std::unique_ptr<CommandContext> commandContext_;

        UINT rtvDescriptorSize_;
        UINT dsvDescriptorSize_;
        UINT cbvSrvUavDescriptorSize_;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;

        static const int SWAP_CHAIN_BUFFERS_COUNT = 2;
        Microsoft::WRL::ComPtr<ID3D12Resource> swapChainBuffer_[SWAP_CHAIN_BUFFERS_COUNT];
        Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer_;
        uint32 currentBackBuffer_ = 0;

        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvHeap_;
        std::map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> psos_;

        std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout_;

        std::unique_ptr<D3D12_VIEWPORT> screenViewport_;
        D3D12_RECT scissorRect_;

        std::vector<std::unique_ptr<FrameResource>> frameResources_;
        uint32 currFrameResource_ = 0;
        uint32 passCbvOffset_;
        uint32 currentObject_;

        Camera camera_;
    };
}
