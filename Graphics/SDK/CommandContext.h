#pragma once

#include <wrl.h>

struct ID3D12GraphicsCommandList;

namespace Graphics
{
    class CommandQueue;
    class RootSignature;
    class PipelineStateObject;

    class CommandContext
    {
    public:
        CommandContext(CommandQueue *commandQueue);

        void Initialize();

        void Reset();
        void Flush(bool wait);

        void SetRootSignature(RootSignature& rootSignature);
        void SetPipelineStateObject(PipelineStateObject& pso);

        ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }

    private:
        CommandQueue* commandQueue_;

        ID3D12CommandAllocator* currentAllocator_;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    };
}
