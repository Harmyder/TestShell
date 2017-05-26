#pragma once
#include "SDK/GpuBuffer.h"
#include "Utility/BufferStuff.h"
#include "Core/RenderItem.h"

namespace Graphics {

    class Material;
    struct RenderVerticesDesc;

    struct RenderItemInstanceDesc
    {
        XMFLOAT4X3 transform;
        Material *material;
    };
    // Keep in sync with grtRenderSubItemWithInstancesDesc
    struct RenderItemWithInstancesDesc
    {
        std::string& name;
        XMFLOAT4X3& transform;
        Texture *texture;
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology;
        const RenderItemInstanceDesc* instancesDescs;
        uint32 instancesCount;
    };

    struct RenderItemInstance {
        XMFLOAT4X3 transform;
        uint32 materialIndex;
    };

    class RenderItemWithInstances : public RenderItemBase, public Utility::BufferEntryDirty
    {
    public:
        static void Create(
            const RenderItemWithInstancesDesc& desc, 
            const RenderVerticesDesc& vertices,
            uint32 vertexSize,
            std::unique_ptr<RenderItemWithInstances>& priwi);
        
        uint32 GetStartInstanceIndex() { return frInstsStart_; }
        const std::vector<RenderItemInstance>& GetInstances() { return instances_; }

        uint32 VerticesCount() const { return verticesCount_; }
        uint32 IndicesCount() const { return indicesCount_; }

        const XMFLOAT4X3& GetTransform() const { return transform_; }
        void SetTransform(const XMFLOAT4X3& transform);
        void SetInstancesTransforms(const XMFLOAT4X3* tranforms);

        const Texture* GetTexture() const { return texture_; }

        D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return primitiveTopology_; }

    private:
        RenderItemWithInstances(uint32 bufferIndex, uint32 vertexSize, uint32 verticesCount, uint32 indicesCount, uint32 instancesCount, XMFLOAT4X3 transform, D3D12_PRIMITIVE_TOPOLOGY primitiveTopology, const Texture* texture);

    private:
        std::string name_;
        uint32 verticesCount_;
        uint32 indicesCount_;
        XMFLOAT4X3 transform_;
        const D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_;
        const Texture* texture_;
        
        const uint32 frInstsStart_;
        std::vector<RenderItemInstance> instances_;
    };
}
