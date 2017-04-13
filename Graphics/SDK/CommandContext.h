#pragma once

#include <wrl.h>

struct ID3D12GraphicsCommandList;

namespace Graphics
{
    class CommandQueue;
    class RootSignature;
    class PipelineStateObject;
    class GpuResource;
    class Allocator;

    class CommandContextPool;

    class CommandContext
    {
    public:
        static void PreInitialize(CommandQueue *graphicsQueue, CommandQueue *copyQueue);
        static void DestroyAllInstances();
        static CommandContext* Start(D3D12_COMMAND_LIST_TYPE type);
        uint64 Finish(bool wait);

        CommandContext(D3D12_COMMAND_LIST_TYPE type);
        ~CommandContext();

        void Reset();
        uint64 Flush(bool wait);

        void SetRootSignature(RootSignature& rootSignature);
        void SetPipelineStateObject(PipelineStateObject& pso);

        ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }

        void WriteBuffer(GpuResource& dest, uint32 destOffset, const void* data, uint32 sizeInBytes);
        void InitializeTexture(GpuResource& Dest, D3D12_SUBRESOURCE_DATA& data);

        void TransitionResource(GpuResource& resource, D3D12_RESOURCE_STATES newState);
        void TransitionResourceBegin(GpuResource& resource, D3D12_RESOURCE_STATES newState);
        void TransitionResourceEnd(GpuResource& resource);
        void FlushResourceBarriers();

    private:
        void Initialize();
        template <class F>
        void TransitionResourceHelper(GpuResource& resource, D3D12_RESOURCE_STATES newState, F f);
        uint64 FlushInternal(bool wait);

        friend class CommandContextPool;

    private:
        D3D12_COMMAND_LIST_TYPE type_;
        ID3D12CommandAllocator* currentAllocator_;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

        uint32 numbersOfBarriersToFlush_ = 0;
        std::array<D3D12_RESOURCE_BARRIER, 16> resourceBarrierBuffer_;

        std::unique_ptr<Allocator> allocator_;
    };

    template <class F>
    inline void CommandContext::TransitionResourceHelper(GpuResource& resource, D3D12_RESOURCE_STATES newState, F f) {
        const auto currentState = resource.GetCurrentState();
        
        if (currentState != newState) {
            auto& barrier = resourceBarrierBuffer_[numbersOfBarriersToFlush_++];
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition.pResource = resource.GetResource();
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Transition.StateBefore = currentState;
            barrier.Transition.StateAfter = newState;

            f(barrier);

            if (numbersOfBarriersToFlush_ == resourceBarrierBuffer_.size()) FlushResourceBarriers();
        }
    }
}
