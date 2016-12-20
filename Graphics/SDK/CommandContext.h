#pragma once

#include <wrl.h>

struct ID3D12GraphicsCommandList;

namespace Graphics
{
    class CommandQueue;

    class CommandContext
    {
    public:
        CommandContext(CommandQueue *commandQueue);

        void Initialize();

        void Reset();
        void Flush(bool wait);

        ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }

    private:
        CommandQueue* commandQueue_;

        ID3D12CommandAllocator* currentAllocator_;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    };
}
