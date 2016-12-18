#pragma once

#include <wrl.h>

struct ID3D12GraphicsCommandList;

namespace Graphics
{
    class CommandQueue;

    class CommandContext
    {
        friend class GpuBuffer;
        friend class GraphicsCore;
    public:
        CommandContext(CommandQueue *commandQueue);

        void Initialize();

    private:
        ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }

    private:
        CommandQueue* commandQueue_;

        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    };
}
