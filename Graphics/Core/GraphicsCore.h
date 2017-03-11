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
    namespace Utility
    {
        class FreeIndices;
    }
    class CommandQueue;
    class FrameResources;
    class CommandContext;
    class RenderItem;
    class RenderItemWithInstances;
    class RenderSubItem;
    class LightsHolder;
    class MaterialsBuffer;

    struct InitParams {
        InitParams(uint32 sceneObjsCountLimit, uint32 instancesCountLimit, uint32 passesCountLimit, uint32 matsCountLimit, uint32 frameResourcesCount) :
            SceneObjectsCountLimit(sceneObjsCountLimit),
            InstancesCountLimit(instancesCountLimit),
            PassesCountLimit(passesCountLimit),
            MaterialsCountLimit(matsCountLimit),
            FrameResourcesCount(frameResourcesCount)
        {}

        uint32 SceneObjectsCountLimit;
        uint32 InstancesCountLimit;
        uint32 PassesCountLimit;
        uint32 MaterialsCountLimit;
        uint32 FrameResourcesCount;
    };

    enum class RootSignatureType;

    class GraphicsCore : Pile::NonCopyable
    {
    public:
        DECLARE_CLASS_SINGLETON(GraphicsCore);

    public:
        ~GraphicsCore();

        void Initialize(HWND hwnd, const InitParams& ip);
        void Shutdown();

        void Resize();

        void Update();
        void BeginScene();
        void EndScene();
        void DrawRenderItem(RenderItem& ri);
        void DrawRenderSubItem(RenderItem& ri, const std::string& name);
        void DrawRenderItemWithInstances(RenderItemWithInstances& ri);

        CommandQueue* GetCommandQueue() { return commandQueue_.get(); }
        CommandContext* GetCommandContext() { return commandContext_; }
        Camera& GetCamera() { return camera_; }

        Utility::FreeIndices& GetFreePerObjBufferIndices() { return *freePerObjBufferIndices_; }
        LightsHolder& GetLightsHolder() { return *lightsHolder_; }
        MaterialsBuffer& GetMaterialsBuffer() { return *materialsBuffer_; }

        const FrameResources* GetFrameResources() const { return frameResources_.get(); }
        FrameResources* GetFrameResourcesNonConst() { return frameResources_.get(); }

        void SetGraphicsRoot(RootSignatureType type);

    private:
        void CreateSwapChain();
        void CreateRtvDsvHeaps();
        void CreateRenderTargetViews();
        void CreateDepthStencil(uint32 width, uint32 height);
        void CreateDescriptorHeaps();
        void CreateConstantBufferViews();

        D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView();
        ID3D12Resource* CurrentBackBuffer();
        CD3DX12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView();

        void UpdatePassesCBs();
        void UpdateMaterials();
        void DrawRenderSubItemInternal(const RenderItem& ri, RenderSubItem& rsi);

    private:
        HWND hwnd_;
        Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory_;
        Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;

        std::unique_ptr<CommandQueue> commandQueue_;
        CommandContext* commandContext_;

        UINT rtvDescriptorSize_;
        UINT dsvDescriptorSize_;
        UINT cbvSrvUavDescriptorSize_;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;

        static const int SWAP_CHAIN_BUFFERS_COUNT = 2;
        Microsoft::WRL::ComPtr<ID3D12Resource> swapChainBuffer_[SWAP_CHAIN_BUFFERS_COUNT];
        Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer_;
        uint32 currentBackBuffer_ = 0;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvHeap_;

        std::unique_ptr<Utility::FreeIndices> freePerObjBufferIndices_;
        std::unique_ptr<LightsHolder> lightsHolder_;
        std::unique_ptr<MaterialsBuffer> materialsBuffer_;

        std::unique_ptr<D3D12_VIEWPORT> screenViewport_;
        D3D12_RECT scissorRect_;

        std::unique_ptr<FrameResources> frameResources_;
        uint32 passCbvOffset_;
        uint32 currentObject_;

        Camera camera_;
    };
}
