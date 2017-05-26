#include "stdafx.h"
#include "Core\GraphicsCore.h"
#include "RenderItem.h"
#include "Utility\BufferStuff.h"
#include "Core\Lighting.h"
#include "Core\Texture.h"
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

    RenderSubItemBase::RenderSubItemBase(const ConstructionData& data) :
        BufferEntryDirty(data.objBufferIndex),
        baseVertexLocation_(data.baseVertexLocation),
        baseIndexLocation_(data.baseIndexLocation),
        verticesCount_(data.verticesCount),
        indicesCount_(data.indicesCount),
        transform_(data.transform),
        materialIndex_(data.material->BufferIndex()),
        texture_(data.texture)
    {
        SetAllFramesDirty();
    }

    void RenderSubItemBase::SetTransform(const XMFLOAT4X3& transform) {
        transform_ = transform;
        SetAllFramesDirty();
    }

    void RenderItemBase::CreateVertexBuffer(const RenderVerticesDesc* verticesDescs, uint32 verticesDescsCount, uint32 totalVerticesCount, uint32 vertexSize) {
        std::vector<uint8> vertices;
        vertices.reserve(totalVerticesCount * vertexSize);
        for (uint_t i = 0; i < verticesDescsCount; ++i) {
            auto& vd = verticesDescs[i];
            vertices.insert(vertices.end(), vd.vertices, vd.vertices + vd.verticesCount * vertexSize);
        }
        VertexBuffer().Create(L"ri_vertex", totalVerticesCount, vertexSize, vertices.data());
    }

    template <>
    bool RenderItemBase2<RenderSubItem>::AddSubItem(const RenderSubItemBase::ConstructionData& data, const RenderSubItem::Desc& itemDesc) {
        const auto p = subItems_.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(itemDesc.name),
            std::forward_as_tuple(data, itemDesc.primitiveTopology, *(RenderItem*)this)
        );
        return p.second;
    }

    template <>
    bool RenderItemBase2<RenderSubItemParticles>::AddSubItem(const RenderSubItemBase::ConstructionData& data, const RenderSubItemParticles::Desc& itemDesc) {
        const auto p = subItems_.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(itemDesc.name),
            std::forward_as_tuple(data, itemDesc.particleSize, *(RenderItemParticles*)this)
        );
        return p.second;
    }

    template <class RSI>
    template<class IndicesOffsets>
    void RenderItemBase2<RSI>::AddSubItems(
        const typename RSI::Desc* itemsDescs,
        uint32 itemsDescsCount,
        const RenderVerticesDesc* verticesDescs,
        const uint32* itemsToData,
        const vector<uint32>& verticesOffsets,
        const IndicesOffsets& indicesOffsets,
        Utility::FreeIndices& freeIndices)
    {
        for (uint_t i = 0; i < itemsDescsCount; ++i) {
            const auto& cur_id = itemsDescs[i];
            const auto& cur_vd = verticesDescs[itemsToData[i]];
            RenderSubItemBase::ConstructionData data{
                verticesOffsets[itemsToData[i]],
                indicesOffsets(itemsToData[i]),
                cur_vd.verticesCount,
                cur_vd.indicesCount,
                cur_id.transform,
                freeIndices.OccupyIndex(),
                cur_id.material,
                cur_id.texture
            };
            if (!AddSubItem(data, cur_id)) throw "At least two elements have the same name";
        }
    }

    void RenderItem::Create(
        const RenderItemDesc* itemsDescs, uint32 itemsDescsCount,
        const RenderVerticesDesc* verticesDescs, uint32 verticesDescsCount,
        const uint32* itemsToData,
        uint32 vertexSize,
        unique_ptr<RenderItem>& ri)
    {
        vector<uint32> verticesOffsets, indicesOffsets;
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

        auto& objBufferIndices = GraphicsCore::GetInstance().GetFreePerObjBufferIndices();
        struct RenderItemUniquePtrEnabler : public RenderItem { RenderItemUniquePtrEnabler(uint32 vs, uint32 vc, uint32 ic) : RenderItem(vs, vc, ic) {} };
        ri = make_unique<RenderItemUniquePtrEnabler>(vertexSize, totalVerticesCount, totalIndicesCount);
        ri->AddSubItems(itemsDescs, itemsDescsCount, verticesDescs, itemsToData, verticesOffsets, [&indicesOffsets](int i) { return indicesOffsets[i]; }, objBufferIndices);

        ri->CreateVertexBuffer(verticesDescs, verticesDescsCount, totalVerticesCount, vertexSize);

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

    void RenderItemParticles::Create(
        const RenderItemParticlesDesc* itemsDescs, uint32 itemsDescsCount,
        const RenderVerticesDesc* verticesDescs, uint32 verticesDescsCount,
        const uint32* itemsToData,
        uint32 vertexSize,
        unique_ptr<RenderItemParticles>& ri)
    {
        std::vector<uint32> verticesOffsets;
        verticesOffsets.reserve(verticesDescsCount);
        uint32 totalVerticesCount = 0;
        for (uint_t i = 0; i < verticesDescsCount; ++i) {
            verticesOffsets.push_back(totalVerticesCount);
            totalVerticesCount += verticesDescs[i].verticesCount;
            assert(verticesDescs[i].indicesCount == 0 && verticesDescs[i].indices == nullptr);
        }

        auto& pmBufferIndices = GraphicsCore::GetInstance().GetFreeParticlesMetaBufferIndices();
        struct RenderItemParticlesUniquePtrEnabler : public RenderItemParticles { RenderItemParticlesUniquePtrEnabler(uint32 vs, uint32 vc) : RenderItemParticles(vs, vc) {} };
        ri = make_unique<RenderItemParticlesUniquePtrEnabler>(vertexSize, totalVerticesCount);
        ri->AddSubItems(itemsDescs, itemsDescsCount, verticesDescs, itemsToData, verticesOffsets, [](int) -> uint32 { return 0; }, pmBufferIndices);

        ri->CreateVertexBuffer(verticesDescs, verticesDescsCount, totalVerticesCount, vertexSize);
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