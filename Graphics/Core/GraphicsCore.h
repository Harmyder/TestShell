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
    class FrameResources;
    class CommandContext;
    class RenderItem;
    class RenderSubItem;
    class LightsHolder;

    // every free index stores next index of free index
    // every taken index stores previous free index
    class FreeIndices {
    public:
        FreeIndices(uint32 sceneObjectsCountLimit) :
            // One more because first index servers to point to first free real index
            indices_(sceneObjectsCountLimit + 1)
        {
            std::iota(std::begin(indices_), std::end(indices_), 1);
        }

        uint32 AcquireIndex() {
            const uint32 res = indices_[0];
            indices_[0] = indices_[res];
            indices_[res] = 0;
            return res - 1;
        }

        void ReleaseIndex(uint32 index) {
            auto internalIndex = index + 1;
            const uint32 prevFree = indices_[internalIndex];
            indices_[internalIndex] = indices_[prevFree];
            indices_[prevFree] = internalIndex;
        }

    private:
        std::vector<uint32> indices_;
    };

    struct InitParams {
        InitParams(uint32 sceneObjsCountLimit, uint32 passesCountLimit, uint32 matsCountLimit, uint32 frameResourcesCount) :
            SceneObjectsCountLimit(sceneObjsCountLimit),
            PassesCountLimit(passesCountLimit),
            MaterialsCountLimit(matsCountLimit),
            FrameResourcesCount(frameResourcesCount)
        {}

        uint32 SceneObjectsCountLimit;
        uint32 PassesCountLimit;
        uint32 MaterialsCountLimit;
        uint32 FrameResourcesCount;
    };

    class GraphicsCore : Pile::NonCopyable
    {
    public:
        DECLARE_CLASS_SINGLETON(GraphicsCore);

    public:
        ~GraphicsCore();

        void Initialize(HWND hwnd, const InitParams& ip);

        void Resize();

        void Update();
        void BeginScene();
        void BeginBoundingVolumes();
        void BeginHud();
        void EndScene();
        void DrawRenderItem(RenderItem& ri);
        void DrawRenderSubItem(RenderItem& ri, const std::string& name);

        CommandContext* GetCommandContext() { return commandContext_.get(); }
        Camera& GetCamera() { return camera_; }

        FreeIndices& GetFreePerObjCbIndices() { return *freePerObjCbIndices_; }
        FreeIndices& GetFreeMaterialCbIndices() { return *freeMaterialCbIndices_; }
        LightsHolder& GetLightsHolder() { return *lightsHolder_; }

        uint_t GetFrameResourcesCount() const;

    private:
        void CreateSwapChain();
        void CreateRtvDsvHeaps();
        void CreateRenderTargetViews();
        void CreateDepthStencil(uint32 width, uint32 height);
        void CreateRootSignature();
        void CreateInputLayout();
        void CreateDescriptorHeaps();
        void CreateConstantBufferViews();
        void CreatePSO();

        D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView();
        ID3D12Resource* CurrentBackBuffer();
        CD3DX12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView();

        void UpdatePassesCBs();
        void DrawRenderSubItemInternal(const RenderItem& ri, RenderSubItem& rsi);

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

        std::unique_ptr<FreeIndices> freePerObjCbIndices_;
        std::unique_ptr<FreeIndices> freeMaterialCbIndices_;
        std::unique_ptr<LightsHolder> lightsHolder_;

        std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout_;

        std::unique_ptr<D3D12_VIEWPORT> screenViewport_;
        D3D12_RECT scissorRect_;

        std::unique_ptr<FrameResources> frameResources_;
        uint32 currFrameResource_ = 0;
        uint32 passCbvOffset_;
        uint32 matCbvOffset_;
        uint32 currentObject_;

        Camera camera_;

        enum : uint8 { kNotUpdated, kUpdated };
        std::vector<uint8> materialsUpdated_;
    };
}
