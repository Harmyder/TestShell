#pragma once

#include "stdafx.h"

namespace Graphics
{
    class CommandAllocatorPool
    {
    public:
        CommandAllocatorPool(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);

        ID3D12CommandAllocator* AcquireAllocator(uint64 currentFence);
        void ReleaseAllocatorUpon(uint64 fence, ID3D12CommandAllocator*& allocator);

    private:
        std::vector<ComPtr<ID3D12CommandAllocator>> allocators_;
        std::queue<std::pair<uint64, uint_t>> enqueued_;

        ID3D12Device* device_;
        D3D12_COMMAND_LIST_TYPE type_;

        std::mutex allocators_mutex_;
    };
}
