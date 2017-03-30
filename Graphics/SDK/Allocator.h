#pragma once

#include "GpuResource.h"

namespace Graphics
{
    class Allocation {
    public:
        Allocation(GpuResource& resource, void* data, uint32 offset, uint32 size) : resource_(resource), data_(data), offset_(offset), size_(size) {}

        GpuResource& Buffer() { return resource_; }
        uint32 Offset() { return offset_; }
        void* DataPtr() { return data_; }

    private:
        GpuResource &resource_;
        uint32 offset_;
        uint32 size_;
        void* data_;
        D3D12_GPU_VIRTUAL_ADDRESS gpuAddress_;
    };

    class AllocationPage;

    // Intended to use by a single thread
    class Allocator
    {
    public:
        enum {
            kConstantBufferAlignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT,
            kLinearSubresourceCopyAlignment = 512
        };
        Allocation ReserveBuffer(uint64 currentFence, uint32 sizeInBytes, uint32 alignment);
        void FreePagesUpon(uint64 fence);

    private:
        uint32 currentOffset_ = 0;
        AllocationPage* currentPage_ = nullptr;
        std::vector<AllocationPage*> usedPages_;
    };
}
