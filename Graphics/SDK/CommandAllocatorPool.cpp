#include "stdafx.h"
#include "CommandAllocatorPool.h"

using namespace std;

namespace Graphics
{
    CommandAllocatorPool::CommandAllocatorPool(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type) :
        device_(device), type_(type)
    {}

    ID3D12CommandAllocator* CommandAllocatorPool::AcquireAllocator(uint64 currentFence) {
        lock_guard<mutex> g(allocators_mutex_);

        ID3D12CommandAllocator* ret;
        if (!enqueued_.empty() && enqueued_.front().first < currentFence) {
            ret = allocators_[enqueued_.front().second].Get();
            THROW_IF_FAILED(ret->Reset());
            enqueued_.pop();
        }
        else {
            THROW_IF_FAILED(device_->CreateCommandAllocator(
                type_,
                IID_PPV_ARGS(&ret)));
            allocators_.push_back(ComPtr<ID3D12CommandAllocator>(ret));
        }
        return ret;
    }

    void CommandAllocatorPool::ReleaseAllocatorUpon(uint64 fence, ID3D12CommandAllocator* allocator) {
        lock_guard<mutex> g(allocators_mutex_);
        
        auto it = find_if(begin(allocators_), end(allocators_), [&allocator](auto cp) { return cp.Get() == allocator; });
        enqueued_.push(make_pair(fence, distance(it, begin(allocators_))));
    }
}