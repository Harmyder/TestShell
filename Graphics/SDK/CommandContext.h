#pragma once

#include <wrl.h>

struct ID3D12GraphicsCommandList;

namespace Graphics
{
    class CommandQueue;
    class RootSignature;
    class PipelineStateObject;

    class CommandContextPool;

    class CommandContext
    {
    public:
        static void PreInitialize(CommandQueue *commandQueue);
        static void DestroyAllInstances();
        static CommandContext* Start();
        uint64 Finish(bool wait);

        void Reset();
        uint64 Flush(bool wait);

        void SetRootSignature(RootSignature& rootSignature);
        void SetPipelineStateObject(PipelineStateObject& pso);

        ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }

    private:
        void Initialize();

        friend class CommandContextPool;

    private:
        ID3D12CommandAllocator* currentAllocator_;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    };
}
