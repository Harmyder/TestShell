#pragma once

namespace Graphics
{
    class GpuResource
    {
    public:
        GpuResource() {}
        GpuResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES state) : resource_(resource), currentState_(state) {}

        ID3D12Resource* GetResource() { return resource_.Get(); }
        const ID3D12Resource* GetResource() const { return resource_.Get(); }

        D3D12_RESOURCE_STATES GetCurrentState() const { return currentState_; }
        D3D12_RESOURCE_STATES GetTransitionState() const { return transitionState_; }
        void SetCurrentState(D3D12_RESOURCE_STATES newState) { currentState_ = newState; }
        void SetTransitionState(D3D12_RESOURCE_STATES transitionState) { transitionState_ = transitionState; }

        D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return resource_->GetGPUVirtualAddress(); }

        void Destroy() { resource_.Reset(); }

    protected:
        Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
        D3D12_RESOURCE_STATES currentState_ = (D3D12_RESOURCE_STATES)-1;
        D3D12_RESOURCE_STATES transitionState_ = (D3D12_RESOURCE_STATES)-1;
    };
}