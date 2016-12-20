#include "stdafx.h"
#include "SDK\CommandContext.h"

#include "SDK\CommandQueue.h"

namespace Graphics
{
    extern ComPtr<ID3D12Device> g_device;
    
    CommandContext::CommandContext(CommandQueue *commandQueue) : commandQueue_(commandQueue) {}

    void CommandContext::Initialize() {
        currentAllocator_ = commandQueue_->AcquireAllocator();
        THROW_IF_FAILED(g_device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            currentAllocator_,
            nullptr,
            IID_PPV_ARGS(&commandList_)));
    }

    void CommandContext::Flush(bool wait) {
        commandQueue_->ExecuteCommandList(commandList_.Get());
        if (wait) commandQueue_->WaitAllDone();
        commandQueue_->ReleaseAllocator(currentAllocator_);
    }

    void CommandContext::Reset() {
        currentAllocator_ = commandQueue_->AcquireAllocator();
        commandList_->Reset(currentAllocator_, nullptr);
    }
}
