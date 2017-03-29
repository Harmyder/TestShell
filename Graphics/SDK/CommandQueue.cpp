#include "stdafx.h"
#include "CommandQueue.h"

#include "CommandAllocatorPool.h"

using namespace std;

namespace Graphics
{
    CommandQueue::CommandQueue() {}
    CommandQueue::~CommandQueue() {}

    void CommandQueue::Create(ID3D12Device *device, D3D12_COMMAND_LIST_TYPE type) {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = type;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        THROW_IF_FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue_.GetAddressOf())));
        commandQueue_->SetName(L"CommandQueue::commandQueue_");

        THROW_IF_FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)));

        commandAllocatorPool_ = make_unique<CommandAllocatorPool>(device, type);
    }

    ID3D12CommandAllocator* CommandQueue::AcquireAllocator() {
        return commandAllocatorPool_->AcquireAllocator(fence_->GetCompletedValue());
    }

    void CommandQueue::ReleaseAllocator(ID3D12CommandAllocator*& allocator) {
        commandAllocatorPool_->ReleaseAllocatorUpon(currentFence_, allocator);
    }

    uint64 CommandQueue::ExecuteCommandList(ID3D12GraphicsCommandList* list) {
        THROW_IF_FAILED(list->Close());
        ID3D12CommandList* l = list;
        // Should i put guard_lock here?
        commandQueue_->ExecuteCommandLists(1, &l);
        commandQueue_->Signal(fence_.Get(), ++currentFence_);
        return currentFence_;
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

    void CommandQueue::WaitAllDone() {
        WaitForFence(currentFence_);
    }

    void CommandQueue::StallForProducer(const CommandQueue* producer) {
        commandQueue_->Wait(producer->fence_.Get(), producer->CurrentFence());
    }

    ID3D12CommandQueue* CommandQueue::GetCommandQueue() { return commandQueue_.Get(); }
}
