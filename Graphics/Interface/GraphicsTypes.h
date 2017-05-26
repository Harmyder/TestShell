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
    greCullMode::Type CullMode;
};

// TODO: do i need this types here?
struct grtVertexNormalTex {
    float Position[3];
    float Normal[3];
    float TexCoord[2];
};
struct grtVertexColor {
    float Position[3];
    float Color[4];
};
struct grtVertexParticles {
    float Position[3];
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
    grtRenderSubItemDescBase(const std::string& name, const XMFLOAT4X3& transform, grTexture texture) :
        name(name),
        transform(transform),
        texture(texture)
    {}

    const std::string& name;
    const XMFLOAT4X3& transform;
    grTexture texture;
};
struct grtRenderSubItemInstanceDesc
{
    grtRenderSubItemInstanceDesc(XMFLOAT4X3 transform, grMaterial material) : transform(transform), material(material) {}

    XMFLOAT4X3 transform;
    grMaterial material;
};
struct grtRenderSubItemWithInstancesDesc : grtRenderSubItemDescBase
{
    grtRenderSubItemWithInstancesDesc(
        const std::string& name, const XMFLOAT4X3& transform, grePrimitiveTopology::Type primitiveTopology, grTexture texture,
        const grtRenderSubItemInstanceDesc* instancesDescs, uint32 instancesCount) :
        grtRenderSubItemDescBase(name, transform, texture),
        primitiveTopology(primitiveTopology),
        instancesDescs(instancesDescs),
        instancesCount(instancesCount)
    {}

    grePrimitiveTopology::Type primitiveTopology;
    const grtRenderSubItemInstanceDesc* instancesDescs;
    uint32 instancesCount;
};
struct grtRenderSubItemDesc : grtRenderSubItemDescBase
{
    grtRenderSubItemDesc(const std::string& name, const XMFLOAT4X3& transform, grMaterial material, grTexture texture, grePrimitiveTopology::Type primitiveTopology) :
        grtRenderSubItemDescBase(name, transform, texture),
        material(material),
        primitiveTopology(primitiveTopology)
    {}

    grMaterial material;
    grePrimitiveTopology::Type primitiveTopology;
};
struct grtRenderSubItemParticlesDesc : grtRenderSubItemDescBase
{
    grtRenderSubItemParticlesDesc(const std::string& name, const XMFLOAT4X3& transform, grMaterial material, grTexture texture, float particleSize) :
        grtRenderSubItemDescBase(name, transform, texture),
        material(material),
        particleSize(particleSize)
    {}

    grMaterial material;
    float particleSize;
};
struct grtRenderItemDesc {
    const grtRenderVertices* renderVertices;
    uint32 renderVerticesCount;
    const grtRenderSubItemDesc* renderSubItems;
    uint32 renderSubItemsCount;
    const uint32* itemsToVertices;
};
struct grtRenderItemParticlesDesc {
    const grtRenderVertices* renderVertices;
    uint32 renderVerticesCount;
    const grtRenderSubItemParticlesDesc* renderSubItems;
    uint32 renderSubItemsCount;
    const uint32* itemsToVertices;
};
