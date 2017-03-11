#pragma once

namespace Graphics
{
    class CommandAllocatorPool;

    class CommandQueue {
    public:
        CommandQueue();
        ~CommandQueue();

        void Create(ID3D12Device* device);
        void Flush();

        ID3D12CommandAllocator* AcquireAllocator();
        void ReleaseAllocator(ID3D12CommandAllocator* allocator);

        uint64 CurrentFence() { return currentFence_; }
        uint64 ExecuteCommandList(ID3D12GraphicsCommandList* list);
        void WaitForFence(uint64 fenceValue);
        void WaitAllDone();

        ID3D12CommandQueue* GetCommandQueue();

    private:
        ComPtr<ID3D12CommandQueue> commandQueue_;
        std::unique_ptr<CommandAllocatorPool> commandAllocatorPool_;
        ComPtr<ID3D12Fence> fence_;
        uint64 currentFence_ = 0;
    };
}
