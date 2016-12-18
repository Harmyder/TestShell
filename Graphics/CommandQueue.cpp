#include "stdafx.h"
#include "CommandQueue.h"

#include "CommandAllocatorPool.h"

using namespace std;

namespace Graphics
{
    CommandQueue::CommandQueue() {}
    CommandQueue::~CommandQueue() {}

    void CommandQueue::Create(ID3D12Device *device) {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        THROW_IF_FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue_)));

        THROW_IF_FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)));

        commandAllocatorPool_ = make_unique<CommandAllocatorPool>(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
    }

    void CommandQueue::Flush() {
        currentFence_++;

        THROW_IF_FAILED(commandQueue_->Signal(fence_.Get(), currentFence_));

        if (fence_->GetCompletedValue() < currentFence_)
        {
            HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

            THROW_IF_FAILED(fence_->SetEventOnCompletion(currentFence_, eventHandle));

            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
    }

    ID3D12CommandAllocator* CommandQueue::AcquireAllocator() {
        return commandAllocatorPool_->AcquireAllocator(currentFence_);
    }

    void CommandQueue::ReleaseAllocator(ID3D12CommandAllocator* allocator) {
        commandAllocatorPool_->ReleaseAllocatorUpon(currentFence_, allocator);
    }

    void CommandQueue::ExecuteCommandList(ID3D12GraphicsCommandList* list) {
        THROW_IF_FAILED(list->Close());
        ID3D12CommandList* l = list;
        // Should i put guard_lock here?
        commandQueue_->ExecuteCommandLists(1, &l);
        commandQueue_->Signal(fence_.Get(), ++currentFence_);
    }

    void CommandQueue::WaitForFence(uint64 fenceValue) {
        if (fence_->GetCompletedValue() < fenceValue)
        {
            HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
            THROW_IF_FAILED(fence_->SetEventOnCompletion(fenceValue, eventHandle));
            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
    }

    ID3D12CommandQueue* CommandQueue::GetCommandQueue() { return commandQueue_.Get(); }
}
