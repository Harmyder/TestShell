#pragma once

#include "Utility/BufferStuff.h"
#include "SDK/GpuResource.h"
#include "Common/Container/Dynarray.h"

namespace Graphics
{
    class Texture : public GpuResource, public Utility::BufferEntryDirty
    {
    public:
        Texture() {}
        Texture(ID3D12Resource* resource, D3D12_RESOURCE_STATES state, uint32 bufferIndex) :
            GpuResource(resource, state), BufferEntryDirty(bufferIndex)
        {}

        void InitBufferIndex(uint32 bufferIndex) { Utility::BufferEntryDirty::InitBufferIndex(bufferIndex); }
        void InitResource(ComPtr<ID3D12Resource>&& resource) { assert(!resource_); resource_ = std::move(resource); }
    };

    namespace TextureCache {
        void Initialize(const std::wstring& rootDirectory);
        void Shutdown();
    }

    class TexturesBuffer {
    public:
        TexturesBuffer(uint32 texturesCountLimit, CD3DX12_CPU_DESCRIPTOR_HANDLE srvHeapStartHandleCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE srvHeapStartHandleGpu, uint32 descriptorByteSize) :
            freeIndices_(texturesCountLimit),
            texturesCountLimit_(texturesCountLimit),
            textures_(texturesCountLimit),
            srvHeapStartHandleCpu_(srvHeapStartHandleCpu),
            srvHeapStartHandleGpu_(srvHeapStartHandleGpu),
            descriptorByteSize_(descriptorByteSize)
        {}

        Texture* CreateFromFile(const std::wstring& title);
        Texture* CreateFromMemory(const std::wstring& title, const uint8* data, uint32 size);
        // Doesn't require knowledge of file format like BMP to create a texture, may be useful for dynamic mesh coloring
        Texture* CreateFromHandmadeData(const std::wstring& title, uint32 width, uint32 height, DXGI_FORMAT format, const void* data, bool forceRecreation);
        void Destroy(Texture* texture);

        D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSRV(const Texture* texture) {
            auto theHandle = srvHeapStartHandleGpu_;
            return (D3D12_GPU_DESCRIPTOR_HANDLE)theHandle.Offset(texture->BufferIndex(), descriptorByteSize_);
        }

    private:
        void AssignIndex(Texture* texture);
        void CreateSRV(Texture* texture);

    private:
        Utility::FreeIndices freeIndices_;
        uint32 texturesCountLimit_;
        Common::Dynarray<Texture*> textures_;
        CD3DX12_CPU_DESCRIPTOR_HANDLE srvHeapStartHandleCpu_;
        CD3DX12_GPU_DESCRIPTOR_HANDLE srvHeapStartHandleGpu_;
        uint32 descriptorByteSize_;
    };
}