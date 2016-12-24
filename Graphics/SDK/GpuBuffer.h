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
        friend class RenderIndexedItem;
    public:
        ~GpuBuffer();

        void Create(
            const std::wstring& name, 
            const uint_t elementsCount,
            const uint_t elementSize,
            const void* data,
            CommandContext* commandContext_);

    private:
        typedef UINT64 D3D12_GPU_VIRTUAL_ADDRESS;
        D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;
        void ReleaseUpload();

        Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;
        Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer_;

        uint_t elementsCount_;
        uint_t elementSize_;
    };
}
