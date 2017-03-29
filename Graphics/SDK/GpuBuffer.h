#pragma once

#include <wrl.h>
#include "Common/Attribute/NonCopyable.h"
#include "SDK/GpuResource.h"

namespace Graphics
{
    class CommandContext;
    class RenderItem;

    class GpuBuffer : public GpuResource, public Common::NonCopyable
    {
    public:
        ~GpuBuffer();

        void Create(
            const std::wstring& name, 
            const uint_t elementsCount,
            const uint_t elementSize,
            const void* data);

    private:
        uint_t elementsCount_;
        uint_t elementSize_;
    };
}
