#include "stdafx.h"
#include "Core\GraphicsCore.h"
#include "RenderItem.h"
#include "Utility\BufferStuff.h"
#include "Core\Lighting.h"
#include "Core\FrameResource.h"
#include "SDK\CommandContext.h"

using namespace std;

namespace Graphics
{
    D3D12_VERTEX_BUFFER_VIEW RenderItemBase::VertexBufferView() const {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
        vbv.StrideInBytes = vertexSize_;
        vbv.SizeInBytes = vbByteSize_;
        return vbv;
    }

    D3D12_INDEX_BUFFER_VIEW RenderItemBase::IndexBufferView() const {
        D3D12_INDEX_BUFFER_VIEW ibv;
        ibv.BufferLocation = indexBuffer_.GetGPUVirtualAddress();
        ibv.Format = (DXGI_FORMAT)kIndexFormat;
        ibv.SizeInBytes = ibByteSize_;
        return ibv;
    }

    RenderSubItem::RenderSubItem(uint32 baseVertexLocation,
        uint32 baseIndexLocation,
        int verticesCount,
        int indicesCount,
        const XMFLOAT4X3& transform,
        uint32 objBufferIndex,
        const Material* material,
        D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
        const RenderItem& container) :
        BufferEntryDirty(objBufferIndex),
        baseVertexLocation_(baseVertexLocation),
        baseIndexLocation_(baseIndexLocation),
        verticesCount_(verticesCount),
        indicesCount_(indicesCount),
        transform_(transform),
        materialIndex_(material->BufferIndex()),
        primitiveTopology_(primitiveTopology),
        container_(container)
    {
        SetAllFramesDirty();
    }

    void RenderSubItem::SetTransform(const XMFLOAT4X3& transform) {
        transform_ = transform;
        SetAllFramesDirty();
    }

    void RenderItem::Create(
        const RenderItemDesc* itemsDescs, uint32 itemsDescsCount,
        const RenderVerticesDesc* verticesDescs, uint32 verticesDescsCount,
        const uint32* itemsToData,
        uint32 vertexSize,
        unique_ptr<RenderItem>& ri)
    {
        auto& objBufferIndices = GraphicsCore::GetInstance().GetFreePerObjBufferIndices();
        std::vector<uint32> verticesOffsets, indicesOffsets;
        verticesOffsets.reserve(verticesDescsCount);
        indicesOffsets.reserve(verticesDescsCount);
        uint32 totalVerticesCount = 0;
        uint32 totalIndicesCount = 0;
        for (uint_t i = 0; i < verticesDescsCount; ++i) {
            verticesOffsets.push_back(totalVerticesCount);
            totalVerticesCount += verticesDescs[i].verticesCount;
            indicesOffsets.push_back(totalIndicesCount);
            totalIndicesCount += verticesDescs[i].indicesCount;
            assert(verticesDescs[i].indicesCount != 0 && verticesDescs[i].indices != nullptr || verticesDescs[i].indicesCount == 0 && verticesDescs[i].indices == nullptr);
        }

        struct RenderItemUniquePtrEnabler : public RenderItem { RenderItemUniquePtrEnabler(uint32 vs, uint32 vc, uint32 ic) : RenderItem(vs, vc, ic) {} };
        ri = make_unique<RenderItemUniquePtrEnabler>(vertexSize, totalVerticesCount, totalIndicesCount);
        for (uint_t i = 0; i < itemsDescsCount; ++i) {
            const auto& cur_id = itemsDescs[i];
            const auto& cur_vd = verticesDescs[itemsToData[i]];
            const auto p = ri->subItems_.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(cur_id.name),
                std::forward_as_tuple(
                    verticesOffsets[itemsToData[i]],
                    indicesOffsets[itemsToData[i]],
                    cur_vd.verticesCount,
                    cur_vd.indicesCount,
                    cur_id.transform,
                    objBufferIndices.OccupyIndex(),
                    cur_id.material,
                    cur_id.primitiveTopology,
                    *ri)
            );
            if (!p.second) throw "At least two elements have the same name";
        }

        std::vector<uint8> vertices;
        vertices.reserve(totalVerticesCount * vertexSize);
        for (uint_t i = 0; i < verticesDescsCount; ++i) {
            auto& vd = verticesDescs[i];
            vertices.insert(vertices.end(), vd.vertices, vd.vertices + vd.verticesCount * vertexSize);
        }
        ri->VertexBuffer().Create(L"ri_vertex", totalVerticesCount, vertexSize, vertices.data());

        if (totalIndicesCount > 0) {
            std::vector<uint8> indices;
            indices.reserve(totalIndicesCount * kIndexSize);
            for (uint_t i = 0; i < verticesDescsCount; ++i) {
                auto& vd = verticesDescs[i];
                indices.insert(indices.end(), vd.indices, vd.indices + vd.indicesCount * kIndexSize);
            }
            ri->IndexBuffer().Create(L"ri_index", totalIndicesCount, kIndexSize, indices.data());
        }
    }

    void RenderItemBase::SetVertexData(const uint8* data, uint32 vertexOffset, uint32 verticesCount) {
        auto ccTransition = CommandContext::Start(D3D12_COMMAND_LIST_TYPE_DIRECT);
        ccTransition->TransitionResource(vertexBuffer_, D3D12_RESOURCE_STATE_COPY_DEST);
        ccTransition->Flush(true);

        auto cc = CommandContext::Start(D3D12_COMMAND_LIST_TYPE_COPY);
        cc->WriteBuffer(vertexBuffer_, vertexOffset * vertexSize_, data, vertexSize_ * verticesCount);
        cc->Finish(true);

        ccTransition->TransitionResource(vertexBuffer_, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        ccTransition->Finish(false);
    }
}