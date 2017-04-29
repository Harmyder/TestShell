#include "stdafx.h"
#include "Texture.h"

#include <fstream>
#include <Common/Container/Dynarray.h>
using namespace std;
using namespace Common;

#include <Wincodec.h>
#include "../src/PlatformHelpers.h"
#include "../src/LoaderHelpers.h"
#include "WICTextureLoader.h"
#include "SDK/CommandContext.h"

namespace Graphics
{
    extern ComPtr<ID3D12Device> g_device;

    namespace TextureCache
    {
        static std::wstring rootDirectory;
        map<wstring, unique_ptr<Texture>> cache;

        void Initialize(const wstring& root) {
            assert(rootDirectory.empty());
            rootDirectory = root;
        }

        void Shutdown() {
            cache.clear();
            rootDirectory = L"";
        }

        pair<bool, reference_wrapper<unique_ptr<Texture>>> FindTexture(const wstring& title) {
            const auto iter = cache.find(title);
            if (iter != cache.end()) return make_pair(true, ref(iter->second));
            auto texture = make_unique<Texture>();
            const auto p = cache.emplace(title, move(texture));
            return make_pair(false, ref(p.first->second));
        }

        void InitializeTexture(
            D3D12_SUBRESOURCE_DATA& subresourceData,
            Texture* texture)
        {
            auto cc = CommandContext::Start(D3D12_COMMAND_LIST_TYPE_DIRECT);
            cc->TransitionResource(*texture, D3D12_RESOURCE_STATE_COPY_DEST, true);
            cc->InitializeTexture(*texture, subresourceData);
            cc->TransitionResource(*texture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            cc->Finish(true);
        }

        Texture* CreateFromFile(const wstring& title) {
            auto p = FindTexture(title);
            auto texture = p.second.get().get();
            if (p.first) return texture;

            ifstream ifs((rootDirectory + title).c_str(), ios::in | ios::binary | ios::ate);
            ifstream::pos_type fileSize = ifs.tellg();
            ifs.seekg(0, ios::beg);
            Dynarray<char> bytes(fileSize);
            ifs.read(bytes.data(), fileSize);

            unique_ptr<uint8_t[]> decodedData;
            D3D12_SUBRESOURCE_DATA subresourceData;
            ComPtr<ID3D12Resource> textureResource;
            assert(sizeof(uint8) == sizeof(char));
            THROW_IF_FAILED(
                DirectX::LoadWICTextureFromFileEx(
                    g_device.Get(),
                    (rootDirectory + title).c_str(),
                    0,
                    D3D12_RESOURCE_FLAG_NONE,
                    DirectX::WIC_LOADER_DEFAULT,
                    textureResource.GetAddressOf(),
                    decodedData,
                    subresourceData
                ));

            texture->InitResource(move(textureResource));
            texture->SetCurrentState(D3D12_RESOURCE_STATE_COPY_DEST);
            InitializeTexture(subresourceData, texture);
            return texture;
        }

        Texture* CreateFromMemory(const wstring& title, const uint8* data, uint32 size) {
            auto p = FindTexture(title);
            auto texture = p.second.get().get();
            if (p.first) return texture;

            unique_ptr<uint8_t[]> decodedData;
            D3D12_SUBRESOURCE_DATA subresourceData;
            ComPtr<ID3D12Resource> textureResource;
            assert(sizeof(uint8) == sizeof(char));
            THROW_IF_FAILED(
                DirectX::LoadWICTextureFromMemoryEx(
                    g_device.Get(),
                    data,
                    size,
                    0,
                    D3D12_RESOURCE_FLAG_NONE,
                    DirectX::WIC_LOADER_DEFAULT,
                    textureResource.GetAddressOf(),
                    decodedData,
                    subresourceData
                ));

            texture->InitResource(move(textureResource));
            texture->SetCurrentState(D3D12_RESOURCE_STATE_COPY_DEST);
            InitializeTexture(subresourceData, texture);
            return texture;
        }
    }

    void TexturesBuffer::AssignIndex(Texture* texture) {
        if (!texture->HasIndex()) {
            texture->InitBufferIndex(freeIndices_.OccupyIndex());
            assert(textures_[texture->BufferIndex()] == nullptr);
        }
        textures_[texture->BufferIndex()] = texture;
    }

    void TexturesBuffer::CreateSRV(Texture* texture) {
        auto theHandle = srvHeapStartHandleCpu_;
        theHandle.Offset(texture->BufferIndex(), descriptorByteSize_);
        g_device->CreateShaderResourceView(texture->GetResource(), nullptr, theHandle);
    }

    Texture* TexturesBuffer::CreateFromFile(const std::wstring& title) {
        auto texture = TextureCache::CreateFromFile(title);
        AssignIndex(texture);
        CreateSRV(texture);
        return texture;
    }

    Texture* TexturesBuffer::CreateFromMemory(const std::wstring& title, const uint8* data, uint32 size) {
        auto texture = TextureCache::CreateFromMemory(title, data, size);
        AssignIndex(texture);
        CreateSRV(texture);
        return texture;
    }

    Texture* TexturesBuffer::CreateFromHandmadeData(const wstring& title, uint32 width, uint32 height, DXGI_FORMAT format, const void* data, bool forceRecreation) {
        auto p = TextureCache::FindTexture(title);
        auto texture = p.second.get().get();
        if (p.first && !forceRecreation) return texture;
        if (forceRecreation) {
            D3D12_SUBRESOURCE_DATA subresourceData;
            subresourceData.pData = data;
            subresourceData.RowPitch = width * DirectX::LoaderHelpers::BitsPerPixel(format) / 8;
            subresourceData.SlicePitch = subresourceData.RowPitch * height;

            TextureCache::InitializeTexture(subresourceData, texture);
        }
        else {
            D3D12_RESOURCE_DESC texDesc = {};
            texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            texDesc.Width = width;
            texDesc.Height = height;
            texDesc.DepthOrArraySize = 1;
            texDesc.MipLevels = 1;
            texDesc.Format = format;
            texDesc.SampleDesc.Count = 1;
            texDesc.SampleDesc.Quality = 0;
            texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

            D3D12_HEAP_PROPERTIES heapProps;
            heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
            heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProps.CreationNodeMask = 1;
            heapProps.VisibleNodeMask = 1;

            ComPtr<ID3D12Resource> textureResource;
            THROW_IF_FAILED(g_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &texDesc,
                D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(textureResource.GetAddressOf())));
            textureResource->SetName((L"Texture:" + title).c_str());

            D3D12_SUBRESOURCE_DATA subresourceData;
            subresourceData.pData = data;
            subresourceData.RowPitch = width * DirectX::LoaderHelpers::BitsPerPixel(format) / 8;
            subresourceData.SlicePitch = subresourceData.RowPitch * height;

            texture->InitResource(move(textureResource));
            texture->SetCurrentState(D3D12_RESOURCE_STATE_COPY_DEST);
            TextureCache::InitializeTexture(subresourceData, texture);

            AssignIndex(texture);
            CreateSRV(texture);
        }
        return texture;
    }

    void TexturesBuffer::Destroy(Texture* texture) {
        auto index = texture->BufferIndex();
        texture->Destroy();
        textures_[index] = nullptr;
        freeIndices_.FreeIndex(index);
    }
}