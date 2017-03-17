#pragma once
#include "Interface\GraphicsConsts.h"
#include "Interface/GraphicsHandle.h"

struct grtPipelineStateDesc {
    greVertexType::Type VertexType;
    greShaderType::Type ShaderType;
    bool BlendEnable;
    bool DepthEnable;
    grePrimitiveTopologyType::Type PrimitiveTolopologyType;
    greFillMode::Type FillMode;
};

struct grtVertexNormalTex {
    float Position[3];
    float Normal[3];
    float TexCoord[2];
};
struct grtVertexColor {
    float Position[3];
    float Color[4];
};
struct grtRenderVertices {
    grtRenderVertices(const uint8* vertices, const uint32 verticesCount, const uint8* indices, const uint32 indicesCount) :
        vertices(vertices),
        verticesCount(verticesCount),
        indices(indices),
        indicesCount(indicesCount) {}

    const uint8* vertices;
    const uint32 verticesCount;
    const uint8* indices;
    const uint32 indicesCount;

    static const int kIndexSize;
};
struct grtRenderSubItemDescBase
{
    grtRenderSubItemDescBase(const std::string& name, const XMFLOAT4X3& transform, grePrimitiveTopology::Type primitiveTopology) :
        name(name),
        transform(transform),
        primitiveTopology(primitiveTopology)
    {}

    const std::string& name;
    const XMFLOAT4X3& transform;
    grePrimitiveTopology::Type primitiveTopology;
};
struct grtRenderSubItemInstanceDesc
{
    grtRenderSubItemInstanceDesc(const XMFLOAT4X3& transform, grMaterial material) : transform(transform), material(material) {}

    const XMFLOAT4X3& transform;
    grMaterial material;
};
struct grtRenderSubItemWithInstancesDesc : grtRenderSubItemDescBase
{
    grtRenderSubItemWithInstancesDesc(
        const std::string& name, const XMFLOAT4X3& transform, grePrimitiveTopology::Type primitiveTopology,
        const grtRenderSubItemInstanceDesc* instancesDescs, uint32 instancesCount) :
        grtRenderSubItemDescBase(name, transform, primitiveTopology),
        instancesDescs(instancesDescs),
        instancesCount(instancesCount)
    {}

    const grtRenderSubItemInstanceDesc* instancesDescs;
    uint32 instancesCount;
};
struct grtRenderSubItemDesc : grtRenderSubItemDescBase
{
    grtRenderSubItemDesc(const std::string& name, const XMFLOAT4X3& transform, grMaterial material, grePrimitiveTopology::Type primitiveTopology) :
        grtRenderSubItemDescBase(name, transform, primitiveTopology),
        material(material)
    {}

    grMaterial material;
};
struct grtRenderItemDesc {
    const grtRenderVertices* renderVertices;
    uint32 renderVerticesCount;
    const grtRenderSubItemDesc* renderSubItems;
    uint32 renderSubItemsCount;
    const uint32* itemsToVertices;
};
