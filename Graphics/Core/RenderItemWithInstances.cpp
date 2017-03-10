#include "stdafx.h"
#include "Core\RenderItemWithInstances.h"

#include "Core\RenderItem.h"
#include "Core\GraphicsCore.h"
#include "Core\FrameResource.h"

using namespace std;

namespace Graphics
{
    RenderItemWithInstances::RenderItemWithInstances(uint32 bufferIndex, uint32 vertexSize, uint32 verticesCount, uint32 instancesCount, XMFLOAT4X4 transform, D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) :
        BufferEntryDirty(bufferIndex),
        transform_(transform),
        primitiveTopology_(primitiveTopology),
        vertexSize_(vertexSize),
        verticesCount_(verticesCount),
        frInstsStart_(GraphicsCore::GetInstance().GetFrameResourcesNonConst()->AcquireInstsRange(instancesCount))
    {
        SetAllFramesDirty();
    }

    void RenderItemWithInstances::SetTransform(const XMFLOAT4X4& transform) {
        transform_ = transform;
        SetAllFramesDirty();
    }

    D3D12_VERTEX_BUFFER_VIEW RenderItemWithInstances::VertexBufferView() const {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
        vbv.StrideInBytes = vertexSize_;
        vbv.SizeInBytes = vertexSize_ * verticesCount_;
        return vbv;
    }

    void RenderItemWithInstances::Create(
        const RenderItemWithInstancesDesc& desc,
        const RenderVerticesDesc& vertices,
        uint32 vertexSize,
        CommandContext& cc,
        unique_ptr<RenderItemWithInstances>& riwi) 
    {
        auto bi = GraphicsCore::GetInstance().GetFreePerObjBufferIndices().OccupyIndex();

        struct RenderItemWithInstancesUniquePtrEnabler : public RenderItemWithInstances { 
            RenderItemWithInstancesUniquePtrEnabler(uint32 bufferIndex, uint32 vertexSize, uint32 verticesCount, uint32 instancesCount, XMFLOAT4X4 transform, D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) :
                RenderItemWithInstances(bufferIndex, vertexSize, verticesCount, instancesCount, transform, primitiveTopology) {}
        };
        riwi = make_unique<RenderItemWithInstancesUniquePtrEnabler>(bi, vertexSize, vertices.verticesCount, desc.instancesCount, desc.transform, desc.primitiveTopology);
        riwi->vertexBuffer_.Create(L"riwi_vertex", vertices.verticesCount, vertexSize, vertices.data, &cc);

        riwi->instances_.resize(desc.instancesCount);
        for (uint32 i = 0; i < desc.instancesCount; ++i) {
            riwi->instances_[i].materialIndex = desc.instancesDescs[i].material->BufferIndex();
            riwi->instances_[i].transform = desc.instancesDescs[i].transform;
        }
    }

}