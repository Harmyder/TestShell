#include "stdafx.h"
#include "SDK\CommandContext.h"

#include "SDK\CommandQueue.h"
#include "SDK\RootSignature.h"
#include "SDK\PipelineStateObject.h"

using namespace std;

namespace Graphics
{
    extern ComPtr<ID3D12Device> g_device;

    class CommandContextPool
    {
    public:
        void SetCommandQueue(CommandQueue* cq) { commandQueue_ = cq; }
        CommandQueue* GetCommandQueue() { return commandQueue_; }

        CommandContext* AcquireContext() {
            CommandContext* ret;
            lock_guard<mutex> lg(acquisitionMutex_);
            if (availableContexts_.empty()) {
                contexts_.push_back(make_unique<CommandContext>());
                ret = contexts_.back().get();
                ret->Initialize();
            }
            else {
                ret = availableContexts_.front();
                availableContexts_.pop();
                ret->Reset();
            }
            return ret;
        }

        void ReleaseContext(CommandContext* cc) {
            lock_guard<mutex> lg(acquisitionMutex_);
            availableContexts_.push(cc);
        }

        void DestroyAll() { contexts_.clear(); }

    private:
        CommandQueue* commandQueue_;
        queue<CommandContext*> availableContexts_;
        vector<unique_ptr<CommandContext>> contexts_;
        mutex acquisitionMutex_;
    };

    CommandContextPool g_Ccp;

    void CommandContext::PreInitialize(CommandQueue *commandQueue) {
        g_Ccp.SetCommandQueue(commandQueue);
    }

    void CommandContext::DestroyAllInstances() {
        g_Ccp.DestroyAll();
    }

    CommandContext* CommandContext::Start() {
        return g_Ccp.AcquireContext();
    }

    uint64 CommandContext::Finish(bool wait) {
        const uint64 fence = Flush(wait);
        g_Ccp.GetCommandQueue()->ReleaseAllocator(currentAllocator_);
        g_Ccp.ReleaseContext(this);
        return fence;
    }

    void CommandContext::Initialize() {
        currentAllocator_ = g_Ccp.GetCommandQueue()->AcquireAllocator();
        THROW_IF_FAILED(g_device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            currentAllocator_,
            nullptr,
            IID_PPV_ARGS(&commandList_)));
    }

    uint64 CommandContext::Flush(bool wait) {
        const uint64 fence = g_Ccp.GetCommandQueue()->ExecuteCommandList(commandList_.Get());
        if (wait) g_Ccp.GetCommandQueue()->WaitAllDone();
        return fence;
    }

    void CommandContext::Reset() {
        currentAllocator_ = g_Ccp.GetCommandQueue()->AcquireAllocator();
        commandList_->Reset(currentAllocator_, nullptr);
    }

    void CommandContext::SetRootSignature(RootSignature& rootSignature) {
        commandList_->SetGraphicsRootSignature(rootSignature.GetRootSignature());
    }

    void CommandContext::SetPipelineStateObject(PipelineStateObject& pso) {
        commandList_->SetPipelineState(pso.GetPipelineStateObject());
    }

}
