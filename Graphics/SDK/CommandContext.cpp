#include "stdafx.h"
#include "SDK/CommandContext.h"

#include "SDK/CommandQueue.h"
#include "SDK/RootSignature.h"
#include "SDK/PipelineStateObject.h"
#include "SDK/Allocator.h"

using namespace std;

namespace Graphics
{
    extern ComPtr<ID3D12Device> g_device;

    class CommandContextPool
    {
    public:
        void SetGraphicsQueue(CommandQueue* graphicsQueue) { commandQueues_[IndexByType(D3D12_COMMAND_LIST_TYPE_DIRECT)] = graphicsQueue; }
        void SetCopyQueue(CommandQueue* copyQueue) { commandQueues_[IndexByType(D3D12_COMMAND_LIST_TYPE_COPY)] = copyQueue; }
        CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) { return commandQueues_[IndexByType(type)]; }

        CommandContext* AcquireContext(D3D12_COMMAND_LIST_TYPE type) {
            const uint32 index = IndexByType(type);
            CommandContext* ret;
            lock_guard<mutex> lg(acquisitionMutex_);
            if (availableContexts_[index].empty()) {
                contexts_[index].push_back(make_unique<CommandContext>(type));
                ret = contexts_[index].back().get();
                ret->Initialize();
            }
            else {
                ret = availableContexts_[index].front();
                availableContexts_[index].pop();
                ret->Reset();
            }
            return ret;
        }

        void ReleaseContext(CommandContext* cc) {
            lock_guard<mutex> lg(acquisitionMutex_);
            availableContexts_[IndexByType(cc->type_)].push(cc);
        }

        void DestroyAll() { for (auto& contexts : contexts_) contexts.clear(); }

    private:
        uint32 IndexByType(D3D12_COMMAND_LIST_TYPE type) {
            switch (type) {
            case D3D12_COMMAND_LIST_TYPE_DIRECT: return 0; break;
            case D3D12_COMMAND_LIST_TYPE_COPY: return 1; break;
            default: throw "Not supported context type";
            }
        }
    private:
        array<CommandQueue*, 2> commandQueues_;
        array<queue<CommandContext*>, 2> availableContexts_;
        array<vector<unique_ptr<CommandContext>>, 2> contexts_;
        mutex acquisitionMutex_;
    };

    CommandContextPool g_Ccp;

    CommandContext::CommandContext(D3D12_COMMAND_LIST_TYPE type) : type_(type), allocator_(make_unique<Allocator>()) {}
    CommandContext::~CommandContext() {}

    void CommandContext::PreInitialize(CommandQueue *graphicsQueue, CommandQueue *copyQueue) {
        g_Ccp.SetGraphicsQueue(graphicsQueue);
        g_Ccp.SetCopyQueue(copyQueue);
    }

    void CommandContext::DestroyAllInstances() {
        g_Ccp.DestroyAll();
    }

    CommandContext* CommandContext::Start(D3D12_COMMAND_LIST_TYPE type) {
        return g_Ccp.AcquireContext(type);
    }

    uint64 CommandContext::Finish(bool wait) {
        const uint64 fence = FlushInternal(wait);
        g_Ccp.GetCommandQueue(type_)->ReleaseAllocator(currentAllocator_);
        g_Ccp.ReleaseContext(this);
        return fence;
    }

    void CommandContext::Initialize() {
        currentAllocator_ = g_Ccp.GetCommandQueue(type_)->AcquireAllocator();
        THROW_IF_FAILED(g_device->CreateCommandList(
            0,
            type_,
            currentAllocator_,
            nullptr,
            IID_PPV_ARGS(&commandList_)));
    }

    uint64 CommandContext::Flush(bool wait) {
        auto fence = FlushInternal(wait);
        commandList_->Reset(currentAllocator_, nullptr);
        return fence;
    }

    uint64 CommandContext::FlushInternal(bool wait) {
        FlushResourceBarriers();
        const uint64 fence = g_Ccp.GetCommandQueue(type_)->ExecuteCommandList(commandList_.Get());
        allocator_->FreePagesUpon(fence);
        if (wait) g_Ccp.GetCommandQueue(type_)->WaitAllDone();
        return fence;
    }

    void CommandContext::Reset() {
        assert(currentAllocator_ == nullptr);
        currentAllocator_ = g_Ccp.GetCommandQueue(type_)->AcquireAllocator();
        commandList_->Reset(currentAllocator_, nullptr);
    }

    void CommandContext::SetRootSignature(RootSignature& rootSignature) {
        commandList_->SetGraphicsRootSignature(rootSignature.GetRootSignature());
    }

    void CommandContext::SetPipelineStateObject(PipelineStateObject& pso) {
        commandList_->SetPipelineState(pso.GetPipelineStateObject());
    }

    void CommandContext::WriteBuffer(GpuResource& dest, uint32 destOffset, const void* data, uint32 sizeInBytes) {
        auto uploadMemory = allocator_->ReserveBuffer(g_Ccp.GetCommandQueue(type_)->CurrentFence(), sizeInBytes, Allocator::kLinearSubresourceCopyAlignment);
        memcpy(uploadMemory.DataPtr(), data, sizeInBytes);
        commandList_->CopyBufferRegion(dest.GetResource(), destOffset, uploadMemory.Buffer().GetResource(), uploadMemory.Offset(), sizeInBytes);
    }

    void CommandContext::TransitionResource(GpuResource& resource, D3D12_RESOURCE_STATES newState) {
        const auto f = [&resource, &newState](D3D12_RESOURCE_BARRIER&) {
            resource.SetCurrentState(newState);
        };
        TransitionResourceHelper(resource, newState, f);
    }

    void CommandContext::TransitionResourceBegin(GpuResource& resource, D3D12_RESOURCE_STATES newState) {
        assert(resource.GetTransitionState() == (D3D12_RESOURCE_STATES)-1 && "Transition already has started");

        const auto f = [&resource, &newState](D3D12_RESOURCE_BARRIER& barrier) {
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY;
            resource.SetTransitionState(newState);
        };
        TransitionResourceHelper(resource, newState, f);
    }

    void CommandContext::TransitionResourceEnd(GpuResource& resource) {
        assert(resource.GetTransitionState() != (D3D12_RESOURCE_STATES)-1 && "Transition already has not started");

        const auto f = [&resource](D3D12_RESOURCE_BARRIER& barrier) {
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
        };
        TransitionResourceHelper(resource, resource.GetTransitionState(), f);
    }

    void CommandContext::FlushResourceBarriers() {
        if (numbersOfBarriersToFlush_ > 0) {
            commandList_->ResourceBarrier(numbersOfBarriersToFlush_, resourceBarrierBuffer_.data());
            numbersOfBarriersToFlush_ = 0;
        }
    }
}
