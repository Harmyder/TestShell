#include "stdafx.h"
#include "RenderItem.h"
#include "Utility\FreeIndices.h"
#include "Core\Lighting.h"

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
        baseVertexLocation_(baseVertexLocation),
        verticesCount_(verticesCount),
        objBufferIndex_(objBufferIndex),
        transform_(transform),
        dirtyFramesCount_((uint32)GraphicsCore::GetInstance().GetFrameResourcesCount()),
        materialIndex_(material->BufferIndex()),
        primitiveTopology_(primitiveTopology),
        container_(container)
    {}

    D3D12_VERTEX_BUFFER_VIEW RenderItem::VertexBufferView() const {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
        vbv.StrideInBytes = vertexSize_;
        vbv.SizeInBytes = vbByteSize_;
        return vbv;
    }

    void RenderItem::Create(
        const std::vector<RenderItemDesc>& itemsDescs,
        const std::vector<RenderVerticesDesc>& verticesDescs,
        const std::vector<uint32> itemsToVertices,
        uint32 vertexSize,
        CommandContext& commandContext_,
        RenderItem *&pri)
    {
        auto& objBufferIndices = GraphicsCore::GetInstance().GetFreePerObjBufferIndices();
        std::vector<uint32> verticesOffsets;
        verticesOffsets.reserve(verticesDescs.size());
        uint32 totalVerticesCount = 0;
        for (const auto& vd : verticesDescs) {
            verticesOffsets.push_back(totalVerticesCount);
            totalVerticesCount += vd.verticesCount;
        }

        pri = new RenderItem(vertexSize, totalVerticesCount);
        for (uint_t i = 0; i < itemsDescs.size(); ++i) {
            const auto& cur_id = itemsDescs[i];
            const auto& cur_vd = verticesDescs[itemsToVertices[i]];
            const auto p = pri->subItems_.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(cur_id.name),
                std::forward_as_tuple(
                    verticesOffsets[itemsToVertices[i]],
                    cur_vd.verticesCount,
                    cur_id.transform,
                    objBufferIndices.AcquireIndex(),
                    cur_id.material,
                    cur_id.primitiveTopology,
                    *pri)
            );
            if (!p.second) throw "At least two elements have the same name";
        }
        std::vector<uint8> vertices;
        vertices.reserve(totalVerticesCount * vertexSize);
        for (auto& vd : verticesDescs) {
            vertices.insert(vertices.end(), vd.data, vd.data + vd.verticesCount * vertexSize);
        }

        pri->vertexBuffer_.Create(L"ri_vertex", totalVerticesCount, vertexSize, vertices.data(), &commandContext_);
    }
}