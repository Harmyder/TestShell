#pragma once
#include "SDK\GpuBuffer.h"
#include "Utility\BufferStuff.h"

namespace Graphics {

    class Material;
    struct RenderVerticesDesc;

    struct RenderItemInstanceDesc
    {
        const XMFLOAT4X3& transform;
        Material *material;
    };
    struct RenderItemWithInstancesDesc
    {
        std::string& name;
        XMFLOAT4X4& transform;
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology;

        const RenderItemInstanceDesc* instancesDescs;
        uint32 instancesCount;
    };

    struct RenderItemInstance {
        XMFLOAT4X3 transform;
        uint32 materialIndex;
    };

    class RenderItemWithInstances : public Utility::BufferEntryDirty
    {
    public:
        static void Create(
            const RenderItemWithInstancesDesc& desc, 
            const RenderVerticesDesc& vertices,
            uint32 vertexSize,
            std::unique_ptr<RenderItemWithInstances>& priwi);
        
        D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;

        uint32 GetStartInstanceIndex() { return frInstsStart_; }
        const std::vector<RenderItemInstance>& GetInstances() { return instances_; }

        uint32 VerticesCount() const { return verticesCount_; }

        const XMFLOAT4X4& GetTransform() const { return transform_; }
        void SetTransform(const XMFLOAT4X4& transform);

        D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return primitiveTopology_; }

    private:
        RenderItemWithInstances(uint32 bufferIndex, uint32 vertexSize, uint32 verticesCount, uint32 instancesCount, XMFLOAT4X4 transform, D3D12_PRIMITIVE_TOPOLOGY primitiveTopology);

    private:
        std::string name_;
        XMFLOAT4X4 transform_;
        const D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_;
        GpuBuffer vertexBuffer_;
        const uint32 vertexSize_;
        const uint32 verticesCount_;
        const uint32 frInstsStart_;

        std::vector<RenderItemInstance> instances_;
    };
}
