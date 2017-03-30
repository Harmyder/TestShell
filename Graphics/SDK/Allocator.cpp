#include "stdafx.h"
#include "Allocator.h"

using namespace std;

namespace Graphics
{
    extern ComPtr<ID3D12Device> g_device;

    class AllocationPage : public GpuResource
    {
    public:
        AllocationPage(ID3D12Resource* resource) : GpuResource(resource, D3D12_RESOURCE_STATE_COPY_SOURCE) { resource_->Map(0, nullptr, (void**)&cpuVirtualAddress_); }
        ~AllocationPage() { resource_->Unmap(0, nullptr); }

        auto GetCpuVirtualAddress() { return cpuVirtualAddress_; }

    private:
        uint8* cpuVirtualAddress_;
    };

    class PagePool {
    public:
        enum { kPageSize = 0x100000 };

        AllocationPage* AcquirePage(uint64 currentFence) {
            lock_guard<mutex> g(pages_mutex_);

            AllocationPage* ret;
            if (!enqueued_.empty() && enqueued_.front().first < currentFence) {
                ret = pages_[enqueued_.front().second].get();
                enqueued_.pop();
            }
            else {
                ID3D12Resource* uploadBuffer;
                THROW_IF_FAILED(g_device->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                    D3D12_HEAP_FLAG_NONE,
                    &CD3DX12_RESOURCE_DESC::Buffer(kPageSize),
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&uploadBuffer)));
                uploadBuffer->SetName(wstring(L"AllocationPage").c_str());

                pages_.push_back(make_unique<AllocationPage>(uploadBuffer));
                ret = pages_.back().get();
            }
            return ret;
        }

        void ReleasePageUpon(uint64 fence, AllocationPage* allocationPage) {
            lock_guard<mutex> g(pages_mutex_);

            auto it = find_if(begin(pages_), end(pages_), [&allocationPage](auto& cp) { return cp.get() == allocationPage; });
            uint_t allocatorIndex = distance(begin(pages_), it);
            enqueued_.push(make_pair(fence, allocatorIndex));
        }

    private:
        vector<unique_ptr<AllocationPage>> pages_;
        queue<std::pair<uint64, uint_t>> enqueued_;

        mutex pages_mutex_;
    };

    static PagePool s_pagePool;

    Allocation Allocator::ReserveBuffer(uint64 currentFence, uint32 sizeInBytes, uint32 alignment) {
        assert(sizeInBytes < PagePool::kPageSize); // Add possibility to request large pages
        auto neededOffset = Utility::AlignUp(currentOffset_, alignment);
        uint32 offset = neededOffset;
        if (usedPages_.size() == 0 || sizeInBytes > PagePool::kPageSize - neededOffset) {
            usedPages_.push_back(s_pagePool.AcquirePage(currentFence));
            offset = 0;
        }
        currentOffset_ = offset + sizeInBytes;
        return Allocation(*usedPages_.back(), usedPages_.back()->GetCpuVirtualAddress() + offset, offset, sizeInBytes);
    }

    void Allocator::FreePagesUpon(uint64 fence) {
        for (auto p : usedPages_) {
            s_pagePool.ReleasePageUpon(fence, p);
        }
        usedPages_.clear();
        currentOffset_ = 0;
    }
}