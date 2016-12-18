#pragma once

#include <wrl.h>

struct ID3D12Resource;

namespace Graphics
{
    class CommandContext;

    class GpuBuffer
    {
    public:
        ~GpuBuffer();

        void Create(
            const std::string& name, 
            const uint_t elementsCount,
            const uint_t elementSize,
            const void* data,
            CommandContext* commandContext_);
        void ReleaseUpload();

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;
        Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer_;

        uint_t elementsCount_;
        uint_t elementSize_;
    };
}
