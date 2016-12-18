#include "stdafx.h"
#include "CommandContext.h"

#include "CommandQueue.h"

namespace Graphics
{
    extern ComPtr<ID3D12Device> g_device;
    
    CommandContext::CommandContext(CommandQueue *commandQueue) : commandQueue_(commandQueue) {}

    void CommandContext::Initialize() {
        THROW_IF_FAILED(g_device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            commandQueue_->AcquireAllocator(),
            nullptr,
            IID_PPV_ARGS(commandList_.GetAddressOf())));
    }
}
