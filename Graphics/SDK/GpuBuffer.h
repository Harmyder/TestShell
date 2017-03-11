#pragma once

#include <wrl.h>
#include "Pile\Attribute\NonCopyable.h"
struct ID3D12Resource;

namespace Graphics
{
    class CommandContext;
    class RenderItem;

    class GpuBuffer : public Pile::NonCopyable
    {
        friend class RenderItem;
        friend class RenderItemWithInstances;
    public:
        ~GpuBuffer();

        void Create(
            const std::wstring& name, 
            const uint_t elementsCount,
            const uint_t elementSize,
            const void* data);

    private:
        typedef UINT64 D3D12_GPU_VIRTUAL_ADDRESS;
        D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

        Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;

        uint_t elementsCount_;
        uint_t elementSize_;
    };
}
