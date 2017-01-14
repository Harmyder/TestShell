#include "stdafx.h"
#include "Core\GraphicsCore.h"
#include "RenderItem.h"
#include "Utility\BufferStuff.h"
#include "Core\Lighting.h"
#include "Core\FrameResource.h"

using namespace std;

namespace Graphics
{
    RenderSubItem::RenderSubItem(uint32 baseVertexLocation,
        uint32 verticesCount,
        const XMFLOAT4X4& transform,
        uint32 objBufferIndex,
        Material* material,
        D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
        const RenderItem& container) :
        BufferEntryDirty(objBufferIndex),
        baseVertexLocation_(baseVertexLocation),
        verticesCount_(verticesCount),
        transform_(transform),
        materialIndex_(material->BufferIndex()),
        primitiveTopology_(primitiveTopology),
        container_(container)
    {
        SetAllFramesDirty();
    }

    void RenderSubItem::SetTransform(const XMFLOAT4X4& transform) {
        transform_ = transform;
        SetAllFramesDirty();
    }

    D3D12_VERTEX_BUFFER_VIEW RenderItem::VertexBufferView() const {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
        vbv.StrideInBytes = vertexSize_;
        vbv.SizeInBytes = vbByteSize_;
        return vbv;
    }

    void RenderItem::Create(
        const RenderItemDesc* itemsDescs, uint32 itemsDescsCount,
        const RenderVerticesDesc* verticesDescs, uint32 verticesDescsCount,
        const uint32* itemsToVertices,
        uint32 vertexSize,
        CommandContext& commandContext,
        unique_ptr<RenderItem>& ri)
    {
        auto& objBufferIndices = GraphicsCore::GetInstance().GetFreePerObjBufferIndices();
        std::vector<uint32> verticesOffsets;
        verticesOffsets.reserve(verticesDescsCount);
        uint32 totalVerticesCount = 0;
        for (uint_t i = 0; i < verticesDescsCount; ++i) {
            verticesOffsets.push_back(totalVerticesCount);
            totalVerticesCount += verticesDescs[i].verticesCount;
        }

        struct RenderItemUniquePtrEnabler : public RenderItem { RenderItemUniquePtrEnabler(uint32 vs, uint32 vc) : RenderItem(vs, vc) {} };
        ri = make_unique<RenderItemUniquePtrEnabler>(vertexSize, totalVerticesCount);
        for (uint_t i = 0; i < itemsDescsCount; ++i) {
            const auto& cur_id = itemsDescs[i];
            const auto& cur_vd = verticesDescs[itemsToVertices[i]];
            const auto p = ri->subItems_.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(cur_id.name),
                std::forward_as_tuple(
                    verticesOffsets[itemsToVertices[i]],
                    cur_vd.verticesCount,
                    cur_id.transform,
                    objBufferIndices.AcquireIndex(),
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
            vertices.insert(vertices.end(), vd.data, vd.data + vd.verticesCount * vertexSize);
        }

        ri->vertexBuffer_.Create(L"ri_vertex", totalVerticesCount, vertexSize, vertices.data(), &commandContext);
    }
}