#pragma once
#include "stdafx.h"
#include "SDK\GpuBuffer.h"
#include "Core\GraphicsCore.h"

namespace Graphics {

    struct RenderItemDesc // keep in sync with grRenderItemDesc
    {
        std::string& name;
        uint8 *data;
        uint32 verticesCount;
        XMFLOAT4X4& transform;
    };

    class RenderItem;
    
    class RenderSubItem {
    public:
        RenderSubItem(uint32 baseVertexLocation, uint32 verticesCount, const XMFLOAT4X4& transform, uint32 objCbIndex, const RenderItem& container) :
            baseVertexLocation_(baseVertexLocation),
            verticesCount_(verticesCount),
            objCbIndex_(objCbIndex),
            transform_(transform),
            dirtyFramesCount_(kFrameResourcesCount),
            container_(container)
        {}

        RenderSubItem(const RenderSubItem& other) = delete;

        uint32 BaseVertexLocation() const { return baseVertexLocation_; }
        uint32 VerticesCount() const { return verticesCount_; }
        
        uint32 GetObjCbIndex() const { return objCbIndex_; }
        const bool IsDirty() const { return dirtyFramesCount_ != 0; }
        const void DecreaseDirtyFramesCount() { --dirtyFramesCount_; }

        const XMFLOAT4X4& GetTransform() const { return transform_; }
        void SetTransform(const XMFLOAT4X4& transform) {
            transform_ = transform;
            dirtyFramesCount_ = kFrameResourcesCount;
        }

        const RenderItem& Container() const { return container_; }

    private:
        uint32 baseVertexLocation_;
        uint32 verticesCount_;
        uint32 dirtyFramesCount_;
        uint32 objCbIndex_;
        XMFLOAT4X4 transform_;

        const RenderItem& container_;
    };

    class RenderItem
    {
    public:
        static void Create(const std::vector<RenderItemDesc>& descs, uint32 vertexSize, CommandContext& commandContext_, RenderItem *&pri) {
            pri = new RenderItem();
            uint_t verticesCount = 0;
            auto objCbIndices = GraphicsCore::GetInstance().GetFreePerObjCbIndices();
            for (auto& d : descs) {
                pri->subItems_.emplace(std::piecewise_construct, std::forward_as_tuple(d.name), std::forward_as_tuple((uint32)verticesCount, d.verticesCount, d.transform, objCbIndices.AcquireIndex(), *pri));
                verticesCount += d.verticesCount;
            }

            std::vector<uint8> vertices;
            vertices.reserve(verticesCount * vertexSize);
            for (auto& d : descs) {
                vertices.insert(vertices.end(), d.data, d.data + d.verticesCount * vertexSize);
            }

            pri->vertexByteStride_ = vertexSize;
            pri->vbByteSize_ = vertexSize * (uint32)verticesCount;

            pri->vertexBuffer_.Create(L"ri_vertex", verticesCount, vertexSize, vertices.data(), &commandContext_);
        }

        D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
        uint32 VertexByteStride() const { return vertexByteStride_; }
        uint32 VertexBufferByteSize() const { return vbByteSize_; }

        using SubItems = std::unordered_map<std::string, RenderSubItem>;
        const SubItems::iterator GetSubItemsBegin() { return begin(subItems_); }
        const SubItems::iterator GetSubItemsEnd() { return end(subItems_); }

    private:
        SubItems subItems_;
        GpuBuffer vertexBuffer_;
        uint32 vertexByteStride_;
        uint32 vbByteSize_;

    };
}
