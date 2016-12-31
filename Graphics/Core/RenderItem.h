#pragma once
#include "stdafx.h"
#include "SDK\GpuBuffer.h"
#include "Core\GraphicsCore.h"

namespace Graphics {

    class Material;

    struct RenderVerticesDesc // keep in sync with grRenderVertices
    {
        uint8 *data;
        uint32 verticesCount;
    };
    struct RenderItemDesc // keep in sync with grRenderSubItemDesc
    {
        std::string& name;
        XMFLOAT4X4& transform;
        Material *material;
    };

    class RenderItem;
    
    class RenderSubItem {
    public:
        RenderSubItem(uint32 baseVertexLocation, uint32 verticesCount, const XMFLOAT4X4& transform, uint32 objCbIndex, Material* material, const RenderItem& container) :
            baseVertexLocation_(baseVertexLocation),
            verticesCount_(verticesCount),
            objCbIndex_(objCbIndex),
            transform_(transform),
            dirtyFramesCount_((uint32)GraphicsCore::GetInstance().GetFrameResourcesCount()),
            material_(*material),
            container_(container)
        {}

        RenderSubItem(const RenderSubItem& other) = delete;

        uint32 BaseVertexLocation() const { return baseVertexLocation_; }
        uint32 VerticesCount() const { return verticesCount_; }
        
        uint32 CbIndex() const { return objCbIndex_; }
        const bool IsDirty() const { return dirtyFramesCount_ != 0; }
        const void DecreaseDirtyFramesCount() { --dirtyFramesCount_; }

        const XMFLOAT4X4& GetTransform() const { return transform_; }
        void SetTransform(const XMFLOAT4X4& transform) {
            transform_ = transform;
            dirtyFramesCount_ = (uint32)GraphicsCore::GetInstance().GetFrameResourcesCount();
        }

        Material& GetMaterial() { return material_; }

        const RenderItem& Container() const { return container_; }

    private:
        uint32 baseVertexLocation_;
        uint32 verticesCount_;
        uint32 dirtyFramesCount_;
        uint32 objCbIndex_;
        XMFLOAT4X4 transform_;
        Material& material_;

        const RenderItem& container_;
    };

    class RenderItem
    {
    public:
        static void Create(
            const std::vector<RenderItemDesc>& itemsDescs,
            const std::vector<RenderVerticesDesc>& verticesDescs,
            const std::vector<uint32> itemsToVertices,
            uint32 vertexSize, CommandContext& commandContext_,
            RenderItem *&pri)
        {
            pri = new RenderItem();
            auto& objCbIndices = GraphicsCore::GetInstance().GetFreePerObjCbIndices();
            std::vector<uint32> verticesOffsets;
            verticesOffsets.reserve(verticesDescs.size());
            uint32 totalVerticesCount = 0;
            for (const auto& vd : verticesDescs) {
                verticesOffsets.push_back(totalVerticesCount);
                totalVerticesCount += vd.verticesCount;
            }

            for (uint_t i = 0; i < itemsDescs.size(); ++i) {
                const auto& cur_id = itemsDescs[i];
                const auto& cur_vd = verticesDescs[itemsToVertices[i]];
                pri->subItems_.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(cur_id.name),
                    std::forward_as_tuple(
                        verticesOffsets[itemsToVertices[i]],
                        cur_vd.verticesCount,
                        cur_id.transform,
                        objCbIndices.AcquireIndex(),
                        cur_id.material,
                        *pri)
                );
            }

            std::vector<uint8> vertices;
            vertices.reserve(totalVerticesCount * vertexSize);
            for (auto& vd : verticesDescs) {
                vertices.insert(vertices.end(), vd.data, vd.data + vd.verticesCount * vertexSize);
            }

            pri->vertexByteStride_ = vertexSize;
            pri->vbByteSize_ = (uint32)vertices.size();

            pri->vertexBuffer_.Create(L"ri_vertex", totalVerticesCount, vertexSize, vertices.data(), &commandContext_);
        }

        ~RenderItem() {
            OutputDebugString("RenderItemDestroyed");
        }
        D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
        uint32 VertexByteStride() const { return vertexByteStride_; }
        uint32 VertexBufferByteSize() const { return vbByteSize_; }

        using SubItems = std::unordered_map<std::string, RenderSubItem>;
        const SubItems::iterator GetSubItemsBegin() { return begin(subItems_); }
        const SubItems::iterator GetSubItemsEnd() { return end(subItems_); }
        RenderSubItem& FindSubItem(const std::string& name) { return subItems_.find(name)->second; }

    private:
        SubItems subItems_;
        GpuBuffer vertexBuffer_;
        uint32 vertexByteStride_;
        uint32 vbByteSize_;

    };
}
