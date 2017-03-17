#include "stdafx.h"
#include "Core\RenderItemWithInstances.h"

#include "Core\RenderItem.h"
#include "Core\GraphicsCore.h"
#include "Core\FrameResource.h"

using namespace std;

namespace Graphics
{
    RenderItemWithInstances::RenderItemWithInstances(uint32 bufferIndex, uint32 vertexSize, uint32 verticesCount, uint32 indicesCount, uint32 instancesCount, XMFLOAT4X3 transform, D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) :
        RenderItemBase(vertexSize, verticesCount, indicesCount),
        BufferEntryDirty(bufferIndex),
        verticesCount_(verticesCount),
        indicesCount_(indicesCount),
        transform_(transform),
        primitiveTopology_(primitiveTopology),
        frInstsStart_(GraphicsCore::GetInstance().GetFrameResourcesNonConst()->AcquireInstsRange(instancesCount))
    {
        SetAllFramesDirty();
    }

    void RenderItemWithInstances::SetTransform(const XMFLOAT4X3& transform) {
        transform_ = transform;
        SetAllFramesDirty();
    }

    void RenderItemWithInstances::Create(
        const RenderItemWithInstancesDesc& desc,
        const RenderVerticesDesc& vertices,
        uint32 vertexSize,
        unique_ptr<RenderItemWithInstances>& riwi) 
    {
        auto bi = GraphicsCore::GetInstance().GetFreePerObjBufferIndices().OccupyIndex();

        struct RenderItemWithInstancesUniquePtrEnabler : public RenderItemWithInstances { 
            RenderItemWithInstancesUniquePtrEnabler(uint32 bufferIndex, uint32 vertexSize, uint32 verticesCount, uint32 indicesCount, uint32 instancesCount, XMFLOAT4X3 transform, D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) :
                RenderItemWithInstances(bufferIndex, vertexSize, verticesCount, indicesCount, instancesCount, transform, primitiveTopology) {}
        };
        riwi = make_unique<RenderItemWithInstancesUniquePtrEnabler>(bi, vertexSize, vertices.verticesCount, vertices.indicesCount, desc.instancesCount, desc.transform, desc.primitiveTopology);
        riwi->VertexBuffer().Create(L"riwi_vertex", vertices.verticesCount, vertexSize, vertices.vertices);

        if (vertices.indicesCount != 0) {
            riwi->IndexBuffer().Create(L"riwi_index", vertices.indicesCount, kIndexSize, vertices.indices);
        }

        riwi->instances_.resize(desc.instancesCount);
        for (uint32 i = 0; i < desc.instancesCount; ++i) {
            riwi->instances_[i].materialIndex = desc.instancesDescs[i].material->BufferIndex();
            riwi->instances_[i].transform = desc.instancesDescs[i].transform;
        }
    }

}